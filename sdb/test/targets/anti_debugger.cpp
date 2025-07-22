#include <cstdio>
#include <numeric>
#include <unistd.h>

void an_innocent_function()
{
    std::puts("Putting pineapple on pizza...");
}

void an_innocent_function_end() {}

int checksum()
{
    auto start = reinterpret_cast<volatile const char*>(&an_innocent_function);
    auto end = reinterpret_cast<volatile const char*>(&an_innocent_function_end);
    return std::accumulate(start, end, 0);
}

int main()
{
    auto safe = checksum();

    while (true)
    {
        sleep(1);
        if (checksum() == safe)
        {
            an_innocent_function();

        } else {

            puts("Putting pepperoni on pizza...");
        }
    }
}