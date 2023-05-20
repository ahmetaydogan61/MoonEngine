using MoonEngine;
using System;

namespace Game
{
    internal class Camera : Entity
    {
        public Entity Other;

        void Update(float dt)
        {
            if(Other != null)
                Transform.Position = Other.Transform.Position;
        }
    }
}
