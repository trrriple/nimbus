using System;
using System.Text;
using System.Runtime.InteropServices;

using IC = Nimbus.InternalCalls;

namespace Nimbus;
public abstract class Entity
{
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    protected delegate void OnCreateDelegate();
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    protected delegate void OnUpdateDelegate(float deltaTime);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    protected delegate void OnPhysicsUpdateDelegate(float deltaTime);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    protected delegate void OnDestroyDelegate();

    private OnCreateDelegate? onCreateDelegate;
    private OnUpdateDelegate? onUpdateDelegate;
    private OnPhysicsUpdateDelegate? onPhysicsUpdateDelegate;
    private OnDestroyDelegate? onDestroyDelegate;

    public readonly uint m_nativeEntityId;

    // keeps references to known components
    private Dictionary<Type, Component> m_cmpCache = new Dictionary<Type, Component>();

    public Entity(uint nativeEntityId)
    {
        m_nativeEntityId = nativeEntityId;
    }

    public IntPtr OnCreateFunctionPointer
    {
        get
        {
            if (onCreateDelegate == null)
            {
                onCreateDelegate = OnCreate;
            }
            return Marshal.GetFunctionPointerForDelegate(onCreateDelegate);
        }
    }

    public IntPtr OnUpdateFunctionPointer
    {
        get
        {
            if (onUpdateDelegate == null)
            {
                onUpdateDelegate = OnUpdate;
            }
            return Marshal.GetFunctionPointerForDelegate(onUpdateDelegate);
        }
    }

    public IntPtr OnPhysicsUpdateFunctionPointer
    {
        get
        {
            if (onPhysicsUpdateDelegate == null)
            {
                onPhysicsUpdateDelegate = OnPhysicsUpdate;
            }
            return Marshal.GetFunctionPointerForDelegate(onPhysicsUpdateDelegate);
        }
    }

    public IntPtr OnDestroyFunctionPointer
    {
        get
        {
            if (onDestroyDelegate == null)
            {
                onDestroyDelegate = OnDestroy;
            }
            return Marshal.GetFunctionPointerForDelegate(onDestroyDelegate);
        }
    }


    protected virtual void OnCreate() { }
    protected virtual void OnUpdate(float deltaTime) { }
    protected virtual void OnPhysicsUpdate(float deltaTime) { }
    protected virtual void OnDestroy() { }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Component functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public bool HasComponent<T>() where T : Component =>
        IC.Component.HasComponent(m_nativeEntityId, typeof(T).TypeHandle.Value);

    public T? GetComponent<T>() where T : Component, new()
    {
        Type cmpType = typeof(T);

        if (!HasComponent<T>())
        {
            // this is a unique case where the component added to the cache but is no longer present on the 
            // native entity, so remove it from the cache
            if (m_cmpCache.ContainsKey(cmpType))
            {
                m_cmpCache.Remove(cmpType);
            }
            return null;
        }

        // the native entity has the component but we don't have a c# version yet. make one and save it
        if (!m_cmpCache.ContainsKey(cmpType))
        {
            T cmp = new T { m_entity = this };
            m_cmpCache.Add(cmpType, cmp);
            return cmp;
        }

        // we have it so provide it
        return m_cmpCache[cmpType] as T;
    }

    public T? AddComponent<T>() where T : Component, new()
    {
        
        // if we have it already, just bypass and grab the existing data
        if (HasComponent<T>())
        {
            return GetComponent<T>();
        }

        Type cmpType = typeof(T);
        
        // add it to the native component
        if(!IC.Component.AddComponent(m_nativeEntityId, cmpType.TypeHandle.Value))
        {
            // failed to add component
            return null;
        }
        
        T cmp = new T {m_entity = this};

        // add it to the cache to get it quickly later
        m_cmpCache.Add(cmpType, cmp);
    
        return cmp;
    }

    public bool RemoveComponent<T>() where T : Component
    {
        Type cmpType = typeof(T);

        bool wasRemoved = IC.Component.RemoveComponent(m_nativeEntityId, cmpType.TypeHandle.Value);

        // if was was successfully removed, and we had a cached c# version, remove it from cache
        if(wasRemoved && m_cmpCache.ContainsKey(cmpType))
        {
            m_cmpCache.Remove(cmpType);
        }

        return wasRemoved;
    }
}
