using System;
using System.Text;
using System.Runtime.InteropServices;

using IC = Nimbus.InternalCalls;


namespace Nimbus;

public abstract class Component
{
    public Entity m_entity { get; internal set; } = null!;

}

public class GuidCmp : Component
{

}

public class NameCmp : Component
{

}

public class AncestryCmp : Component
{

}

public class ScriptCmp : Component
{

}

public class NativeLogicCmp : Component
{

}

public class TransformCmp : Component
{

    public Mat4 LocalTransform
    {
        get
        {
            return IC.Transform.GetLocalTransform(m_entity.m_nativeEntityId);
        }

        set
        {
            IC.Transform.SetLocalTransform(m_entity.m_nativeEntityId, ref value);
        }
    }

    public Mat4 WorldTransform
    {
        get
        {
            return IC.Transform.GetWorldTransform(m_entity.m_nativeEntityId);
        }
    }
}

public class SpriteCmp : Component
{

}

public class TextCmp : Component
{

}

public class ParticleEmitterCmp : Component
{

}

public class RigidBody2DCmp : Component
{

}

public class CameraCmp : Component
{
}