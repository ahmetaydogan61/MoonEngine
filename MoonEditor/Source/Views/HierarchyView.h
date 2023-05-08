#pragma once
#include "Views/InspectorView.h"

namespace MoonEngine
{
	class Scene;
	class Entity;

	struct HierarchyView : public BasicView
	{
	public:
		HierarchyView();
		~HierarchyView() = default;
		void Render();
	private:
		void EntityTreeNode(Entity& entity, int id);
		void EntityCreationMenu();
	};
}