namespace MoonEngine
{
    public struct Vector3
    {
        public float X, Y, Z;

        public static Vector3 Zero => new Vector3(0.0f);
        public static Vector3 One => new Vector3(1.0f);

        public Vector3(float x, float y, float z) { X = x; Y = y; Z = z; }

        public Vector3(float val) { X = val; Y = val; Z = val; }

        public static Vector3 operator *(Vector3 v, float mul)
        {
            return new Vector3(v.X * mul, v.Y * mul, v.Z * mul);
        }

        public static Vector3 operator +(Vector3 v1, Vector3 v2)
        {
            return new Vector3(v1.X + v2.X, v1.Y + v2.Y, v1.Z + v2.Z);
        }
    }
}
