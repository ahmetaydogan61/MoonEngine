using System;
using MoonEngine;

namespace Game
{
    internal class Player : Entity
    {
        void Awake()
        {
            Console.WriteLine($"Player Awaken - {ID}");
        }

        void Update(float dt)
        {
            float speed = 3.0f;
            Vector3 velocity = Vector3.Zero;

            if (Input.GetKey(Keycode.W))
                velocity.Y = 1.0f;
            else if (Input.GetKey(Keycode.S))
                velocity.Y = -1.0f;

            if (Input.GetKey(Keycode.A))
                velocity.X = -1.0f;
            else if (Input.GetKey(Keycode.D))
                velocity.X = 1.0f;

            velocity *= speed * dt;

            Vector3 pos = Position;
            pos += velocity;
            Position = pos;
        }
    }
}
