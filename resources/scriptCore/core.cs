using System;

namespace nimbus
{
    public class ScriptCore
    {
        public float MyPublicFloatVar = 5.0f;

        public void PrintFloatVar()
        {
            Console.WriteLine("MyPublicFloatVar = {0:F}", MyPublicFloatVar);
        }

        private void IncrementFloatVar(float value)
        {
            MyPublicFloatVar += value;
        }

    }
}