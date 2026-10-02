#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#include "RootishArrayStack.h"

using ods::RootishArrayStack;

static void test_basic_sequence() {
    RootishArrayStack<int> s;
    assert(s.empty());
    assert(s.size() == 0);

    s.add(10);
    s.add(20);
    s.add(30);
    assert(s.size() == 3);
    assert(s.get(0) == 10);
    assert(s.get(1) == 20);
    assert(s.get(2) == 30);

    s.add(1, 15);
    assert(s.get(0) == 10);
    assert(s.get(1) == 15);
    assert(s.get(2) == 20);
    assert(s.get(3) == 30);

    assert(s.remove(1) == 15);
    assert(s.size() == 3);
    assert(s.get(0) == 10);
    assert(s.get(1) == 20);
    assert(s.get(2) == 30);

    s.clear();
    assert(s.empty());
    assert(s.size() == 0);
}

static void test_randomized_stress() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    for (int case_id = 0; case_id < 2000; ++case_id) {
        RootishArrayStack<int> stack;
        std::vector<int> reference;

        for (int op = 0; op < 100; ++op) {
            int kind = std::rand() % 6;

            if (kind == 0 || reference.empty()) {
                int value = std::rand() % 1000;
                stack.add(value);
                reference.push_back(value);
            } else if (kind == 1) {
                int index = std::rand() % (reference.size() + 1);
                int value = std::rand() % 1000;
                stack.add(index, value);
                reference.insert(reference.begin() + index, value);
            } else if (kind == 2) {
                int index = std::rand() % reference.size();
                assert(stack.get(index) == reference[index]);
            } else if (kind == 3) {
                int index = std::rand() % reference.size();
                int value = std::rand() % 1000;
                assert(stack.set(index, value) == reference[index]);
                reference[index] = value;
            } else if (kind == 4) {
                int index = std::rand() % reference.size();
                assert(stack.remove(index) == reference[index]);
                reference.erase(reference.begin() + index);
            } else {
                int index = std::rand() % reference.size();
                int value = std::rand() % 1000;
                stack.set(index, value);
                reference[index] = value;
            }

            assert(stack.size() == static_cast<int>(reference.size()));
            for (int i = 0; i < static_cast<int>(reference.size()); ++i) {
                assert(stack.get(i) == reference[i]);
            }
        }
    }
}

int main() {
    test_basic_sequence();
    test_randomized_stress();
    std::cout << "RootishArrayStack tests passed\n";
    return 0;
}
