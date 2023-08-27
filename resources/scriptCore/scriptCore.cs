using System;
using System.Text;
using System.Runtime.Loader;
using System.Runtime.InteropServices;
using System.Reflection;

using IC = Nimbus.InternalCalls;


using System.Runtime.CompilerServices;
[assembly: DisableRuntimeMarshalling]

namespace Nimbus;
public unsafe class ScriptCore
{
    internal static WeakReference? scriptAlcWeakRef;
    internal static AssemblyLoadContext? scriptAlc;
    internal static Assembly? scriptAssembly;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Purely Internal Functions (Managed)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    internal static bool IntUnloadScriptAssembly()
    {
        if (scriptAlc is null)
        {
            IC.Log.CoreError("Can't unload script assembly when it isn't loaded!");
            return false;
        }
        else
        {
            IC.Log.CoreInfo("Unloading script Assembly");

            scriptAlc.Unload();
            scriptAssembly = null;
            scriptAlc = null;

            GC.Collect();
            GC.WaitForPendingFinalizers();

            // block and poll while assembly is unloading
            while (scriptAlcWeakRef!.IsAlive)
            {
                IC.Log.CoreInfo("Unloading...");
                GC.Collect();
                GC.WaitForPendingFinalizers();
                System.Threading.Thread.Sleep(1);
            }

        }
        IC.Log.CoreInfo("Unloaded script Assembly");
        return true;
    }


    // TODO
    // private static Assembly OnScriptinglcResolving(AssemblyLoadContext alc, AssemblyName assemblyName)
    // {

    //     string relativePath = "../../../felix/resources/script/bin/script.dll"; // The relative path to the assembly
    //     string assemblyPath = Path.Combine(directory, relativePath);
    //     if (File.Exists(assemblyPath))
    //     {
    //         return alc.LoadFromAssemblyPath(assemblyPath);
    //     }
    //     else
    //     {
    //         return null;

    //     }
    // }

    internal static bool IntLoadScriptAssembly(string assemblyPath)
    {
        // todo custom resolver for caching
        // string executingAssemblyLocation = Assembly.GetExecutingAssembly().Location;
        // string directory = Path.GetDirectoryName(executingAssemblyLocation)!;
        // string assemblyPath = Path.Combine(directory, relativePath);

        scriptAlc = new AssemblyLoadContext(assemblyPath, true);
        scriptAlcWeakRef = new WeakReference(scriptAlc, trackResurrection: true);

        scriptAssembly = scriptAlc.LoadFromAssemblyPath(assemblyPath);

        if (scriptAssembly is null)
        {
            IC.Log.CoreError($"Failed to load assembly {assemblyPath}");
            return false;

        }

        // TODO
        // scriptAlc.Resolving += OnScriptinglcResolving;

        IC.Log.CoreInfo($"Loaded script Assembly {assemblyPath}");
        return true;
    }

    internal static List<string> IntReflectOnScriptAssembly(Type? baseType)
    {
        List<string> typeNames = new List<string>();

        if (scriptAssembly is null)
        {
            IC.Log.CoreError("Can't Reflect on unloaded script assembly!");
            return typeNames;
        }

        // Iterate through all types in the assembly
        foreach (Type type in scriptAssembly.GetTypes())
        {
            if (!type.IsClass || (baseType != null && !type.IsSubclassOf(baseType)))
                continue;

            typeNames.Add(type.FullName!);
        }

        return typeNames;
    }

    internal static object? GetObjectFromHandle(IntPtr handle)
    {
        if (handle == IntPtr.Zero)
        {
            IC.Log.CoreError("Null instance handle!");
            return null;
        }

        GCHandle gcHandle = GCHandle.FromIntPtr(handle);
        object? instance = gcHandle.Target;

        if (instance == null || !(instance is Entity))
        {
            IC.Log.CoreError("Invalid instance handle!");
            return null;
        }

        return instance;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Unmanaged only functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [UnmanagedCallersOnly]
    internal static void MemFree(void* p)
    {
        Util.FreeNative(p);
    }

    [UnmanagedCallersOnly]
    internal static void ReleaseHandle(IntPtr handle)
    {
        if (handle == IntPtr.Zero)
        {
            IC.Log.CoreError("Null handle!");
            return;
        }

        // Convert the IntPtr to a GCHandle
        GCHandle gcHandle = GCHandle.FromIntPtr(handle);

        // Release the handle
        gcHandle.Free();
    }

    [UnmanagedCallersOnly]
    internal static IntPtr GetRuntimeInformation()
    {
        return Util.AllocNativeString(System.Runtime.InteropServices.RuntimeInformation.FrameworkDescription);
    }

