#include "TBAATools.h"
#include "llvm/IR/LLVMContext.h"

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
