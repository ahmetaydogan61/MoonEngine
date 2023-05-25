using System;
using System.Runtime.CompilerServices;

namespace MoonEngine
{
    internal class InternalCalls
    {
        #region Entity

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Entity_Instantiate(ulong entityId, ref Vector3 position);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Entity_Destroy(ulong entityId);


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static object Entity_GetScript(ulong entityId);

        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(ulong entityId, Type componentType);

        #endregion

        #region Transform

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Transform_GetPosition(ulong entityId, out Vector3 position);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Transform_SetPosition(ulong entityId, ref Vector3 position);

        #endregion

        #region Camera

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Camera_GetSize(ulong entityId);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Camera_SetSize(ulong entityId, float size);

        #endregion

        #region Physics


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PhysicsBody_SetBodyType(ulong entityId, PhysicsBodyComponent.PhysicsBodyType bodyType);


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PhysicsBody_AddForce(ulong entityId, ref Vector2 force, ref Vector2 position);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PhysicsBody_AddImpulse(ulong entityId, ref Vector2 force, ref Vector2 position);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PhysicsBody_AddAngularImpulse(ulong entityId, float impulse);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void PhysicsBody_AddTorque(ulong entityId, float force);

        #endregion

        #region Input

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_GetKey(Keycode key);

        #endregion
    }
}
