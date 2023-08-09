#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include <unordered_set>

namespace nimbus
{

static std::unordered_set<void*> s_LiveReferences;
static std::mutex                s_LiveReferenceMutex;

namespace refUtils
{

void addToLiveReferences(void* instance)
{
    std::scoped_lock<std::mutex> lock(s_LiveReferenceMutex);
    NM_CORE_ASSERT_STATIC(instance, "Null Instance");
    s_LiveReferences.insert(instance);
}

void removeFromLiveReferences(void* instance)
{
    std::scoped_lock<std::mutex> lock(s_LiveReferenceMutex);
    NM_CORE_ASSERT_STATIC(instance, "Null Instance");
    NM_CORE_ASSERT_STATIC(
        s_LiveReferences.find(instance) != s_LiveReferences.end(),
        "Absent Instance");
    s_LiveReferences.erase(instance);
}

bool isLive(void* instance)
{
    NM_CORE_ASSERT_STATIC(instance, "Null Instance");
    return s_LiveReferences.find(instance) != s_LiveReferences.end();
}
}  // namespace refUtils

}  // namespace Hazel