    [UnmanagedCallersOnly]
    internal static void InitializeScriptCore()
    {
        // TODO nothing yet?
        IC.Log.CoreInfo("ScriptCore Initialized");
    }

    [UnmanagedCallersOnly]
    [return: MarshalAs(UnmanagedType.I1)]
    internal static bool LoadScriptAssembly(IntPtr p_pathToLib)
    {
        string pathToLib = Marshal.PtrToStringAnsi(p_pathToLib)!; // Assuming ANSI encoding
        return IntLoadScriptAssembly(pathToLib);
    }

    [UnmanagedCallersOnly]
    [return: MarshalAs(UnmanagedType.I1)]
    internal static bool UnloadScriptAssembly()
    {
        return IntUnloadScriptAssembly();
    }

    [UnmanagedCallersOnly]
    internal static IntPtr GetScriptAssemblyTypes(IntPtr p_baseClassName, IntPtr p_count)
    {

        Type? baseType = null;
        if (p_baseClassName != IntPtr.Zero)
        {
            string baseClassName = Marshal.PtrToStringAnsi(p_baseClassName)!; // Assuming ANSI encoding
            baseType = Type.GetType(baseClassName);

            if (baseType is null)
            {
                IC.Log.CoreError($"Base class type {baseClassName} not found!");
                return IntPtr.Zero;
            }
        }

        List<string> typeNames = IntReflectOnScriptAssembly(baseType);

        Marshal.WriteInt32(p_count, typeNames.Count);

        IntPtr p_typeNamesAnsi = Marshal.AllocHGlobal(typeNames.Count * IntPtr.Size);

        for (int i = 0; i < typeNames.Count; i++)
        {
            IntPtr p_typeNameAnsi = Util.AllocNativeString(typeNames[i]);
            Marshal.WriteIntPtr(p_typeNamesAnsi, i * IntPtr.Size, p_typeNameAnsi);
        }

        return p_typeNamesAnsi;

    }

    [UnmanagedCallersOnly]
    internal static IntPtr CreateInstanceOfScriptAssemblyEntity(IntPtr p_typeName, uint nativeEntityId)
    {
        if (scriptAssembly is null)
        {
            IC.Log.CoreError("Can't get type from unloaded script assembly!");
            return IntPtr.Zero;
        }

        if (p_typeName != IntPtr.Zero)
        {
            string typeName = Marshal.PtrToStringAnsi(p_typeName)!; // Assuming ANSI encoding

            Type? type = scriptAssembly.GetType(typeName);
            if (type == null)
            {
                IC.Log.CoreError($"Type {typeName} not found!");
                return IntPtr.Zero;
            }

            object? instance = Activator.CreateInstance(type, new object[] { nativeEntityId });
            if (instance == null)
            {
                IC.Log.CoreError($"Couldn't create instance of {typeName}!");
                return IntPtr.Zero;
            }

            // Return the handle to the newly created object.
            return GCHandle.ToIntPtr(GCHandle.Alloc(instance));
        }
        else
        {
            IC.Log.CoreError($"Null type Name!");
            return IntPtr.Zero;

        }
    }


    [UnmanagedCallersOnly]
    internal static IntPtr GetEntityOnCreateFPtr(IntPtr instanceHandle)
    {
        object? instance = GetObjectFromHandle(instanceHandle);

        if (instance == null)
        {
            return IntPtr.Zero;
        }

        Entity entity = (Entity)instance;
        return entity.OnCreateFunctionPointer;
    }

    [UnmanagedCallersOnly]
    internal static IntPtr GetEntityOnUpdateFPtr(IntPtr instanceHandle)
    {
        object? instance = GetObjectFromHandle(instanceHandle);

        if (instance == null)
        {
            return IntPtr.Zero;
        }

        Entity entity = (Entity)instance;
        return entity.OnUpdateFunctionPointer;
    }

    [UnmanagedCallersOnly]
    internal static IntPtr GetEntityOnPhysicsUpdateFPtr(IntPtr instanceHandle)
    {
        object? instance = GetObjectFromHandle(instanceHandle);

        if (instance == null)
        {
            return IntPtr.Zero;
        }

        Entity entity = (Entity)instance;
        return entity.OnPhysicsUpdateFunctionPointer;
    }


    [UnmanagedCallersOnly]
    internal static IntPtr GetEntityOnDestroyFPtr(IntPtr instanceHandle)
    {
        object? instance = GetObjectFromHandle(instanceHandle);

        if (instance == null)
        {
            return IntPtr.Zero;
        }

        Entity entity = (Entity)instance;
        return entity.OnDestroyFunctionPointer;
    }

}


