using MoonEngine;
using System;

namespace Game
{
    public class Camera : Entity
    {
        public float Speed = 8.0f;

        private Entity Player;

        void Awake()
        {
            Player = FindEntityByName("Player");
        }

        void Update(float dt)
        {
            if(Player != null)
            {
                Position = Player.Position;
            }
        }
    }
}
