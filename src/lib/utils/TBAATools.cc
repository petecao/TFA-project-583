#include "TBAATools.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Constants.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/MDBuilder.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"

#include "../AliasAnalysis/AliasAnalysis.h"

using namespace llvm;

// Fetch TBAA metadata attached to an instruction (unchanged).
MDNode *getTBAANode(const Instruction *I) {
    if (!I) return nullptr;
    return I->getMetadata(LLVMContext::MD_tbaa);
}

// ----------------- Helpers for struct-path TBAA -----------------

// Root node: 0 operands OR exactly 1 MDString operand.
static bool isRootNode(MDNode *N) {
    if (!N) return false;
    unsigned NumOps = N->getNumOperands();
    if (NumOps == 0)
        return true;
    if (NumOps == 1 && isa<MDString>(N->getOperand(0)))
        return true;
    return false;
}

// Scalar type descriptor: 2 or 3 operands,
// operand 0 = MDString name, operand 1 = parent MDNode.
static bool isScalarDesc(MDNode *N) {
    if (!N) return false;
    unsigned NumOps = N->getNumOperands();
    if (NumOps != 2 && NumOps != 3)
        return false;
    if (!isa<MDString>(N->getOperand(0)))
        return false;
    if (!isa<MDNode>(N->getOperand(1)))
        return false;
    return true;
}

// Struct type descriptor: odd number of operands > 1,
// operand 0 = MDString name, then (fieldTy, fieldOffset)*.
static bool isStructDesc(MDNode *N) {
    if (!N) return false;
    unsigned NumOps = N->getNumOperands();
    if (NumOps <= 1)
        return false;
    if ((NumOps & 1) == 0) // must be odd
        return false;
    if (!isa<MDString>(N->getOperand(0)))
        return false;
    return true;
}

// Given an access tag (BaseTy, AccessTy, Offset, [const]),
// extract BaseTy and Offset. Returns false if malformed.
static bool getBaseAndOffsetFromTag(MDNode *Tag,
                                    MDNode *&Base,
                                    uint64_t &Offset) {
    Base = nullptr;
    Offset = 0;

    if (!Tag)
        return false;

    if (Tag->getNumOperands() < 3)
        return false;

    auto *BaseNode = dyn_cast<MDNode>(Tag->getOperand(0));
    if (!BaseNode)
        return false;

    auto *CI = mdconst::dyn_extract<ConstantInt>(Tag->getOperand(2));
    if (!CI)
        return false;

    Base = BaseNode;
    Offset = CI->getZExtValue();
    return true;
}

// Get the TBAA root of an access tag by following the AccessTy's
// parent chain until we hit a root node.
static MDNode *getTBAARootFromTag(MDNode *Tag) {
    if (!Tag)
        return nullptr;

    if (Tag->getNumOperands() < 2)
        return nullptr;

    auto *AccessTy = dyn_cast<MDNode>(Tag->getOperand(1));
    if (!AccessTy)
        return nullptr;

    MDNode *Cur = AccessTy;
    // Follow scalar parents until root.
    while (Cur && !isRootNode(Cur)) {
        if (!isScalarDesc(Cur))
            return nullptr;

        auto *Parent = dyn_cast<MDNode>(Cur->getOperand(1));
        if (!Parent)
            return nullptr;
        Cur = Parent;
    }
    return Cur;
}

// ImmediateParent(BaseTy, Offset) as defined in the spec:
//
//  If BaseTy is scalar:
//    - Offset must be 0
//    - Parent is (ParentTy, 0)
//  If BaseTy is struct:
//    - Find field at or before Offset (last field whose fieldOffset <= Offset)
//    - Parent is (FieldTy, Offset - fieldOffset)
//
// Returns true if a parent exists; false if undefined/unknown.
static bool immediateParent(MDNode *BaseTy,
                            uint64_t Offset,
                            MDNode *&OutBaseTy,
                            uint64_t &OutOffset) {
    OutBaseTy = nullptr;
    OutOffset = 0;

    if (!BaseTy)
        return false;

    // Scalar descriptor case
    if (isScalarDesc(BaseTy)) {
        if (Offset != 0)
            return false; // undefined
        auto *Parent = dyn_cast<MDNode>(BaseTy->getOperand(1));
        if (!Parent)
            return false;
        OutBaseTy = Parent;
        OutOffset = 0;
        return true;
    }

    // Struct descriptor case
    if (isStructDesc(BaseTy)) {
        unsigned NumOps = BaseTy->getNumOperands();

        MDNode *ChosenFieldTy = nullptr;
        uint64_t ChosenFieldOff = 0;
        bool Found = false;

        // Operands: 0 = name, then (fieldTy, fieldOffset)*.
        for (unsigned i = 1; i + 1 < NumOps; i += 2) {
            auto *FieldTy = dyn_cast<MDNode>(BaseTy->getOperand(i));
            auto *CI = mdconst::dyn_extract<ConstantInt>(BaseTy->getOperand(i + 1));
            if (!FieldTy || !CI)
                continue;

            uint64_t FieldOff = CI->getZExtValue();

            if (FieldOff > Offset)
                break;

            ChosenFieldTy = FieldTy;
            ChosenFieldOff = FieldOff;
            Found = true;
        }

        if (!Found)
            return false;

        OutBaseTy = ChosenFieldTy;
        OutOffset = Offset - ChosenFieldOff;
        return true;
    }

    // Root or unknown node types have no parent in this model.
    if (isRootNode(BaseTy))
        return false;

    return false;
}

