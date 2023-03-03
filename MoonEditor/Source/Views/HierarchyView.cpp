#include "mpch.h"
#include "Views/HierarchyView.h"
#include "EditorLayer.h"

#include <Engine/Components.h>
#include <Engine/Entity.h>
#include <Gui/ImGuiUtils.h>

#include <IconsMaterialDesign.h>

#include <imgui.h>

namespace MoonEngine
{
	void HierarchyView::EntityTreeNode(Entity& entity, int id)
	{
		std::string name = entity.GetComponent<IdentityComponent>().Name;
		name += "##" + std::to_string(id);

		EditorLayer& editor = *EditorLayer::Get();

		ImGuiTreeNodeFlags flags = ((editor.GetSelectedEntity() == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		bool opened = ImGui::TreeNodeEx(name.c_str(), flags);

		if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1))
			editor.SetSelectedEntity(entity);

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				entity.Destroy();
				if (editor.GetSelectedEntity() == entity)
					editor.SetSelectedEntity({});
			}
			ImGui::EndPopup();
		}

		if (opened)
			ImGui::TreePop();
	}

	void HierarchyView::Render(bool& renderHierarchy, bool& renderInspector)
	{
		if (renderHierarchy)
		{
			EditorLayer& editor = *EditorLayer::Get();

			float height = ImGui::GetFrameHeight();
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
			bool openPopup = false;

			ImGui::Begin(ICON_MD_SORT "Hierarchy", &renderHierarchy, window_flags);

			if (ImGui::BeginMenuBar())
			{
				const auto& contentRegion = ImGui::GetContentRegionAvail();
				float buttonSize = ImGui::GetFontSize() + 5.0f;
				ImGuiUtils::AddPadding(contentRegion.x - (buttonSize * 0.5f) - (ImGui::GetStyle().FramePadding.x * 2.0f), 0);
				if (ImGui::Button(":", { buttonSize, height }))
					openPopup = true;
				ImGui::EndMenuBar();
			}

			if (openPopup)
				ImGui::OpenPopup("CreateStuff");

			if (ImGui::BeginPopup("CreateStuff"))
			{
				if (ImGui::MenuItem("Entity"))
				{
					Entity entity = m_Scene->CreateEntity();
					editor.SetSelectedEntity(entity);
				}
				if (ImGui::MenuItem("Camera"))
				{
					Entity entity = m_Scene->CreateEntity();
					entity.GetComponent<IdentityComponent>().Name = "Camera";
					entity.AddComponent<CameraComponent>();
					entity.RemoveComponent<SpriteComponent>();
					editor.SetSelectedEntity(entity);
				}
				ImGui::EndPopup();
			}

			if (ImGui::BeginPopupContextWindow(0, 1))
			{
				if (ImGui::MenuItem("Entity"))
				{
					Entity entity = m_Scene->CreateEntity();
					editor.SetSelectedEntity(entity);
				}
				if (ImGui::MenuItem("Camera"))
				{
					Entity entity = m_Scene->CreateEntity();
					entity.GetComponent<IdentityComponent>().Name = "Camera";
					entity.AddComponent<CameraComponent>();
					entity.RemoveComponent<SpriteComponent>();
					editor.SetSelectedEntity(entity);
				}
				ImGui::EndPopup();
			}

			int id = 0;
			m_Scene->m_Registry.each_reverse([&](auto entityID)
			{
				Entity entity{ entityID, m_Scene.get() };
			EntityTreeNode(entity, id++);
			});

			if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				editor.SetSelectedEntity({});
			ImGui::End();
		}

		if (renderInspector)
			m_InspectorView.Render(renderInspector);
	}
}

