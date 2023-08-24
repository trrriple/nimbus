using System;
using System.Text;
using System.Runtime.Loader;
using System.Runtime.InteropServices;
using System.Reflection;


namespace Nimbus
{
    public unsafe class ScriptCore
    {
        internal static WeakReference? scriptAlcWeakRef;
        internal static AssemblyLoadContext? scriptAlc;
        internal static Assembly? scriptAssembly;
        internal static int callCount = 0;


        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Purely Internal Functions (Managed)
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        internal static void IntUnloadScriptAssembly()
        {
            if (scriptAlc is null)
            {
                InternalCalls.CoreError("Can't unload script assembly when it isn't loaded!");
                return;
            }
            else
            {
                InternalCalls.CoreInfo("Unloading script Assembly");

                scriptAlc.Unload();
                scriptAssembly = null;
                scriptAlc = null;

                GC.Collect();
                GC.WaitForPendingFinalizers();

                // block and poll while assembly is unloading
                while (scriptAlcWeakRef!.IsAlive)
                {
                    InternalCalls.CoreInfo("Unloading...");
                    GC.Collect();
                    GC.WaitForPendingFinalizers();
                    System.Threading.Thread.Sleep(1);
                }

            }
            InternalCalls.CoreInfo("Unloaded script Assembly");
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

        internal static void IntLoadScriptAssembly()
        {
            // todo custom resolver for caching
            string relativePath = "../../scripts/bin/scripts.dll"; // The relative path to the assembly
            string executingAssemblyLocation = Assembly.GetExecutingAssembly().Location;
            string directory = Path.GetDirectoryName(executingAssemblyLocation)!;
            string assemblyPath = Path.Combine(directory, relativePath);

            scriptAlc = new AssemblyLoadContext(assemblyPath, true);
            scriptAlcWeakRef = new WeakReference(scriptAlc, trackResurrection: true);

            scriptAssembly = scriptAlc.LoadFromAssemblyPath(assemblyPath);

            if (scriptAssembly is null)
            {
                InternalCalls.CoreError("Failed to get assembly!");
                return;

            }

            // TODO
            // scriptAlc.Resolving += OnScriptinglcResolving;

            InternalCalls.CoreInfo($"Loaded script Assembly {callCount}");
            callCount++;
        }

        internal static List<string> IntReflectOnScriptAssembly(Type? baseType)
        {
            List<string> typeNames = new List<string>();

            if (scriptAssembly is null)
            {
                InternalCalls.CoreError("Can't Reflect on unloaded script assembly!");
                return typeNames;
            }

            // Iterate through all types in the assembly
            foreach (Type type in scriptAssembly.GetTypes())
            {
                if (!type.IsClass || (baseType != null && !type.IsSubclassOf(baseType)))
                    continue;

                InternalCalls.CoreInfo($"Type: {type.FullName}");

                typeNames.Add(type.FullName!);


                // // Iterate through all methods in the type
                // foreach (MethodInfo method in type.GetMethods(BindingFlags.Public | BindingFlags.NonPublic
                //             | BindingFlags.Instance | BindingFlags.Static))
                // {
                //     InternalCalls.CoreInfo($"  Method: {method.Name}");
                // }
            }

            return typeNames;
        }

        internal static object? GetObjectFromHandle(IntPtr handle)
        {
            if (handle == IntPtr.Zero)
            {
                InternalCalls.CoreError("Null instance handle!");
                return null;
            }

            GCHandle gcHandle = GCHandle.FromIntPtr(handle);
            object? instance = gcHandle.Target;

            if (instance == null || !(instance is Entity))
            {
                InternalCalls.CoreError("Invalid instance handle!");
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
                InternalCalls.CoreError("Null handle!");
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
        internal static void InitializeScriptCore(IntPtr p_pathToLib)
        {
            // TODO remove hardcode
            string pathToLib = Marshal.PtrToStringAnsi(p_pathToLib)!; // Assuming ANSI encoding
            IntLoadScriptAssembly();
        }

        [UnmanagedCallersOnly]
        internal static void LoadScriptAssembly()
        {
            IntLoadScriptAssembly();
        }

        [UnmanagedCallersOnly]
        internal static void UnloadScriptAssembly()
        {
            IntUnloadScriptAssembly();
        }

        [UnmanagedCallersOnly]
        internal static void ReloadScriptAssembly()
        {
            IntUnloadScriptAssembly();
            IntLoadScriptAssembly();
        }

        [UnmanagedCallersOnly]
        internal static void CallScriptMethod(IntPtr baseClassNamePtr)
        {
            Type? type = scriptAssembly!.GetType("Script.Felix");


            if (type is null)
            {
                InternalCalls.CoreError("Failed to get type!");
                return;
            }


            // foreach (MethodInfo m in type.GetMethods())
            // {
            //     Console.WriteLine(m.Name);
            // }

            MethodInfo? method = type.GetMethod("Hello");

            if (method is null)
            {
                InternalCalls.CoreError("Failed to get method!");
                return;
            }

            object[] parameters = { /* your parameters here */ };
            method.Invoke(null, parameters);


            Vec4 vector = new Vec4(1.0f, 2.0f, 3.0f, 4.0f);

            InternalCalls.vec4Test(ref vector);

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
                    InternalCalls.CoreError($"Base class type {baseClassName} not found!");
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
                InternalCalls.CoreError("Can't get type from unloaded script assembly!!");
                return IntPtr.Zero;
            }

            if (p_typeName != IntPtr.Zero)
            {
                string typeName = Marshal.PtrToStringAnsi(p_typeName)!; // Assuming ANSI encoding

                Type? type = scriptAssembly.GetType(typeName);
                if (type == null)
                {
                    InternalCalls.CoreError($"Type {typeName} not found!");
                    return IntPtr.Zero;
                }

                object? instance = Activator.CreateInstance(type, new object[] { nativeEntityId });
                if (instance == null)
                {
                    InternalCalls.CoreError($"Couldn't create instance of {typeName}!");
                    return IntPtr.Zero;
                }

                // Return the handle to the newly created object.
                return GCHandle.ToIntPtr(GCHandle.Alloc(instance));
            }
            else
            {
                InternalCalls.CoreError($"Null type Name!");
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

}
