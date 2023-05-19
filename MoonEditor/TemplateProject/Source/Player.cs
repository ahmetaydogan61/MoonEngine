using System;
using MoonEngine;

namespace Game
{
    public class Player : Entity
    {
        private PhysicsBodyComponent m_PhysicsBody;

        public float Speed = 2500.0f;
        public Camera m_Camera;

        void Awake()
        {
            m_PhysicsBody = GetComponent<PhysicsBodyComponent>();
            m_Camera = FindEntityByName("Camera").As<Camera>();
        }

        void Update(float dt)
        {
            m_Camera.Speed = Speed;

            float speed = Speed;
            Vector2 velocity = Vector2.Zero;

            if (Input.GetKey(Keycode.W))
                velocity.Y = 1.0f;
            else if (Input.GetKey(Keycode.S))
                velocity.Y = -1.0f;

            if (Input.GetKey(Keycode.A))
                velocity.X = -1.0f;
            else if (Input.GetKey(Keycode.D))
                velocity.X = 1.0f;

            velocity *= speed * dt;

            m_PhysicsBody.AddForce(velocity);
        }
    }
}
