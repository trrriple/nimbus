#pragma once

#include "nimbus/core/common.hpp"

#include <atomic>
#include <memory>

namespace nimbus
{

class refCounted
{
   public:
    inline void incRefCount() const
    {
        ++m_refCount;
    }
    inline uint32_t decRefCount() const
    {
        return --m_refCount;
    }

   private:
    mutable std::atomic<uint32_t> m_refCount = 0;
};

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
        static_assert(std::is_base_of<refCounted, T>::value, "Class is not refCounted!");

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

    static ref<T> replicate(const ref<T>& other)
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

    inline ref& operator=(std::nullptr_t)
    {
        decRef();
        m_inst = nullptr;
        return *this;
    }

    inline ref& operator=(const ref<T>& other)
    {
        other.incRef();
        decRef();

        m_inst = other.m_inst;
        return *this;
    }

    template <typename T2>
    inline ref& operator=(const ref<T2>& other)
    {
        other.incRef();
        decRef();

        m_inst = other.m_inst;
        return *this;
    }

    template <typename T2>
    inline ref& operator=(ref<T2>&& other)
    {
        decRef();

        m_inst       = other.m_inst;
        other.m_inst = nullptr;
        return *this;
    }

    inline operator bool()
    {
        return m_inst != nullptr;
    }

    inline operator bool() const
    {
        return m_inst != nullptr;
    }

    inline T* operator->()
    {
        return m_inst;
    }

    inline const T* operator->() const
    {
        return m_inst;
    }

    inline T& operator*()
    {
        return *m_inst;
    }

    inline const T& operator*() const
    {
        return *m_inst;
    }

    inline T* raw()
    {
        return m_inst;
    }

    inline const T* raw() const
    {
        return m_inst;
    }

    inline void reset(T* instance = nullptr)
    {
        decRef();
        m_inst = instance;
    }

    template <typename T2>
    inline ref<T2> As() const
    {
        return ref<T2>(*this);
    }

    template <typename... Args>
    inline static ref<T> gen(Args&&... args)
    {
        return ref<T>(new T(std::forward<Args>(args)...));
    }

    inline bool operator==(const ref<T>& other) const
    {
        return m_inst == other.m_inst;
    }

    inline bool operator!=(const ref<T>& other) const
    {
        return !(*this == other);
    }

    inline bool equalsObject(const ref<T>& other)
    {
        if (!m_inst || !other.m_inst)
            return false;

        return *m_inst == *other.m_inst;
    }

   private:
    inline void incRef() const
    {
        if (m_inst)
        {
            m_inst->incRefCount();
        }
    }

    inline void decRef() const
    {
        if (m_inst)
        {
            if (m_inst->decRefCount() == 0)
            {
                delete m_inst;
                m_inst = nullptr;
            }
        }
    }

    template <class T2>
    friend class ref;
    mutable T* m_inst;
};

// template <typename T>
// class weakRef
// {
//    public:
//     weakRef() = default;

//     inline weakRef(ref<T> ref)
//     {
//         m_inst = ref.raw();
//     }

//     inline weakRef(T* instance)
//     {
//         m_inst = instance;
//     }

//     inline T* operator->()
//     {
//         return m_inst;
//     }
//     inline const T* operator->() const
//     {
//         return m_inst;
//     }

//     inline T& operator*()
//     {
//         return *m_inst;
//     }

//     inline const T& operator*() const
//     {
//         return *m_inst;
//     }

//     inline bool isValid() const
//     {
//         return m_inst ? refUtils::isLive(m_inst) : false;
//     }

//     inline operator bool() const
//     {
//         return isValid();
//     }

//    private:
//     T* m_inst = nullptr;
// };

}  // namespace nimbus
