#include <thread>
#include <numeric>
#include <algorithm>
#include <vector>
#include <iostream>
#include <ranges>

template <typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
    unsigned long const length = std::distance(first, last);

    if (!length)
        return init;

    unsigned long const min_per_thread = 25;
    unsigned long const max_threads =
        (length + min_per_thread - 1) / min_per_thread;

    unsigned long const hardware_threads =
        std::thread::hardware_concurrency();

    unsigned long const num_threads =
        std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

    unsigned long const block_size = length / num_threads;

    std::vector<T> results(num_threads);
    std::vector<std::jthread> threads(num_threads - 1);

    Iterator block_start = first;
    for (unsigned long i = 0; i < (num_threads - 1); ++i)
    {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        threads[i] = std::jthread([block_start, block_end, &results, i] {
            results[i] = std::reduce(block_start, block_end);
        });
        block_start = block_end;
    }
    results[num_threads - 1] = std::reduce(block_start, last);

    return std::reduce(results.begin(), results.end(), init);
}

int main()
{
    std::vector<int> vi(10, 10);
    int sum = parallel_accumulate(vi.begin(), vi.end(), 5);
    std::cout << "sum=" << sum << std::endl;
}