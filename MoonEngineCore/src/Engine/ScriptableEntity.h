#pragma once
#include "Entity.h"

namespace MoonEngine
{
	class ScriptableEntity
	{
	private:
		friend class Scene;
	protected:
		Entity m_Entity;
	public:
		virtual ~ScriptableEntity() {}

		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}

		void Destroy()
		{
			OnDestroy();
			m_Entity.Destroy();
		}
	protected:
		virtual void Awake() {}
		virtual void Update() {}
		virtual void OnDestroy() {}
	};
}
