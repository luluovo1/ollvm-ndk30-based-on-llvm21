// BogusControlFlow.cpp
// BCF (Bogus Control Flow) pass
//
// 原理：对每个 BasicBlock 以不透明谓词（永为真，但IDA静态分析无法证伪）为条件
// 制造菱形分支结构：
//
//   EntryBB
//    |        \
//    v         v
//  OrigBB   BogBB（垃圾指令） --> OrigBB
//    |       /
//    v      /
//  [SuccBB]
//
// IDA 静态分析会把 BogBB 当作可达路径，引入大量额外分析节点，
// 触发 Hex-Rays "optimizations loop too much" 或显著降低反编译质量。

// ---- 先包含所有 LLVM 头文件，防止 CryptoUtils.h 的宏污染它们 ----
#include "llvm/Transforms/Obfuscation/BogusControlFlow.h"
#include "llvm/Transforms/Obfuscation/ObfuscationOptions.h"
#include "llvm/Transforms/Obfuscation/Utils.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

// ---- CryptoUtils 最后包含，避免其 R/S/Maj/Ch 等宏污染上方头文件 ----
#include "llvm/CryptoUtils.h"
// CryptoUtils.h 为 SHA256 定义了大量单字母/短名宏，会与 APInt.h 等冲突，立即清理
#ifdef R
#undef R
#endif
#ifdef S
#undef S
#endif
#ifdef Maj
#undef Maj
#endif
#ifdef Ch
#undef Ch
#endif
#ifdef Sigma0
#undef Sigma0
#endif
#ifdef Sigma1
#undef Sigma1
#endif
#ifdef Gamma0
#undef Gamma0
#endif
#ifdef Gamma1
#undef Gamma1
#endif
#ifdef MIN
#undef MIN
#endif
#ifdef RND
#undef RND
#endif
#ifdef RORc
#undef RORc
#endif

#include <vector>

using namespace llvm;


// 混淆概率：0-100，默认 70。用 -mllvm -irobf-bcf_prob=X 覆盖。
static cl::opt<unsigned>
    BCFProb("irobf-bcf_prob", cl::init(70), cl::NotHidden,
            cl::desc("Probability (0-100) that each basic block gets BCF "
                     "applied. Default: 70"),
            cl::ZeroOrMore);

// ============================================================
// 构造不透明谓词：(x*(x+1)) % 2 == 0，在整数域上永远为真
// 通过 volatile load 阻止 LLVM 将其折叠为常量，同时也欺骗 IDA 的值域分析
// ============================================================
static Value *buildOpaqueTrue(IRBuilder<> &B, LLVMContext &Ctx) {
  AllocaInst *Alloca = B.CreateAlloca(B.getInt64Ty(), nullptr, "bcf_x");
  Alloca->setAlignment(Align(8));

  // 用随机偶数初始化（每次编译不同，增加多样性）
  uint64_t Seed = cryptoutils->get_uint64_t();
  Seed = (Seed | 1) ^ 1; // 确保是偶数
  B.CreateStore(ConstantInt::get(B.getInt64Ty(), Seed), Alloca);

  // volatile load：强制运行时读取，阻止 LLVM 常量折叠
  LoadInst *LI = B.CreateLoad(B.getInt64Ty(), Alloca, "bcf_x_val");
  LI->setVolatile(true);

  // (x * (x + 1)) % 2 == 0  ←→ 永远为 0，即永远为 true
  Value *X     = LI;
  Value *XP1   = B.CreateAdd(X, ConstantInt::get(B.getInt64Ty(), 1), "bcf_xp1");
  Value *Prod  = B.CreateMul(X, XP1, "bcf_prod");
  Value *Rem   = B.CreateURem(Prod, ConstantInt::get(B.getInt64Ty(), 2), "bcf_rem");
  return B.CreateICmpEQ(Rem, ConstantInt::get(B.getInt64Ty(), 0), "bcf_cond");
}

// ============================================================
// 向 BogusBlock 填充永远不会执行的垃圾指令
// IDA 仍然会尝试分析这些指令，增加分析负担
// ============================================================
static void fillBogusBB(BasicBlock *BogBB, LLVMContext &Ctx) {
  IRBuilder<> B(BogBB);
  // 插入 4 条随机算术垃圾指令（结果没有被使用）
  for (int i = 0; i < 4; i++) {
    uint32_t R1 = cryptoutils->get_uint32_t();
    uint32_t R2 = cryptoutils->get_uint32_t();
    Value *A    = ConstantInt::get(Type::getInt32Ty(Ctx), R1);
    Value *Bv   = ConstantInt::get(Type::getInt32Ty(Ctx), R2);
    Value *Mul  = B.CreateMul(A, Bv, "junk_mul");
    /* unused */ B.CreateXor(Mul, A, "junk_xor");
  }
  // 注意：不在这里加终止指令，调用者负责添加 BranchInst
}

