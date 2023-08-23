using System;
using System.Runtime.InteropServices;


namespace Nimbus
{
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Vec4
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [StructLayout(LayoutKind.Sequential)]
    public struct Vec4
    {
        public float x;
        public float y;
        public float z;
        public float w;

        public Vec4(float x, float y, float z, float w)
        {
            this.x = x;
            this.y = y;
            this.z = z;
            this.w = w;
        }

        public Vec4(float all) => x = y = z = w = all;

    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Vec3
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [StructLayout(LayoutKind.Sequential)]
    public struct Vec3
    {
        public float x;
        public float y;
        public float z;

        public Vec3(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public Vec3(float all) => x = y = z = all;

    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Vec2
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [StructLayout(LayoutKind.Sequential)]
    public struct Vec2
    {
        public float x;
        public float y;

        public Vec2(float x, float y)
        {
            this.x = x;
            this.y = y;
        }

        public Vec2(float all) => x = y = all;

    }
}