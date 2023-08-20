#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/core/guid.hpp"

#if defined(_WIN32) || defined(_WIN64)
#include <objbase.h>
#elif defined(__linux__)
#include <uuid/uuid.h>
#else
#error Platform not supported
#endif

namespace nimbus
{

Guid::Guid()
{
#if defined(_WIN32) || defined(_WIN64)
    GUID guid;
    CoCreateGuid(&guid);

    i128_t high = static_cast<i128_t>(guid.Data1) << 96 | static_cast<i128_t>(guid.Data2) << 80
                  | static_cast<i128_t>(guid.Data3) << 64;

    i128_t low = *reinterpret_cast<i128_t*>(guid.Data4);

    m_guid = high | low;

    _toString();

#elif defined(__linux__)
    uuid_t uuid;
    uuid_generate_random(uuid);

    m_guid = *reinterpret_cast<i128_t*>(uuid);

    _toString();

#endif
}

Guid::Guid(i128_t guid)
{
    m_guid = guid;
    _toString();
}

Guid::Guid(const std::string& guidStr)
{
    m_guidStr = guidStr;
    _fromString();
}

void Guid::_toString()
{
    char buffer[37];  // 32 characters, 4 hyphens, and a null terminator

    u64_t high = static_cast<u64_t>(m_guid >> 64);
    u64_t low  = static_cast<u64_t>(m_guid);

    snprintf(buffer,
             sizeof(buffer),
             "%08llx-%04llx-%04llx-%04llx-%012llx",
             high >> 32,
             (high >> 16) & 0xFFFF,
             high & 0xFFFF,
             low >> 48,
             low & 0xFFFFFFFFFFFF);

    m_guidStr = std::string(buffer);
}

void Guid::_fromString()
{
    u64_t high, low;
    u32_t a;
    u16_t b, c, d;
    u64_t e;

    sscanf_s(m_guidStr.c_str(), "%08lx-%04hx-%04hx-%04hx-%012lx", &a, &b, &c, &d, &e);

    high = (static_cast<u64_t>(a) << 32) | (static_cast<u64_t>(b) << 16) | static_cast<u64_t>(c);

    low = (static_cast<u64_t>(d) << 48) | e;

    m_guid = (static_cast<i128_t>(high) << 64) | low;
}

}  // namespace nimbus