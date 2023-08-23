using System;
using Nimbus;

namespace Script;

public class ExamplePlayer : Entity
{

    protected override void OnCreate()
    {

    }

    protected override void OnUpdate(float deltaTime)
    {
        InternalCalls.Info($"On update @ {deltaTime}");
    }


}
