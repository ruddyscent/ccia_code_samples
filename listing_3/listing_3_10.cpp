#include <iostream>
#include <mutex>

class Y
{
private:
    int some_detail;
    mutable std::mutex m;

    int get_detail() const
    {
        std::scoped_lock lock(m);
        return some_detail;
    }

public:
    Y(int sd) : some_detail(sd) {}

    friend bool operator==(Y const &lhs, Y const &rhs)
    {
        if (&lhs == &rhs)
            return true;
        int const lhs_value = lhs.get_detail();
        int const rhs_value = rhs.get_detail();
        return lhs_value == rhs_value;
    }
};

int main()
{
    Y y1(10);
    Y y2(10);
    if (y1 == y2)
    {
        std::cout << "y1 is equal to y2" << std::endl;
    }
    else
    {
        std::cout << "y1 is not equal to y2" << std::endl;
    }
}
