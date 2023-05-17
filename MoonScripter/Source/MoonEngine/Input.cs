namespace MoonEngine
{
    public class Input
    {
        public static bool GetKey(Keycode key)
        {
            return InternalCalls.Input_GetKey(key);
        }
    }
}
