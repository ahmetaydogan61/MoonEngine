using MoonEngine;

namespace Game
{
    public class TimedDestroyer : Entity
    {
        private bool m_Enabled = false;
        public float TimeToDestroy = 0.5f;
        private float m_Timer = 0.0f;

        void Update(float dt)
        {
            if(!m_Enabled) 
                return;

            m_Timer += dt;

            if (TimeToDestroy <= m_Timer)
                Destroy(this);
        }

        public void StartCounter()
        {
            m_Enabled = true;
        }

        public void StopCounter()
        {
            m_Enabled = false;
            m_Timer = 0.0f;
        }
    }
}
