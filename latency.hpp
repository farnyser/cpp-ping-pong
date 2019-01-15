#ifndef PING_PONG_LATENCY_HPP
#define PING_PONG_LATENCY_HPP

#include <iostream>
#include <chrono>
#include <iomanip>
#include <array>

template <std::uint64_t TMaxDurationNs, size_t TBucketCount = 1000000>
class latency
{
private:
    size_t count { 0 };
    std::chrono::nanoseconds bucketDuration{TMaxDurationNs / TBucketCount};
    std::array<size_t, TBucketCount> buckets{0};

public:
    template <typename T>
    void add(T duration)
    {
        auto bucketIndex = duration / bucketDuration;
        if(bucketIndex > TBucketCount)
            throw std::runtime_error("Latency computing failed");

        buckets[bucketIndex]++;
        count++;
    }

    template <typename O, typename TRatio = std::chrono::microseconds>
    void generate(O& output)
    {
        std::cout   << std::setw(20) << "duration"
                    << std::setw(21) << "percentile"
                    << std::setw(20) << "count"
                    << std::endl;

        size_t cum = 0;
        double mean = 0;

        for(size_t i = 0; i < TBucketCount; i++)
        {
            auto current = buckets[i];
            if (current == 0)
                continue;

            cum += current;

            auto percentile = (double)cum / count * 100.0;
            std::cout   << std::setw(20) << std::chrono::duration_cast<TRatio>((i+1) * bucketDuration).count()
                        << std::setw(20) << percentile << "%"
                        << std::setw(20) << current
                        << std::endl;

            mean = (mean * (cum-current) + current * (i+1)) / cum;
        }

        std::cout << "#Mean " << std::chrono::duration_cast<TRatio>(mean * bucketDuration).count() << std::endl;
    }
};

#endif //PING_PONG_LATENCY_HPP
