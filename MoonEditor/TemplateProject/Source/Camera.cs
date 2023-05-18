using MoonEngine;

namespace Game
{
    public class Camera : Entity
    {
        public float Speed = 8.0f;

        void Update(float dt)
        {
            float speed = Speed;
            Vector3 velocity = Vector3.Zero;

            if (Input.GetKey(Keycode.Up))
                velocity.Y = 1.0f;
            else if (Input.GetKey(Keycode.Down))
                velocity.Y = -1.0f;

            if (Input.GetKey(Keycode.Left))
                velocity.X = -1.0f;
            else if (Input.GetKey(Keycode.Right))
                velocity.X = 1.0f;

            velocity *= speed * dt;

            Vector3 position = Position;
            position += velocity;
            Position = position;
        }
    }
}
