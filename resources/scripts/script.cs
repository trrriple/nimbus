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

        Vec4 testV = new Vec4(1.0f, 2.0f, 3.0f, 4.0f);
        Mat4 testM = new Mat4(1.123f, 2.234f, 3.345f, 4.456f,
                              11.123f, 12.234f, 13.345f, 14.456f,
                              21.123f, 22.234f, 23.345f, 24.456f,
                              31.123f, 32.234f, 33.345f, 34.456f);

        Vec4 testVM = testM * testV;

        InternalCalls.vec4Test(ref testVM);


    }

    protected override void OnUpdate(float deltaTime)
    {
        InternalCalls.Info($"ExamplePlayer onUpdate @ {deltaTime}");
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
