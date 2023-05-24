using MoonEngine;
using System;

namespace Game
{
    internal class Camera : Entity
    {
        public Vector2 StartPosition = new Vector2(1.0f, 5.0f);
        private CameraComponent m_CameraComponent;
        public Entity Other;
        public float ZoomSpeed = 5.0f;
        public double ADoubleVal = 5.01;
        public int IntegerTest = 5;
        public short ShortTest = 4;
        public long LongTest = 1;
        public bool s = true;
        public bool ss = false;
        public bool sas = false;

        void Awake()
        {
            m_CameraComponent = GetComponent<CameraComponent>();
        }

        void Update(float dt)
        {
            //Console.WriteLine($"s:{s}, ss:{ss}, sas:{sas}");
            //Console.WriteLine($"Integer:{IntegerTest}");
            //Console.WriteLine($"Vec2: x {StartPosition.X}, y {StartPosition.Y}");

            if (Other != null)
                Transform.Position = Other.Transform.Position;


            if (m_CameraComponent != null)
            {
                bool zoomOut = Input.GetKey(Keycode.LeftShift);
            
                if (Input.GetKey(Keycode.Z))
                    m_CameraComponent.Size += zoomOut ? dt * ZoomSpeed : dt * -ZoomSpeed;
            }
        }
    }
}
