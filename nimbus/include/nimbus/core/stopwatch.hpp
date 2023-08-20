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
    inline Stopwatch()
    {
        reset();
        m_splitElapsed_s = 0.0;
        m_lastSplit_s    = 0.0;
    }

    inline void reset()
    {
        m_ts = std::chrono::steady_clock::now();
    }

    inline f64_t elapsed() const
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_ts).count()
               * 1E-6;
    }

    inline u32_t elapsedMs() const
    {
        return elapsed() * 1000.0f;
    }

    inline u32_t elapsedUs() const
    {
        return elapsed() * 1000000.0f;
    }

    inline f32_t split()
    {
        f64_t tNow_s     = elapsed();
        m_lastSplit_s    = tNow_s - m_splitElapsed_s;
        m_splitElapsed_s = tNow_s;
        return m_lastSplit_s;
    }

    inline void splitAndSave()
    {
        split();
        saveSplit();
    }

    inline void saveSplit()
    {
        std::scoped_lock<std::mutex> lock(m_splitMtx);
        m_savedSplit = m_lastSplit_s;
    }

    inline void saveSplitOverride(f64_t split)
    {
        std::scoped_lock<std::mutex> lock(m_splitMtx);
        m_savedSplit = split;
    }

    inline f32_t getLastSplit() const
    {
        return m_lastSplit_s;
    }

    inline f32_t getLastSavedSplit() const
    {
        std::scoped_lock<std::mutex> lock(m_splitMtx);
        return m_savedSplit;
    }

   private:
    std::chrono::time_point<std::chrono::steady_clock> m_ts;
    f64_t                                              m_splitElapsed_s;
    f32_t                                              m_lastSplit_s;
    inline static std::mutex                           m_splitMtx;
    f32_t                                              m_savedSplit;

    friend class WatchBank;
};

class StopWatchBank
{
   public:
    StopWatchBank() = default;
    ~StopWatchBank()
    {
        std::scoped_lock<std::mutex> lock(m_bankMtx);
        m_bank.clear();
    }

    ref<Stopwatch> newSw(const char* name)
    {
        std::scoped_lock<std::mutex> lock(m_bankMtx);

        return m_bank.emplace(name, ref<Stopwatch>::gen()).first->second;
    }

    void delSw(const char* name)
    {
        std::scoped_lock<std::mutex> lock(m_bankMtx);
        auto                         p_sw = m_bank.find(name);

        if (p_sw != m_bank.end())
        {
            m_bank.erase(p_sw);
        }
    }

    void clear()
    {
        std::scoped_lock<std::mutex> lock(m_bankMtx);
        m_bank.clear();
    }

    const std::unordered_map<const char*, ref<Stopwatch>>& getBank()
    {
        return m_bank;
    }

   private:
    std::unordered_map<const char*, ref<Stopwatch>> m_bank;
    inline static std::mutex                        m_bankMtx;
};

}  // namespace nimbus