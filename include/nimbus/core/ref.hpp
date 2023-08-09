#pragma once

#include "nimbus/core/common.hpp"

#include <atomic>
#include <memory>

namespace nimbus
{

class refCounted
{
   public:
    void incRefCount() const
    {
        ++m_refCount;
    }
    void decRefCount() const
    {
        --m_refCount;
    }

    uint32_t getRefCount() const
    {
        return m_refCount.load();
    }

   private:
    mutable std::atomic<uint32_t> m_refCount = 0;
};

namespace RefUtils
{
void addToLiveReferences(void* instance);
void removeFromLiveReferences(void* instance);
bool isLive(void* instance);
}  // namespace RefUtils

template <typename T>
class ref
{
   public:
    ref() : m_inst(nullptr)
    {
    }

    ref(std::nullptr_t n) : m_inst(nullptr)
    {
        (void)(n);
    }

    ref(T* instance) : m_inst(instance)
    {
        static_assert(std::is_base_of<refCounted, T>::value,
                      "Class is not refCounted!");

        incRef();
    }

    template <typename T2>
    ref(const ref<T2>& other)
    {
        m_inst = (T*)other.m_inst;
        incRef();
    }

    template <typename T2>
    ref(ref<T2>&& other)
    {
        m_inst       = (T*)other.m_inst;
        other.m_inst = nullptr;
    }

    static ref<T> CopyWithoutIncrement(const ref<T>& other)
    {
        ref<T> result  = nullptr;
        result->m_inst = other.m_inst;
        return result;
    }

    ~ref()
    {
        decRef();
    }

    ref(const ref<T>& other) : m_inst(other.m_inst)
    {
        incRef();
    }

    ref& operator=(std::nullptr_t)
    {
        decRef();
        m_inst = nullptr;
        return *this;
    }

    ref& operator=(const ref<T>& other)
    {
        other.incRef();
        decRef();

        m_inst = other.m_inst;
        return *this;
    }

    template <typename T2>
    ref& operator=(const ref<T2>& other)
    {
        other.incRef();
        decRef();

        m_inst = other.m_inst;
        return *this;
    }

    template <typename T2>
    ref& operator=(ref<T2>&& other)
    {
        decRef();

        m_inst       = other.m_inst;
        other.m_inst = nullptr;
        return *this;
    }

    operator bool()
    {
        return m_inst != nullptr;
    }
    operator bool() const
    {
        return m_inst != nullptr;
    }

    T* operator->()
    {
        return m_inst;
    }
    const T* operator->() const
    {
        return m_inst;
    }

    T& operator*()
    {
        return *m_inst;
    }
    const T& operator*() const
    {
        return *m_inst;
    }

    T* raw()
    {
        return m_inst;
    }
    const T* raw() const
    {
        return m_inst;
    }

    void reset(T* instance = nullptr)
    {
        decRef();
        m_inst = instance;
    }

    template <typename T2>
    ref<T2> As() const
    {
        return ref<T2>(*this);
    }

    template <typename... Args>
    static ref<T> gen(Args&&... args)
    {
        return ref<T>(new T(std::forward<Args>(args)...));
    }

    bool operator==(const ref<T>& other) const
    {
        return m_inst == other.m_inst;
    }

    bool operator!=(const ref<T>& other) const
    {
        return !(*this == other);
    }

    bool equalsObject(const ref<T>& other)
    {
        if (!m_inst || !other.m_inst)
            return false;

        return *m_inst == *other.m_inst;
    }

   private:
    void incRef() const
    {
        if (m_inst)
        {
            m_inst->incRefCount();
            RefUtils::addToLiveReferences((void*)m_inst);
        }
    }

    void decRef() const
    {
        if (m_inst)
        {
            m_inst->decRefCount();
            if (m_inst->getRefCount() == 0)
            {
                delete m_inst;
                RefUtils::removeFromLiveReferences((void*)m_inst);
                m_inst = nullptr;
            }
        }
    }

    template <class T2>
    friend class ref;
    mutable T* m_inst;
};

template <typename T>
class weakRef
{
   public:
    weakRef() = default;

    weakRef(ref<T> ref)
    {
        m_inst = ref.raw();
    }

    weakRef(T* instance)
    {
        m_inst = instance;
    }

    T* operator->()
    {
        return m_inst;
    }
    const T* operator->() const
    {
        return m_inst;
    }

    T& operator*()
    {
        return *m_inst;
    }
    const T& operator*() const
    {
        return *m_inst;
    }

    bool isValid() const
    {
        return m_inst ? RefUtils::isLive(m_inst) : false;
    }
    operator bool() const
    {
        return isValid();
    }

   private:
    T* m_inst = nullptr;
};

}  // namespace nimbus
