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
    }

    protected override void OnUpdate(float deltaTime)
    {

        // Vec3 scale = InternalCalls.GetWorldScale(nativeEntityId);
        // Vec3 rotation = InternalCalls.GetWorldRotation(nativeEntityId);

        // rotation.Z += 0.0174533f;
        // scale.X += 0.0001f;

        // InternalCalls.SetLocalRotation(nativeEntityId, ref rotation);
        // InternalCalls.SetLocalScale(nativeEntityId, ref scale);

        Vec3 translation = IC.Transform.GetWorldTranslation(nativeEntityId);
        Vec3 rotation = IC.Transform.GetWorldRotation(nativeEntityId);


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


        IC.Transform.SetLocalTranslation(nativeEntityId, ref translation);
        IC.Transform.SetLocalRotation(nativeEntityId, ref rotation);


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

    protected override void OnCreate()
    {

    }

    protected override void OnUpdate(float deltaTime)
    {
        IC.Log.Info($"On update @ {deltaTime}");
    }

}
