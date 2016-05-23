//
// Created by Ivan Shynkarenka on 15.04.2016.
//

#include "catch.hpp"

#include "threads/named_semaphore.h"

#include <atomic>
#include <thread>

using namespace CppCommon;

TEST_CASE("Named semaphore locker", "[CppCommon][Threads]")
{
    int items_to_produce = 10000;
    int producers_count = 8;
    std::atomic<int> crc(0);

    // Caclulate result value
    int result = 0;
    for (int i = 0; i < items_to_produce; ++i)
        result += i;

    // Named semaphore master
    NamedSemaphore lock("named_semaphore_test", 4);

    // Start producers threads
    std::vector<std::thread> producers;
    for (int producer = 0; producer < producers_count; ++producer)
    {
        producers.push_back(std::thread([&crc, producer, items_to_produce, producers_count]()
        {
            // Named semaphore slave
            NamedSemaphore lock("named_semaphore_test", 4);

            int items = (items_to_produce / producers_count);
            for (int i = 0; i < items; ++i)
            {
                Locker<NamedSemaphore> locker(lock);
                crc += (producer * items) + i;
            }
        }));
    }

    // Wait for all producers threads
    for (auto& producer : producers)
        producer.join();

    // Check result
    REQUIRE(crc == result);
}
