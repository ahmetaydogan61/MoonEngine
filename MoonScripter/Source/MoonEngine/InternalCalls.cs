using System;
using System.Runtime.CompilerServices;

namespace MoonEngine
{
    internal class InternalCalls
    {
        #region Entity

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(ulong entityId, Type componentType);

        #endregion

        #region Transform

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Transform_SetPosition(ulong entityId, ref Vector3 position);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Transform_GetPosition(ulong entityId, out Vector3 position);

        #endregion

        #region Physics

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PhysicsBody_AddForce(ulong entityId, ref Vector2 force, ref Vector2 position);

        #endregion

        #region Input

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_GetKey(Keycode key);
        
        #endregion
    }
}
