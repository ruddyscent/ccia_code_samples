#include <mutex>
#include <vector>
#include <algorithm>
#include <execution>
#include <iostream>

class X {
    mutable std::mutex m;
    int data;
public:
    X() : data(0) {}
    int get_value() const {
        std::lock_guard<std::mutex> guard(m);
        return data;
    }
    void increment() {
        std::lock_guard<std::mutex> guard(m);
        ++data;
    }
};

void increment_all(std::vector<X>& v) {
    std::for_each(std::execution::par, v.begin(), v.end(),
        [](X& x) {
            x.increment();
        });
}

int main() {
    std::vector<X> vec(10);
    increment_all(vec);
    for (const auto& x : vec) {
        std::cout << x.get_value() << " ";
    }
    std::cout << std::endl;
    return 0;
}