namespace {

struct BogusControlFlow : public FunctionPass {
  static char ID;
  ObfuscationOptions *ArgsOptions;

  BogusControlFlow(ObfuscationOptions *argsOptions) : FunctionPass(ID) {
    this->ArgsOptions = argsOptions;
  }

  StringRef getPassName() const override { return {"BogusControlFlow"}; }

  bool runOnFunction(Function &F) override {
    if (F.isDeclaration() || F.isIntrinsic())
      return false;

    // 检查是否对此函数应用 BCF
    auto Opt = ArgsOptions->toObfuscate(ArgsOptions->bcfOpt(), &F);
    if (!Opt.isEnabled())
      return false;

    bool Changed = false;

    // 收集所有合法的目标块（快照，防止迭代时被修改）
    SmallVector<BasicBlock *, 16> Targets;
    for (BasicBlock &BB : F) {
      if (BB.isLandingPad())
        continue;
      // 只处理有正常终止指令的块（不处理只有一条指令的块，太小了没意义）
      if (BB.size() < 2)
        continue;
      Instruction *Term = BB.getTerminator();
      if (isa<ReturnInst>(Term) || isa<BranchInst>(Term) ||
          isa<SwitchInst>(Term))
        Targets.push_back(&BB);
    }

    unsigned Prob = BCFProb.getValue();
    if (Prob > 100) Prob = 100;

    for (BasicBlock *BB : Targets) {
      if ((cryptoutils->get_uint32_t() % 100) >= Prob)
        continue;

      if (applyBCFToBlock(BB, F))
        Changed = true;
    }

    if (Changed)
      fixStack(&F);

    return Changed;
  }

  // ============================================================
  // 对一个 BasicBlock 注入菱形 BCF 结构（安全，无真实环路）
  //
  // Before:
  //   [EntryBB] ---> [OrigBB ...] ---> [SuccBB]
  //
  // After:
  //   [EntryBB]
  //     |         \
  //     v          v
  //   [OrigBB]  [BogBB]垃圾指令
  //     ^           |
  //      \----------/  (BogBB 无条件跳回 OrigBB)
  //     |
  //   [SuccBB]
  //
  // EntryBB 用不透明谓词做条件分支：
  //   OpaqueTrue  -> OrigBB（实际执行路径）
  //   OpaqueFalse -> BogBB（永远不会到达，但 IDA 不知道）
  // ============================================================
  bool applyBCFToBlock(BasicBlock *BB, Function &F) {
    LLVMContext &Ctx = F.getContext();

    // 1. 把 BB 从第一条非 PHI 指令处分裂
    //    BB = EntryBB（剩余部分）
    //    OrigBB = 原始内容从非PHI开始
    BasicBlock *OrigBB =
        BB->splitBasicBlock(BB->getFirstNonPHI(), "bcf.orig");

    // 2. 创建 BogusBlock（在 OrigBB 之前插入，保持排列整洁）
    BasicBlock *BogBB =
        BasicBlock::Create(Ctx, "bcf.bogus", &F, OrigBB);

    // 3. 填充垃圾指令，然后 BogBB 无条件跳到 OrigBB
    fillBogusBB(BogBB, Ctx);
    {
      IRBuilder<> B(BogBB);
      B.CreateBr(OrigBB);
    }

    // 4. 把 EntryBB（即原 BB）原来的无条件跳转替换为
    //    基于不透明谓词的条件分支
    Instruction *OldTerm = BB->getTerminator(); // 这是 splitBasicBlock 插入的 br OrigBB
    {
      IRBuilder<> B(OldTerm);
      Value *Cond = buildOpaqueTrue(B, Ctx);
      B.CreateCondBr(Cond, OrigBB, BogBB);
    }
    OldTerm->eraseFromParent();

    return true;
  }
};

} // anonymous namespace

char BogusControlFlow::ID = 0;

FunctionPass *llvm::createBogusControlFlowPass(ObfuscationOptions *argsOptions) {
  return new BogusControlFlow(argsOptions);
}
