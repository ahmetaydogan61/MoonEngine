using System;

namespace MoonEngine
{
    public class Main
    {
        public float Float { get; set; }

        public Main()
        {
            Console.WriteLine("Hello Constructer");
        }

        public void PrintMessage()
        {
            Console.WriteLine("Hello Message");
        }

        public void PrintMessage(int counter)
        {
            Console.WriteLine($"Custom Message: {counter}");
        }
    }
}
