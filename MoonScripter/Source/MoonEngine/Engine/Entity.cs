using System;

namespace MoonEngine
{
    public class Entity
    {
        protected Entity() { }

        internal Entity(ulong id)
        {
            m_InstanceID = id;
            Transform = new TransformComponent() { Entity = this };
        }

        private readonly ulong m_InstanceID = 0;
        private TransformComponent m_Transform;

        public ulong ID => m_InstanceID;

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
