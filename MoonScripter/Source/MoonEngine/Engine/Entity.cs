using System;

namespace MoonEngine
{
    public class Entity
    {
        protected Entity() { }

        internal Entity(ulong id)
        {
            ID = id;
        }

        public readonly ulong ID;

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

        public Entity FindEntityByName(string name)
        {
            ulong entityId = InternalCalls.Entity_FindByName(name);
            if(entityId == 0)
                return null;

            return new Entity(entityId);
        }

        public T As<T>() where T : Entity, new()
        {
            object instance = InternalCalls.GetScriptInstance(ID);
            return instance as T;
        }
    }
}