// Implement the struct-path alias rule for two access tags.
//
// Return true  => TBAA says they may alias OR we can't prove no-alias.
// Return false => TBAA proves they do NOT alias.
bool mayAliasByTBAA(MDNode *A, MDNode *B) {
    // Basic checks
    if (!A || !B) return true; // No info => May Alias
    if (A == B)   return true; // Same tag => May Alias

    MDNode *BaseA = nullptr, *BaseB = nullptr;
    uint64_t OffA = 0, OffB = 0;

    // Parse tags
    if (!getBaseAndOffsetFromTag(A, BaseA, OffA) ||
        !getBaseAndOffsetFromTag(B, BaseB, OffB)) {
        return true; // Malformed => May Alias
    }

    // If the base types are identical (e.g., both are accessing "Struct S"), 
    // but the offsets are different, strict aliasing says they cannot overlap.
    // (This works because TBAA nodes are distinct for Unions, so simple offset checks are safe).
    if (BaseA == BaseB && OffA != OffB) {
        return false;
    }

    MDNode *RootA = getTBAARootFromTag(A);
    MDNode *RootB = getTBAARootFromTag(B);

    // If we have two valid roots and they are DIFFERENT, assume disjoint
    // prune
    if (RootA && RootB && RootA != RootB) {
         return false;
    }

    // Fallback: if roots are missing/malformed, we must assume aliasing.
    if (!RootA || !RootB) return true;

    // Build Full Ancestry Chain for A
    // We store pairs of (MDNode*, Offset)
    SmallVector<std::pair<MDNode*, uint64_t>, 8> ChainA;
    {
        MDNode *CurBase = BaseA;
        uint64_t CurOff = OffA;
        while (CurBase) {
            ChainA.emplace_back(CurBase, CurOff);
            
            MDNode *ParentBase = nullptr;
            uint64_t ParentOff = 0;
            if (!immediateParent(CurBase, CurOff, ParentBase, ParentOff))
                break;
            CurBase = ParentBase;
            CurOff  = ParentOff;
        }
    }

    // Build Full Ancestry Chain for B
    SmallVector<std::pair<MDNode*, uint64_t>, 8> ChainB;
    {
        MDNode *CurBase = BaseB;
        uint64_t CurOff = OffB;
        while (CurBase) {
            ChainB.emplace_back(CurBase, CurOff);
            
            MDNode *ParentBase = nullptr;
            uint64_t ParentOff = 0;
            if (!immediateParent(CurBase, CurOff, ParentBase, ParentOff))
                break;
            CurBase = ParentBase;
            CurOff  = ParentOff;
        }
    }

    // Check Reachability (The "OR" logic from the docs)
    
    // Check if (BaseA, OffA) is reachable from B (i.e., is StartA inside ChainB?)
    // This handles cases where B is a struct and A is a member of that struct.
    bool A_is_ancestor_of_B = false;
    for (auto &NodeInB : ChainB) {
        if (NodeInB.first == BaseA && NodeInB.second == OffA) {
            A_is_ancestor_of_B = true;
            break;
        }
    }

    // Check if (BaseB, OffB) is reachable from A (i.e., is StartB inside ChainA?)
    bool B_is_ancestor_of_A = false;
    for (auto &NodeInA : ChainA) {
        if (NodeInA.first == BaseB && NodeInA.second == OffB) {
            B_is_ancestor_of_A = true;
            break;
        }
    }

    // "aliases ... if either ... is reachable from ... or vice versa"
    return A_is_ancestor_of_B || B_is_ancestor_of_A;
}

// Uses mayAliasByTBAA on the AliasNodes' TBAATag fields.
// Returns true if it's safe to merge (alias or unknown),
// false if TBAA proves they are disjoint.
bool shouldMergeByTBAA(AliasNode *A, AliasNode *B) {
    if (!A || !B)
        return true;

    MDNode *TA = A->TBAATag;
    MDNode *TB = B->TBAATag;

    // No metadata => can't use TBAA to block merges.
    if (!TA || !TB)
        return true;

#ifdef ENABLE_DEBUG
    OP << "[TBAA] comparing tags:\n";
    OP << "  TA: "; TA->print(OP); OP << "\n";
    OP << "  TB: "; TB->print(OP); OP << "\n";
#endif

    // If TBAA says "may alias" (or unknown), we allow the merge.
    // If it returns false, that's "proven no-alias" => block merge.
    return mayAliasByTBAA(TA, TB);
}

bool tbaaCompatibleForICall(AliasNode *ICallNode, AliasNode *FuncNode) {
    if (!ICallNode || !FuncNode)
        return true;  // no info -> cannot prune

    llvm::MDNode *CallTag = ICallNode->TBAATag;
    llvm::MDNode *FuncTag = FuncNode->TBAATag;

    // If either side has no TBAA metadata, we cannot prove no-alias.
    if (!CallTag || !FuncTag)
        return true;

#ifdef ENABLE_DEBUG
    OP << "[TBAA-ICALL] Comparing icall tag vs func tag:\n";
    OP << "  ICallNode tag: ";
    CallTag->print(OP);
    OP << "\n  FuncNode tag: ";
    FuncTag->print(OP);
    OP << "\n";
#endif

    // mayAliasByTBAA == true  => alias or unknown -> KEEP
    // mayAliasByTBAA == false => proven no-alias  -> PRUNE
    return mayAliasByTBAA(CallTag, FuncTag);
}
