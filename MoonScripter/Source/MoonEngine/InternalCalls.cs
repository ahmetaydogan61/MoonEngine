using System;
using System.Runtime.CompilerServices;

namespace MoonEngine
{
    internal class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(int entityId, Type componentType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Transform_SetPosition(int entityId, ref Vector3 position);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Transform_GetPosition(int entityId, out Vector3 position);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_GetKey(Keycode key);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PhysicsBody_AddForce(int entityId, ref Vector2 force, ref Vector2 position);
    }
}
