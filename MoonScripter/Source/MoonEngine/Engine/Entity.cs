using System;

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
            get
            {
                InternalCalls.Transform_GetPosition(ID, out Vector3 position);
                return position;
            }
            set
            {
                InternalCalls.Transform_SetPosition(ID, ref value);
            }
        }

        public bool HasComponent<T>() where T: Component, new()
        {
            Type component = typeof(T);
            return InternalCalls.Entity_HasComponent(ID, component);
        }

        public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return null;

            T component = new T() { Entity = this };
            return component;
        }
    }
}
