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

    public readonly uint nativeEntityId;

    public Entity(uint nativeEntityId)
    {
        this.nativeEntityId = nativeEntityId;
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
        IC.Component.HasComponent(nativeEntityId, typeof(T).TypeHandle.Value);
}
