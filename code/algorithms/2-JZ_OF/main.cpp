#include <iostream>
#include <cstdio>

inline int add_from_1_to_n(int n) {
    return n < 0 ? 0 : n + add_from_1_to_n(n - 1);
}

int main() {
    std::cout << add_from_1_to_n(4) << std::endl;
    return 0;
}