﻿using System;
using MoonEngine;

namespace Game
{
    internal class Player : Entity
    {
        private TransformComponent m_Transform;
        private PhysicsBodyComponent m_PhysicsBody;

        void Awake()
        {
            m_Transform = GetComponent<TransformComponent>();
            m_PhysicsBody = GetComponent<PhysicsBodyComponent>();

            bool hasTransform = HasComponent<TransformComponent>();
            Console.WriteLine($"Has Transfrom Component - {hasTransform}");

            Console.WriteLine($"Player Awaken - {ID}");
        
        }

        void Update(float dt)
        {
            float speed = 2500.0f;
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