#pragma once

#include "llvm/IR/Instruction.h"
#include "llvm/IR/Metadata.h"

/// Return the TBAA metadata node for an instruction or nullptr if there is none
llvm::MDNode *getTBAANode(const llvm::Instruction *I);

/// Return true if TBAA cannot rule out aliasing (i.e. "may alias")
/// Return false if TBAA proves they do NOT alias
bool mayAliasByTBAA(llvm::MDNode *A, llvm::MDNode *B);
