#pragma once
#include "nimbus/core/common.hpp"

namespace nimbus
{

class Guid
{
   public:
    Guid();

    inline __int128_t get() const
    {
        return m_guid;
    }

    inline std::string toString() const
    {
        return m_guidStr;
    }

    inline bool operator==(const Guid& other) const
    {
        return m_guid == other.m_guid;
    }

    inline bool operator!=(const Guid& other) const
    {
        return m_guid != other.m_guid;
    }

   private:
    // private constructor takes already generated guid
    Guid(__int128_t guid);
    Guid(const std::string& guidStr);

    __int128_t  m_guid;
    std::string m_guidStr;

    void _toString();
    void _fromString();

    friend struct GuidCmp;
};

}  // namespace nimbus