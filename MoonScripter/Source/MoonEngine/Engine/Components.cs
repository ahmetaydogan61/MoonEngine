using System;

namespace MoonEngine
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }

    public class TransformComponent : Component
    {
        public Vector3 Position
        {
            get
            {
                InternalCalls.Transform_GetPosition(Entity.ID, out Vector3 position);
                return position;
            }
            set
            {
                InternalCalls.Transform_SetPosition(Entity.ID, ref value);
            }
        }
    }

    public class CameraComponent : Component
    {
        public float Size
        {
            get
            {
                return InternalCalls.Camera_GetSize(Entity.ID);
            }
            set
            {
                InternalCalls.Camera_SetSize(Entity.ID, value);
            }
        }
    }

    public class PhysicsBodyComponent : Component
    {
        public enum PhysicsBodyType
        {
            Static, Dynamic, Kinematic
        }

        public PhysicsBodyType BodyType
        {
            set { InternalCalls.PhysicsBody_SetBodyType(Entity.ID, value); }
        }

        public void AddForce(Vector2 Force, Vector2 Position = new Vector2())
        {
            InternalCalls.PhysicsBody_AddForce(Entity.ID, ref Force, ref Position);
        }

        public void AddImpulse(Vector2 Force, Vector2 Position = new Vector2())
        {
            InternalCalls.PhysicsBody_AddImpulse(Entity.ID, ref Force, ref Position);
        }

        public void AddAngularImpulse(float impulse)
        {
            InternalCalls.PhysicsBody_AddAngularImpulse(Entity.ID, impulse);
        }

        public void AddTorque(float force)
        {
            InternalCalls.PhysicsBody_AddTorque(Entity.ID, force);
        }
    }
}
