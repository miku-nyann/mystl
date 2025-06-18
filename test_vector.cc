#include "vector.h"

static void dumpVector(const Vector<int>& v) {
    printf("    v=[");
    for (size_t i = 0; i < v.size(); ++i) {
        if (i == 0)
            printf("%d", v[i]);
        else 
            printf(" %d", v[i]);
    }
    printf("] size=%lld capacity=%lld\n", v.size(), v.capacity());
}

int main() {
    Vector<int> v;
    dumpVector(v);
    for (size_t i = 0; i < 16; ++i) {
        printf("v.pushBack(%lld)\n", i);
        v.pushBack(i);
        dumpVector(v);
    }

    v.erase(&v[3]);
    printf("v.erase(&v[3])\n");
    dumpVector(v);

    v.insert(&v[5], 3, 100);
    printf("v.insert(&v[5], 3, 100)\n");
    dumpVector(v);

    size_t sz = v.size();
    for (size_t i = 0; i < sz; ++i) {
        printf("v.popBack()\n");
        v.popBack();
        dumpVector(v);
    }
}