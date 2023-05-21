using MoonEngine;
using System;

namespace Game
{
    internal class Camera : Entity
    {
        private CameraComponent m_CameraComponent;
        public Entity Other;
        public float ZoomSpeed = 5.0f;

        void Awake()
        {
            m_CameraComponent = GetComponent<CameraComponent>();
        }

        void Update(float dt)
        {
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
