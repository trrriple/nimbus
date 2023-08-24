using System;
using Nimbus;

namespace Script;

public class ExamplePlayer : Entity
{

    protected override void OnCreate()
    {
        InternalCalls.Info("Created ExamplePlayer Audrey!");

    }

    protected override void OnUpdate(float deltaTime)
    {
        InternalCalls.Info($"ExamplePlayer onUpdate Audrey @ {deltaTime}");
    }

    protected override void OnDestroy()
    {
        InternalCalls.Info("Destroyed ExamplePlayer Audrey!");
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
