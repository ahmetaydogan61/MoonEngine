using System;
using MoonEngine;

namespace Game
{
    public class Player : Entity
    {
        public Entity BulletEntity;
        private PhysicsBodyComponent PhysicsBody;

        public float Speed = 1500.0f;
        public float ShootForce = 5.0f;
        public float FireRate = 0.5f;

        private float m_FireTimer = 0.0f;

        void Awake()
        {
            PhysicsBody = GetComponent<PhysicsBodyComponent>();
            m_FireTimer = FireRate;
        }

        void Update(float dt)
        {
            if (PhysicsBody != null)
            {
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

            m_FireTimer -= dt;

            if (m_FireTimer < 0.0f)
            {
                if (Input.GetKey(Keycode.Space))
                {
                    m_FireTimer = FireRate;
                    Shoot();
                }
            }
        }

        public void Shoot()
        {
            Entity e = Instantiate(BulletEntity, Transform.Position + new Vector3(0.5f, 0f, 0f));

            float force = ShootForce;
            PhysicsBodyComponent physicsBody = e.GetComponent<PhysicsBodyComponent>();
            physicsBody.BodyType = PhysicsBodyComponent.PhysicsBodyType.Dynamic;
            physicsBody.AddImpulse(new Vector2(force, 0.0f), new Vector2(Transform.Position.X, Transform.Position.Y));
            TimedDestroyer destroyer = e.As<TimedDestroyer>();
            destroyer.StartCounter();
        }
    }
}
