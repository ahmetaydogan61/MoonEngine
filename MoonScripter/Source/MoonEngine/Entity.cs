namespace MoonEngine
{
    public class Entity
    {
        protected Entity() { }

        internal Entity(int id)
        {
            ID = id;
        }

        public readonly int ID;

        public Vector3 Position
        {
            set
            {
                InternalCalls.Entity_SetPosition(ID, ref value);
            }
            get
            {
                InternalCalls.Entity_GetPosition(ID, out Vector3 position);
                return position;
            }
        }
    }
}
