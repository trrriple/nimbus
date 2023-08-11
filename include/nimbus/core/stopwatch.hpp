#pragma once
#include "nimbus/core/common.hpp"

#include <chrono>
#include <unordered_map>
#include <mutex>

namespace nimbus
{

class Stopwatch : public refCounted
{
   public:
    inline Stopwatch() noexcept
    {
        reset();
        m_splitElapsed_s = 0.0;
        m_lastSplit_s    = 0.0;
    }

    inline void reset() noexcept
    {
        m_ts = std::chrono::steady_clock::now();
    }

    inline double elapsed() const noexcept
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::steady_clock::now() - m_ts)
                   .count()
               * 1E-6;
    }

    inline uint32_t elapsedMs() const noexcept
    {
        return elapsed() * 1000.0f;
    }

    inline uint32_t elapsedUs() const noexcept
    {
        return elapsed() * 1000000.0f;
    }

    inline float split() noexcept
    {
        double tNow_s    = elapsed();
        m_lastSplit_s    = tNow_s - m_splitElapsed_s;
        m_splitElapsed_s = tNow_s;
        return m_lastSplit_s;
    }

    inline void splitAndSave() noexcept
    {
        split();
        saveSplit();
    }

    inline void saveSplit() noexcept
    {
        std::scoped_lock<std::mutex> lock(m_splitMtx);
        m_savedSplit = m_lastSplit_s;
    }

    inline void saveSplitOverride(double split)
    {
        std::scoped_lock<std::mutex> lock(m_splitMtx);
        m_savedSplit = split;
    }

    inline float getLastSplit() const noexcept
    {
        return m_lastSplit_s;
    }

    inline float getLastSavedSplit() const noexcept
    {
        std::scoped_lock<std::mutex> lock(m_splitMtx);
        return m_savedSplit;
    }

   private:
    std::chrono::time_point<std::chrono::steady_clock> m_ts;
    double                                             m_splitElapsed_s;
    float                                              m_lastSplit_s;
    inline static std::mutex                           m_splitMtx;
    float                                              m_savedSplit;

    friend class WatchBank;
};

class StopWatchBank
{
   public:
    StopWatchBank() noexcept = default;
    ~StopWatchBank() noexcept
    {
        std::scoped_lock<std::mutex> lock(m_bankMtx);
        m_bank.clear();
    }

    ref<Stopwatch> newSw(const char* name) noexcept
    {
        std::scoped_lock<std::mutex> lock(m_bankMtx);

        return m_bank.emplace(name, ref<Stopwatch>::gen()).first->second;
    }

    void delSw(const char* name) noexcept
    {
        std::scoped_lock<std::mutex> lock(m_bankMtx);
        auto                         p_sw = m_bank.find(name);

        if (p_sw != m_bank.end())
        {
            m_bank.erase(p_sw);
        }
    }

    void clear() noexcept
    {
        std::scoped_lock<std::mutex> lock(m_bankMtx);
        m_bank.clear();
    }

    const std::unordered_map<const char*, ref<Stopwatch>>& getBank() noexcept
    {
        return m_bank;
    }

   private:
    std::unordered_map<const char*, ref<Stopwatch>> m_bank;
    inline static std::mutex                        m_bankMtx;
};

}  // namespace nimbus