using System;
using System.Runtime.InteropServices;
using System.Numerics;

using System.Runtime.CompilerServices;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Attempt to provide GLM/GL/Vulkan-esk Vector & Matrix behavior but using SIMD enabled System.Numerics types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Nimbus
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Vec4
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public struct Vec4
    {
        private Vector4 native;

        public float X
        {
            get { return native.X; }
            set { native.X = value; }
        }

        public float Y
        {
            get { return native.Y; }
            set { native.Y = value; }
        }

        public float Z
        {
            get { return native.Z; }
            set { native.Z = value; }
        }

        public float W
        {
            get { return native.W; }
            set { native.W = value; }
        }

        //////////////////////////////////////////////////////
        // Constructors
        //////////////////////////////////////////////////////
        public Vec4(float x, float y, float z, float w)
        {
            native.X = x;
            native.Y = y;
            native.Z = z;
            native.W = w;
        }

        public Vec4(Vector4 native)
        {
            this.native = native;
        }

        public Vec4(float all) => native.X = native.Y = native.Z = native.W = all;


        //////////////////////////////////////////////////////
        // Operators
        //////////////////////////////////////////////////////
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vec4 operator *(Mat4 lhs, Vec4 rhs) // note the order here is to follow GL convention
        {
            return new Vec4(Vector4.Transform(rhs.native, lhs.native));
        }

    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Vec3
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public struct Vec3
    {
        public Vector3 native;

        public float X
        {
            get { return native.X; }
            set { native.X = value; }
        }

        public float Y
        {
            get { return native.Y; }
            set { native.Y = value; }
        }

        public float Z
        {
            get { return native.Z; }
            set { native.Z = value; }
        }

        //////////////////////////////////////////////////////
        // Constructors
        //////////////////////////////////////////////////////
        public Vec3(float x, float y, float z)
        {
            native = new Vector3(x, y, z);
        }

        public Vec3(Vector3 native)
        {
            this.native = native;
        }

        public Vec3(float all)
        {
            native = new Vector3(all, all, all);
        }

        // Operator overloads here if needed
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Vec2
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public struct Vec2
    {
        public Vector2 native;

        public float X
        {
            get { return native.X; }
            set { native.X = value; }
        }

        public float Y
        {
            get { return native.Y; }
            set { native.Y = value; }
        }

        //////////////////////////////////////////////////////
        // Constructors
        //////////////////////////////////////////////////////
        public Vec2(float x, float y)
        {
            native = new Vector2(x, y);
        }

        public Vec2(Vector2 native)
        {
            this.native = native;
        }

        public Vec2(float all)
        {
            native = new Vector2(all, all);
        }

        // Operator overloads here if needed
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Mat4
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    public struct Mat4
    {
        public Matrix4x4 native;

        public float this[int row, int column]
        {
            get { return native[row, column]; }
            set { native[row, column] = value; }
        }

        //////////////////////////////////////////////////////
        // Constructors
        //////////////////////////////////////////////////////

        public Mat4(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31,
                    float m32, float m33, float m34, float m41, float m42, float m43, float m44)
        {
            native.M11 = m11;
            native.M12 = m12;
            native.M13 = m13;
            native.M14 = m14;
            native.M21 = m21;
            native.M22 = m22;
            native.M23 = m23;
            native.M24 = m24;
            native.M31 = m31;
            native.M32 = m32;
            native.M33 = m33;
            native.M34 = m34;
            native.M41 = m41;
            native.M42 = m42;
            native.M43 = m43;
            native.M44 = m44;
        }

        public Mat4(Matrix4x4 native)
        {
            this.native = native;
        }

        public Mat4(float diagonal)
        {
            native = Matrix4x4.Identity;
            native.M11 = diagonal;
            native.M22 = diagonal;
            native.M33 = diagonal;
            native.M44 = diagonal;
        }

        //////////////////////////////////////////////////////
        // Operators
        //////////////////////////////////////////////////////
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Mat4 operator *(Mat4 lhs, Mat4 rhs)
        {
            return new Mat4(Matrix4x4.Multiply(lhs.native, rhs.native));
        }
    }
}