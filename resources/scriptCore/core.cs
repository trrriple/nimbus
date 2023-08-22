using System;
using System.Text;
using System.Runtime.Loader;
using System.Runtime.InteropServices;
using System.Reflection;


namespace nimbus
{
    public unsafe class ScriptCore
    {
        internal class ScriptAssemblyContext : AssemblyLoadContext
        {

            private AssemblyDependencyResolver _resolver;

            public ScriptAssemblyContext(string mainAssemblyToLoadPath) : base(isCollectible: true)
            {

                _resolver = new AssemblyDependencyResolver(mainAssemblyToLoadPath);

            }

            protected override Assembly? Load(AssemblyName name)
            {
                string? assemblyPath = _resolver.ResolveAssemblyToPath(name);
                if (assemblyPath != null)
                {
                    return LoadFromAssemblyPath(assemblyPath);
                }

                return null;
            }

        }

        internal static WeakReference?          scriptAlcWeakRef;
        internal static ScriptAssemblyContext?  scriptAlc;
        internal static Assembly?               scriptAssembly;
        internal static int                     callCount = 0;

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

            scriptAlc = new ScriptAssemblyContext(assemblyPath);
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


        [UnmanagedCallersOnly]
        internal static void memFree(void* p)
        {
            Util.FreeNative(p);
        }

        [UnmanagedCallersOnly]
        internal static IntPtr GetRuntimeInformation()
        {
            InternalCalls.CoreInfo("Testing!");
            return Util.AllocNativeString(System.Runtime.InteropServices.RuntimeInformation.FrameworkDescription);
        }

        [UnmanagedCallersOnly]
        internal static void InitializeScriptCore(IntPtr p_pathToLibs)
        {
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
        internal static void TestCallScript()
        {
            Type? type = scriptAssembly!.GetType("script.Felix");


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
        }

    }
}