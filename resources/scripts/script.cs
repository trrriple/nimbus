using System;
using Nimbus;
using System.Numerics;

using IC = Nimbus.InternalCalls;

using System.Runtime.CompilerServices;

[assembly: DisableRuntimeMarshalling]

namespace Script;
public class ExamplePlayer : Entity
{

    public ExamplePlayer(uint nativeEntityId) : base(nativeEntityId)
    {
    }

    protected override void OnCreate()
    {
        IC.Log.Info("Created ExamplePlayer!");

        bool hasTc = HasComponent<TransformCmp>();
        bool hasTxC = HasComponent<TextCmp>();

        IC.Log.Info($"Has TransformCmp {hasTc}, has TextCmp {hasTxC}");

        AddComponent<TextCmp>();

        hasTxC = HasComponent<TextCmp>();

        IC.Log.Info($"Has TransformCmp {hasTc}, has TextCmp {hasTxC}");



    }

    protected override void OnUpdate(float deltaTime)
    {

        // Vec3 scale = InternalCalls.GetWorldScale(nativeEntityId);
        // Vec3 rotation = InternalCalls.GetWorldRotation(nativeEntityId);

        // rotation.Z += 0.0174533f;
        // scale.X += 0.0001f;

        // InternalCalls.SetLocalRotation(nativeEntityId, ref rotation);
        // InternalCalls.SetLocalScale(nativeEntityId, ref scale);

        Vec3 translation = IC.Transform.GetWorldTranslation(m_nativeEntityId);
        Vec3 rotation = IC.Transform.GetWorldRotation(m_nativeEntityId);


        if(IC.Input.KeyDown(ScanCode.d))
        {
            translation.X += 10.0f * deltaTime;
        }
        if(IC.Input.KeyDown(ScanCode.a))
        {
            translation.X += -10.0f * deltaTime;
        }
        if(IC.Input.KeyDown(ScanCode.w))
        {
            translation.Y += 10.0f * deltaTime;
        }
        if(IC.Input.KeyDown(ScanCode.s))
        {
            translation.Y += -10.0f * deltaTime;
        }

        if(IC.Input.MouseButtonDown(MouseButton.left))
        {
            rotation.Z += 1.0f * deltaTime;
        }
        if(IC.Input.MouseButtonDown(MouseButton.right))
        {
            rotation.Z += -1.0f * deltaTime;
        }


        IC.Transform.SetLocalTranslation(m_nativeEntityId, ref translation);
        IC.Transform.SetLocalRotation(m_nativeEntityId, ref rotation);


    }

    protected override void OnDestroy()
    {
        IC.Log.Info("Destroyed ExamplePlayer!");
    }

}

public class ExampleCamera : Entity
{
    public ExampleCamera(uint nativeEntityId) : base(nativeEntityId)
    {
    }
}


public class Placeholder0 : Entity
{
    public Placeholder0(uint nativeEntityId) : base(nativeEntityId)
    {
    }
}

public class Placeholder1 : Entity
{
    public Placeholder1(uint nativeEntityId) : base(nativeEntityId)
    {
    }
}

public class Placeholder2 : Entity
{
    public Placeholder2(uint nativeEntityId) : base(nativeEntityId)
    {
    }
}

public class Placeholder3 : Entity
{
    public Placeholder3(uint nativeEntityId) : base(nativeEntityId)
    {
    }
}

public class Placeholder4 : Entity
{
    public Placeholder4(uint nativeEntityId) : base(nativeEntityId)
    {
    }
}

public class Placeholder5 : Entity
{
    public Placeholder5(uint nativeEntityId) : base(nativeEntityId)
    {
    }
}

public class Placeholder6 : Entity
{
    public Placeholder6(uint nativeEntityId) : base(nativeEntityId)
    {
    }
}

public class Placeholder7 : Entity
{
    public Placeholder7(uint nativeEntityId) : base(nativeEntityId)
    {
    }
}

public class Placeholder8 : Entity
{
    public Placeholder8(uint nativeEntityId) : base(nativeEntityId)
    {
    }
}

public class Placeholder9 : Entity
{
    public Placeholder9(uint nativeEntityId) : base(nativeEntityId)
    {
    }
}

public class Placeholder10 : Entity
{
    public Placeholder10(uint nativeEntityId) : base(nativeEntityId)
    {
    }
}
