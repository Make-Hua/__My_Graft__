#include "My_Allocator.h"
#include <iostream>
#include <vector>

int main() {
    std::vector<int, MyAllocator<int>> vec;

    for (int i = 0; i < 100; ++i) {
        vec.push_back(i);
    }

    for (auto x : vec) std::cout << x << " ";

    return 0;
}
