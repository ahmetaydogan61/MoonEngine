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
    public class PhysicsBodyComponent : Component
    {
        public void AddForce(Vector2 Force, Vector2 Position = new Vector2())
        {
            InternalCalls.PhysicsBody_AddForce(Entity.ID, ref Force, ref Position);
        }
    }
}
