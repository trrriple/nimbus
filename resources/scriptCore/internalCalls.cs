using System;
using System.Runtime.InteropServices;
using System.Numerics;

namespace Nimbus;

public unsafe partial class InternalCalls
{
    public unsafe partial class Component
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Component
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////        
        [LibraryImport("nimbus", EntryPoint = "ic_hasComponent")]
        [return: MarshalAs(UnmanagedType.I1)]
        public static partial bool HasComponent(uint entityId, IntPtr typeHandle);

        [LibraryImport("nimbus", EntryPoint = "ic_addComponent")]
        [return: MarshalAs(UnmanagedType.I1)]
        public static partial bool AddComponent(uint entityId, IntPtr typeHandle);

        [LibraryImport("nimbus", EntryPoint = "ic_removeComponent")]
        [return: MarshalAs(UnmanagedType.I1)]
        public static partial bool RemoveComponent(uint entityId, IntPtr typeHandle);


    }

    public unsafe partial class Log
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Logging
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [LibraryImport("nimbus", EntryPoint = "ic_coreInfo", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void CoreInfo(string str);

        [LibraryImport("nimbus", EntryPoint = "ic_coreWarn", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void CoreWarn(string str);

        [LibraryImport("nimbus", EntryPoint = "ic_coreError", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void CoreError(string str);

        [LibraryImport("nimbus", EntryPoint = "ic_coreCritical", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void CoreCritical(string str);

        [LibraryImport("nimbus", EntryPoint = "ic_coreTrace", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void CoreTrace(string str);

        [LibraryImport("nimbus", EntryPoint = "ic_info", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void Info(string str);

        [LibraryImport("nimbus", EntryPoint = "ic_warn", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void Warn(string str);

        [LibraryImport("nimbus", EntryPoint = "ic_error", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void Error(string str);

        [LibraryImport("nimbus", EntryPoint = "ic_critical", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void Critical(string str);

        [LibraryImport("nimbus", EntryPoint = "ic_trace", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void Trace(string str);
    }

    public unsafe partial class Transform
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Transform
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////
        // World Transform
        //////////////////////////////////////////////////////
        ///////////////////////////
        // Getters
        ///////////////////////////
        [LibraryImport("nimbus", EntryPoint = "ic_getWorldTransform")]
        public static partial Mat4 GetWorldTransform(uint entityId);

        [LibraryImport("nimbus", EntryPoint = "ic_getWorldTranslation")]
        public static partial Vec3 GetWorldTranslation(uint entityId);

        [LibraryImport("nimbus", EntryPoint = "ic_getWorldRotation")]
        public static partial Vec3 GetWorldRotation(uint entityId);

        [LibraryImport("nimbus", EntryPoint = "ic_getWorldScale")]
        public static partial Vec3 GetWorldScale(uint entityId);


        /////////////////////////////////////////////////////////
        // Local Transform
        //////////////////////////////////////////////////////
        ///////////////////////////
        // Getters
        ///////////////////////////
        [LibraryImport("nimbus", EntryPoint = "ic_getLocalTransform")]
        public static partial Mat4 GetLocalTransform(uint entityId);

        [LibraryImport("nimbus", EntryPoint = "ic_getLocalTranslation")]
        public static partial Vec3 GetLocalTranslation(uint entityId);

        [LibraryImport("nimbus", EntryPoint = "ic_getLocalRotation")]
        public static partial Vec3 GetLocalRotation(uint entityId);

        [LibraryImport("nimbus", EntryPoint = "ic_getLocalScale")]
        public static partial Vec3 GetLocalScale(uint entityId);

        ///////////////////////////
        // Setters
        ///////////////////////////
        [LibraryImport("nimbus", EntryPoint = "ic_setLocalTransform")]
        public static partial void SetLocalTransform(uint entityId, ref Mat4 transform);

        [LibraryImport("nimbus", EntryPoint = "ic_setLocalTranslation")]
        public static partial void SetLocalTranslation(uint entityId, ref Vec3 translation);

        [LibraryImport("nimbus", EntryPoint = "ic_setLocalRotation")]
        public static partial void SetLocalRotation(uint entityId, ref Vec3 rotation);

        [LibraryImport("nimbus", EntryPoint = "ic_setLocalScale")]
        public static partial void SetLocalScale(uint entityId, ref Vec3 scale);
    }

    public unsafe partial class Input
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Input
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [return: MarshalAs(UnmanagedType.I1)]
        [LibraryImport("nimbus", EntryPoint = "ic_keyPresed")]
        public static partial bool KeyDown(ScanCode scanCode);

        [return: MarshalAs(UnmanagedType.I1)]
        [LibraryImport("nimbus", EntryPoint = "ic_modKeyPressed")]
        public static partial bool ModKeyDown(KeyMod keyMod);

        [return: MarshalAs(UnmanagedType.I1)]
        [LibraryImport("nimbus", EntryPoint = "ic_mouseButtonPressed")]
        public static partial bool MouseButtonDown(MouseButton mouseButton);

    }

}
