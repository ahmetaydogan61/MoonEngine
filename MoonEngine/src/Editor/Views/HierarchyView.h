#pragma once
#include "Core.h"

namespace MoonEngine
{
	class HierarchyView
	{
	private:
		Ref<Scene> m_Scene;
		Entity m_SelectedEntity;

		void CreateOptions();
		void EntityTreeNode(Entity entity, int id);
		
		void ComponentView();
		template<typename T, typename Function>
		void ShowComponent(std::string componentName, Function func);
		void UtilVectorColumn(const std::string& vecName, glm::vec3& vector, float resetValue = 0.0f, float columnWidth = 100.0f);

	public:
		HierarchyView();
		~HierarchyView() = default;
		void SetScene(Ref<Scene> scene);
		void BeginHierarchyView(bool& state);
		void BeginInspectorView(bool& state);
		void MouseSelect();
		Entity GetSelectedEntity() const { return m_SelectedEntity; }
		void DuplicateSelectedEntity();
		void DeleteSelectedEntity();
	};
}
