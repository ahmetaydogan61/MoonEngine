#pragma once
#include "Engine/Entity.h"

namespace MoonEngine
{
	class Entity;

	class HierarchyView
	{
	private:
		Scene* m_Scene;
		Entity m_SelectedEntity;

		void EntityTreeNode(Entity entity, int id);

		void ComponentView();
		template<typename T, typename Function>
		void ShowComponent(std::string componentName, Function func);
		void UtilVectorColumn(const std::string& vecName, glm::vec2& vector, float resetValue = 0.0f, float columnWidth = 100.0f);
	public:
		void SetScene(Scene* scene);
		void BeginHierarchyView(bool& state);
		void BeginInspectorView(bool& state);
		void MouseSelect();
	};
}