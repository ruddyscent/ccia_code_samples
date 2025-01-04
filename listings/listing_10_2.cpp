#include <mutex> // Include this header for std::mutex and std::lock_guard
#include <vector> // Include this header for std::vector
#include <algorithm> // Include this header for std::for_each
#include <execution> // Include this header for std::execution
#include <iostream> // Include this header for std::cout

class Y {
    int data;
public:
    Y() : data(0) {}
    int get_value() const {
        return data;
    }
    void increment() {
        ++data;
    }
};

class ProtectedY {
    std::mutex m;
    std::vector<Y> v;
public:
    void lock() {
        m.lock();
    }
    void unlock() {
        m.unlock();
    }
    std::vector<Y>& get_vec() {
        return v;
    }
    std::mutex& get_mutex() {
        return m;
    }
};

void increment_all(ProtectedY& data) {
    std::lock_guard<std::mutex> guard(data.get_mutex());
    auto& v = data.get_vec();
    std::for_each(std::execution::par_unseq, v.begin(), v.end(),
        [](Y& y) {
            y.increment();
        });
}

int main() {
    ProtectedY py;
    py.get_vec().resize(10); // Add 10 elements to the vector
    increment_all(py);
    for (const auto& y : py.get_vec()) {
        std::cout << y.get_value() << " ";
    }
    std::cout << std::endl;
    return 0;
}