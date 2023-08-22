using System;
using System.Text;
using System.Runtime.InteropServices;


namespace nimbus
{
    public unsafe class Util
    {
        internal static IntPtr AllocNativeString(string str)
        {
            // shortcut blank strings
            if (str is null)
                return IntPtr.Zero;

            // get length of memory needed, including null terminator
            int length = str.Length + 1;

            // allocate the memory
            void* p_mem = NativeMemory.AlignedAlloc((UIntPtr)length, 16);

            // use a span to share the memory
            Span<byte> byteSpan = new Span<byte>(p_mem, length);

            // encode the string, interprted as ascii into the span
            Encoding.ASCII.GetBytes(str, byteSpan);

            // set the null teminator
            byteSpan[length - 1] = 0;

            return (IntPtr)p_mem;
        }

        internal static void FreeNative(void* p)
        {
            NativeMemory.AlignedFree(p);
        }
    }
}