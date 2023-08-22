using System;
using System.Runtime.InteropServices;

namespace nimbus
{
    public unsafe partial class InternalCalls
    {
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Logging
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [LibraryImport("nimbusInternalCalls", EntryPoint = "ic_coreInfo", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void CoreInfo(string str);

        [LibraryImport("nimbusInternalCalls", EntryPoint = "ic_coreWarn", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void CoreWarn(string str);
        
        [LibraryImport("nimbusInternalCalls", EntryPoint = "ic_coreError", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void CoreError(string str);
        
        [LibraryImport("nimbusInternalCalls", EntryPoint = "ic_coreCritical", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void CoreCritical(string str);
        
        [LibraryImport("nimbusInternalCalls", EntryPoint = "ic_coreTrace", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void CoreTrace(string str);
        
        [LibraryImport("nimbusInternalCalls", EntryPoint = "ic_info", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void Info(string str);
        
        [LibraryImport("nimbusInternalCalls", EntryPoint = "ic_warn", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void Warn(string str);
        
        [LibraryImport("nimbusInternalCalls", EntryPoint = "ic_error", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void Error(string str);
        
        [LibraryImport("nimbusInternalCalls", EntryPoint = "ic_critical", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void Critical(string str);
        
        [LibraryImport("nimbusInternalCalls", EntryPoint = "ic_trace", StringMarshalling = StringMarshalling.Utf8)]
        public static partial void Trace(string str);

    }
}