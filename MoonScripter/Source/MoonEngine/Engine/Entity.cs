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

        private ulong m_InstanceID = 0;
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
            if (!InternalCalls.Entity_HasComponent(m_InstanceID, typeof(T)))
                return null;

            return new T() { Entity = this };
        }

        public T As<T>() where T : Entity, new()
        {
            object instance = InternalCalls.Entity_GetScript(m_InstanceID);
            if (instance == null)
            {
                Console.WriteLine("Instance not found!");
                return null;
            }
            return instance as T;
        }

        public static Entity Instantiate(Entity entity, Vector3 position)
        {
            return new Entity() { m_InstanceID = InternalCalls.Entity_Instantiate(entity.m_InstanceID, ref position) };
        }

        public static void Destroy(Entity entity)
        {
            InternalCalls.Entity_Destroy(entity.m_InstanceID);
        }
    }
}
 