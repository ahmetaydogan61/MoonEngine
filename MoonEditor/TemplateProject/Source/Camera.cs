using MoonEngine;
using System;

namespace Game
{
    internal class Camera : Entity
    {
        public Entity Player;

        private CameraComponent m_CameraComponent;
        public float ZoomSpeed = 5.0f;

        void Awake()
        {
            m_CameraComponent = GetComponent<CameraComponent>();
        }

        void Update(float dt)
        {
            //TODO: Lerp
            if (Player != null)
                Transform.Position = Player.Transform.Position;

            if (m_CameraComponent != null)
            {
                bool zoomOut = Input.GetKey(Keycode.LeftShift);

                if (Input.GetKey(Keycode.Q))
                    m_CameraComponent.Size += zoomOut ? dt * ZoomSpeed : dt * -ZoomSpeed;
                if (Input.GetKey(Keycode.E))
                    m_CameraComponent.Size += zoomOut ? dt * ZoomSpeed : dt * ZoomSpeed;
            }
        }
    }
}
