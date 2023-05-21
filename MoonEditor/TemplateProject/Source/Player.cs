using System;
using MoonEngine;

namespace Game
{
    public class Player : Entity
    {
        public float Speed = 1500.0f;
        private PhysicsBodyComponent PhysicsBody;

        void Awake()
        {
            PhysicsBody = GetComponent<PhysicsBodyComponent>();
        }

        void Update(float dt)
        {
            if (PhysicsBody == null)
                return;

            Vector2 velocity = Vector2.Zero;

            if (Input.GetKey(Keycode.A))
                velocity.X -= Speed;
            else if (Input.GetKey(Keycode.D))
                velocity.X += Speed;

            if (Input.GetKey(Keycode.W))
                velocity.Y += Speed;
            else if (Input.GetKey(Keycode.S))
                velocity.Y -= Speed;
            
            PhysicsBody.AddForce(velocity * dt);
        }
    }
}
