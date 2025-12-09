#include "TBAATools.h"
#include "llvm/IR/LLVMContext.h"

#include "../AliasAnalysis/AliasAnalysis.h"
#include <iostream>

using namespace llvm;

MDNode *getTBAANode(const Instruction *I) {
    if (!I) return nullptr;
    // LLVM 17+/20 way to get TBAA metadata
    return I->getMetadata(LLVMContext::MD_tbaa);
}

// This version is very simple
// - If either side has no TBAA => "may alias" (true) since no conclusive information was found
// - If same node => "may alias" (true)
// - If different nodes => "no alias" (false)
bool mayAliasByTBAA(MDNode *A, MDNode *B) {
    if (!A || !B) return true;   // no info => may alias
    if (A == B)   return true;   // same tag => may alias

    // Different TBAA nodes -> treat as no-alias for now
    return false;
}

bool shouldMergeByTBAA(AliasNode *A, AliasNode *B) {
    if (!A || !B) {
        return true;
    }

    llvm::MDNode *TA = A->TBAATag;
    llvm::MDNode *TB = B->TBAATag;

    // No metadata on one or both sides so we can't prove anything
    if (!TA || !TB) {
        return true;
    }

#ifdef ENABLE_DEBUG
    OP << "[TBAA] comparing tags:\n";
    OP << "  TA: "; TA->print(OP); OP << "\n";
    OP << "  TB: "; TB->print(OP); OP << "\n";
#endif

    // Delegate to your TBAA logic
    // If TBAA says no alias, we block the merge
    return mayAliasByTBAA(TA, TB);
}
