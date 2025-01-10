#include <iostream>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>

class dns_entry
{
    // Add data members if needed
};

class dns_cache
{
    std::map<std::string, dns_entry> entries;
    mutable std::shared_mutex entry_mutex;

public:
    dns_entry find_entry(std::string const &domain) const
    {
        std::shared_lock lock(entry_mutex); // Use shared_lock for read access
        auto const it = entries.find(domain);
        return (it == entries.end()) ? dns_entry() : it->second;
    }

    void update_or_add_entry(std::string const &domain, dns_entry const &dns_details)
    {
        std::unique_lock lock(entry_mutex); // Use unique_lock for write access
        entries[domain] = dns_details;
    }
};

void reader(dns_cache const &cache, std::string const &domain)
{
    for (int i = 0; i < 10; ++i)
    {
        dns_entry entry = cache.find_entry(domain);
        std::cout << "Read entry for domain: " << domain << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void writer(dns_cache &cache, std::string const &domain, dns_entry const &entry)
{
    for (int i = 0; i < 10; ++i)
    {
        cache.update_or_add_entry(domain, entry);
        std::cout << "Updated entry for domain: " << domain << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
}

int main()
{
    dns_cache cache;
    dns_entry entry;

    std::jthread t1(reader, std::cref(cache), "example.com");
    std::jthread t2(writer, std::ref(cache), "example.com", entry);

    return 0;
}