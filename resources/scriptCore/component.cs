using System;
using System.Text;
using System.Runtime.InteropServices;

using IC = Nimbus.InternalCalls;


namespace Nimbus;

public abstract class Component
{
    public Entity entity { get; internal set; }

    public Component(Entity entity)
    {
        this.entity = entity;
    }
}

public class GuidCmp : Component
{
    public GuidCmp(Entity entity) : base(entity)
    {
    }
}

public class NameCmp : Component
{
    public NameCmp(Entity entity) : base(entity)
    {
    }
}

public class AncestryCmp : Component
{
    public AncestryCmp(Entity entity) : base(entity)
    {
    }
}

public class ScriptCmp : Component
{
    public ScriptCmp(Entity entity) : base(entity)
    {
    }
}

public class NativeLogicCmp : Component
{
    public NativeLogicCmp(Entity entity) : base(entity)
    {
    }
}

public class TransformCmp : Component
{
    public TransformCmp(Entity entity) : base(entity)
    {
    }
    public Mat4 LocalTransform
    {
        get
        {
            return IC.Transform.GetLocalTransform(entity.nativeEntityId);
        }

        set
        {
            IC.Transform.SetLocalTransform(entity.nativeEntityId, ref value);
        }
    }

    public Mat4 WorldTransform
    {
        get
        {
            return IC.Transform.GetWorldTransform(entity.nativeEntityId);
        }
    }
}

public class SpriteCmp : Component
{
    public SpriteCmp(Entity entity) : base(entity)
    {
    }
}

public class TextCmp : Component
{
    public TextCmp(Entity entity) : base(entity)
    {
    }
}

public class ParticleEmitterCmp : Component
{
    public ParticleEmitterCmp(Entity entity) : base(entity)
    {
    }
}

public class RigidBody2DCmp : Component
{
    public RigidBody2DCmp(Entity entity) : base(entity)
    {
    }
}

public class CameraCmp : Component
{
    public CameraCmp(Entity entity) : base(entity)
    {
    }
}