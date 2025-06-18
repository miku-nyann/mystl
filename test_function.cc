// #include "function.h"

// #include <cstdio>

// void func1(int i) {
//     printf("#%d function pointer\n");
// }

// struct func2 {
//     void operator()(int i) const {
//         printf("#%d functor %d %d\n", i, _x, _y);
//     }
//     int _x;
//     int _y;
// };

// void repeatcall(const Function<void(int)>& f) {
//     f(1);
//     f(2);
// }

// int main() {
//     int x = 4;
//     int y = 2;
//     repeatcall([=](int i) {
//         printf("#%d lambda %d %d\n", i, x, y);
//     });
//     repeatcall(func2{x, y});
//     repeatcall(func1);
//     return 0;
// }
