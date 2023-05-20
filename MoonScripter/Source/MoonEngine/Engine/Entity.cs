using System;

namespace MoonEngine
{
    public class Entity
    {
        protected Entity() { }

        internal Entity(ulong id)
        {
            ID = id;
            Transform = new TransformComponent() { Entity = this };
        }

        public ulong ID;
        private TransformComponent m_Transform;

        public TransformComponent Transform
        {
            get
            {
                return m_Transform;
            }

            private set
            {
                m_Transform = value;
            }
        }

        public T GetComponent<T>() where T : Component, new()
        {
            if (!InternalCalls.Entity_HasComponent(ID, typeof(T)))
                return null;

            return new T() { Entity = this };
        }
    }
}
