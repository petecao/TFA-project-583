#pragma once

#include "llvm/IR/Instruction.h"
#include "llvm/IR/Metadata.h"

#include "../AliasAnalysis/AliasAnalysis.h"

// Return the TBAA metadata node for an instruction or nullptr if there is none
llvm::MDNode *getTBAANode(const llvm::Instruction *I);

// Return true if TBAA cannot rule out aliasing (i.e. "may alias")
// Return false if TBAA proves they do NOT alias
bool mayAliasByTBAA(llvm::MDNode *A, llvm::MDNode *B);

// Returns true if we should allow these two alias nodes to be merged,
// considering their TBAA tags. If TBAA is missing or inconclusive,
// we conservatively return true.
bool shouldMergeByTBAA(AliasNode *A, AliasNode *B);

/// Return true if it's *safe to keep* FuncNode as a target for this icall:
///  - true  => may alias / unknown (do NOT prune)
///  - false => proven no-alias by TBAA (PRUNE)
bool tbaaCompatibleForICall(AliasNode *ICallNode, AliasNode *FuncNode);
