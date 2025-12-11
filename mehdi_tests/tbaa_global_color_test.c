// mehdi_tests/tbaa_fp_merge_ok.c
//
// Goal: exercise a case where both alias nodes have TBAA = YES
// and the tags are *identical*, so TBAA does NOT block the merge.

typedef void (*fp_t)(void);

int gi;

// Two callbacks (same signature)
void cb_one(void)  { gi = 1; }
void cb_two(void)  { gi = 2; }

// Single struct type: both slots share the same layout & field types
struct Slot {
    int pad;     // int-typed field
    fp_t fp;     // function pointer field (usually untagged)
};

// Two globals of the *same* struct type
struct Slot slot1 = { 0, cb_one };
struct Slot slot2 = { 0, cb_two };

void drive(int cond) {
    fp_t fp;

    // These two stores should both carry the *same* TBAA path
    // for the "fp" field of `struct Slot`, because the struct
    // type is the same and the field offset is the same.
    //
    // Your HandleStore(StoreInst*) should:
    //   - Attach the same TBAATag to the memory nodes for slot1.fp and slot2.fp
    //   - So n1->TBAATag == n2->TBAATag (pointer-equal MDNode)
    slot1.fp = cb_one;
    slot2.fp = cb_two;

    if (cond) {
        // Access via slot1
        slot1.pad = 10;   // will get int TBAA, but we don't rely on it
        fp = slot1.fp;
    } else {
        // Access via slot2
        slot2.pad = 20;   // also int TBAA
        fp = slot2.fp;
    }

    // Indirect call; your analysis will build alias sets and
    // eventually try to merge cb_one and cb_two via the nodes
    // associated with slot1.fp and slot2.fp.
    fp();
}
