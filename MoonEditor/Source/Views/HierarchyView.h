#pragma once
#include "Views/InspectorView.h"

namespace MoonEngine
{
	class Scene;
	class Entity;

	class HierarchyView
	{
	private:
		Shared<Scene> m_Scene;
		InspectorView m_InspectorView;

		void EntityTreeNode(Entity& entity, int id);
	public:
		void SetScene(const Shared<Scene>& scene) { m_Scene = scene; }
		void Render(bool& renderHierarchy, bool& renderInspector);
	};
}