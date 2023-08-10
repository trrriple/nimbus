#pragma once
#include "nimbus/core/common.hpp"

#include <chrono>

namespace nimbus {

class Stopwatch
{
   public:
    inline Stopwatch() noexcept
    {
        reset();
    }

    inline void reset() noexcept
    {
        m_ts = std::chrono::steady_clock::now();
    }

    inline double elapsed() const noexcept
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::steady_clock::now() - m_ts)
            .count() * 1E-6;
    }

    inline uint32_t elapsedMs() const noexcept
    {
        return elapsed() * 1000.0f;
    }

    inline uint32_t elapsedUs() const noexcept
    {
        return elapsed() * 1000000.0f;
    }

   private:
    std::chrono::time_point<std::chrono::steady_clock> m_ts;
};

}  // namespace nimbus