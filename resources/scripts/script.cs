using System;
using Nimbus;
using System.Numerics;


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
        InternalCalls.Info("Created ExamplePlayer!");
    }

    protected override void OnUpdate(float deltaTime)
    {

        // Vec3 scale = InternalCalls.GetWorldScale(nativeEntityId);
        // Vec3 rotation = InternalCalls.GetWorldRotation(nativeEntityId);

        // rotation.Z += 0.0174533f;
        // scale.X += 0.0001f;

        // InternalCalls.SetLocalRotation(nativeEntityId, ref rotation);
        // InternalCalls.SetLocalScale(nativeEntityId, ref scale);

        Vec3 translation = InternalCalls.GetWorldTranslation(nativeEntityId);
        Vec3 rotation = InternalCalls.GetWorldRotation(nativeEntityId);


        if(InternalCalls.KeyPressed(ScanCode.d))
        {
            translation.X += 10.0f * deltaTime;
        }
        if(InternalCalls.KeyPressed(ScanCode.a))
        {
            translation.X += -10.0f * deltaTime;
        }
        if(InternalCalls.KeyPressed(ScanCode.w))
        {
            translation.Y += 10.0f * deltaTime;
        }
        if(InternalCalls.KeyPressed(ScanCode.s))
        {
            translation.Y += -10.0f * deltaTime;
        }

        if(InternalCalls.MouseButtonPressed(MouseButton.left))
        {
            rotation.Z += 1.0f * deltaTime;
        }
        if(InternalCalls.MouseButtonPressed(MouseButton.right))
        {
            rotation.Z += -1.0f * deltaTime;
        }


        InternalCalls.SetLocalTranslation(nativeEntityId, ref translation);
        InternalCalls.SetLocalRotation(nativeEntityId, ref rotation);


    }

    protected override void OnDestroy()
    {
        InternalCalls.Info("Destroyed ExamplePlayer!");
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
        InternalCalls.Info($"On update @ {deltaTime}");
    }

}
