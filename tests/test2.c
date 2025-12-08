struct S {
    int  a;
    double b;
};

struct S gs;

void use(struct S *p, int x) {
    int  *pa = &p->a;
    double *pb = &p->b;

    int va = x;
    double vb = (double)x;

    *pa = va;   // store with !tbaa "int"
    *pb = vb;   // store with !tbaa "double"
}
