#include <exception>
#include <memory> // For std::shared_ptr<>

struct empty_stack : std::exception
{
    const char *what() const throw();
};

template <typename T>
class threadsafe_stack
{
public:
    threadsafe_stack();
    threadsafe_stack(const threadsafe_stack &);
    threadsafe_stack &operator=(const threadsafe_stack &) = delete; // Assignment operator is deleted

    void push(T new_value);
    std::shared_ptr<T> pop();
    void pop(T &value);
    bool empty() const;
};

int main()
{
}
