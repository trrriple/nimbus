#pragma once
#include "nimbus/core/common.hpp"

namespace nimbus
{

class Guid
{
   public:
    Guid() noexcept;

    inline __int128_t get() const noexcept
    {
        return m_guid;
    }

    inline std::string toString() const noexcept
    {
        return m_guidStr;
    }

    inline bool operator==(const Guid& other) const noexcept
    {
        return m_guid == other.m_guid;
    }

    inline bool operator!=(const Guid& other) const noexcept
    {
        return m_guid != other.m_guid;
    }

   private:
    // private constructor takes already generated guid
    Guid(__int128_t guid) noexcept;
    Guid(const std::string& guidStr) noexcept;

    __int128_t  m_guid;
    std::string m_guidStr;

    void _toString() noexcept;
    void _fromString() noexcept;

    friend struct GuidCmp;
};

}  // namespace nimbus