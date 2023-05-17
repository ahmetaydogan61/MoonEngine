namespace MoonEngine
{
    public struct Vector2
    {
        public float X, Y;

        public static Vector2 Zero => new Vector2(0.0f);
        public static Vector2 One => new Vector2(1.0f);

        public Vector2(float x, float y) { X = x; Y = y; }

        public Vector2(float val) { X = val; Y = val; }

        public static Vector2 operator *(Vector2 v, float mul)
        {
            return new Vector2(v.X * mul, v.Y * mul);
        }

        public static Vector2 operator +(Vector2 v1, Vector2 v2)
        {
            return new Vector2(v1.X + v2.X, v1.Y + v2.Y);
        }
    }
}
