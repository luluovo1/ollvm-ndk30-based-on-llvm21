#ifndef OBFUSCATION_BOGUSCONTROLFLOW_H
#define OBFUSCATION_BOGUSCONTROLFLOW_H

#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/Obfuscation/ObfuscationOptions.h"

namespace llvm {
FunctionPass *createBogusControlFlowPass(ObfuscationOptions *argsOptions);
}

#endif
