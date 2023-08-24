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

        Vector4 test = new Vector4(1.123f, 2.234f, 3.345f, 4.456f);
        Matrix4x4 testMat = new Matrix4x4(1.123f, 2.234f, 3.345f, 4.456f,
                                          11.123f, 12.234f, 13.345f, 14.456f,
                                          21.123f, 22.234f, 23.345f, 24.456f,
                                          31.123f, 32.234f, 33.345f, 34.456f);


        InternalCalls.vec4Test(ref test);
        InternalCalls.mat4Test(ref testMat);


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
