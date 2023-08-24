using System;
using Nimbus;

namespace Script;

public class ExamplePlayer : Entity
{

    protected override void OnCreate()
    {
        InternalCalls.Info("Created ExamplePlayer!");

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

    protected override void OnCreate()
    {

    }

    protected override void OnUpdate(float deltaTime)
    {
        InternalCalls.Info($"On update @ {deltaTime}");
    }

}
