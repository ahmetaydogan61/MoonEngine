#include "HierarchyView.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "imgui/misc/cpp/imgui_stdlib.cpp"
#include "Engine/Components.h"
#include "../ImGuiUtils.h"
#include <imgui/imgui.cpp>
#include "Utils/IconsFontAwesome.h"

namespace MoonEngine
{
	void HierarchyView::SetScene(Scene* scene)
	{
		m_Scene = scene;
	}

	void HierarchyView::MouseSelect()
	{
		bool selected = false;
		float x = Input::OrthoX();
		float y = Input::OrthoY();

		auto transforms = m_Scene->m_Registry.view<TransformComponent>();
		for (auto entity : transforms)
		{
			glm::vec2 pos = transforms.get<TransformComponent>(entity).position;
			glm::vec2 scale = transforms.get<TransformComponent>(entity).size;
			float halfExtendX = scale.x / 2.0f;
			float halfExtendY = scale.y / 2.0f;

			if (x < (pos.x + halfExtendX) && x >(pos.x - halfExtendX))
			{
				if (y < (pos.y + halfExtendY) && y >(pos.y - halfExtendY))
				{
					selected = true;
					m_SelectedEntity = Entity{ entity };
					break;
				}
			}
		}

		if (!selected)
			m_SelectedEntity = {};
	}

	void HierarchyView::BeginHierarchyView(bool& state)
	{
		ImGui::Begin(ICON_FK_LIST_UL "Hierarchy", &state);

		int id = 0;
		m_Scene->m_Registry.each([&](auto entityID)
		{
			Entity entity{ entityID };
			EntityTreeNode(entity, id++);
		});

		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create Camera"))
			{
				Entity& entity = m_Scene->CreateEntity();
				entity.GetComponent<IdentityComponent>().Name = "Camera";
				entity.AddComponent<CameraComponent>();
				entity.RemoveComponent<SpriteComponent>();
			}

			if (ImGui::MenuItem("Create Entity"))
				m_Scene->CreateEntity();

			ImGui::EndPopup();
		}

		if (!ImGui::IsAnyItemHovered() && ImGui::IsAnyMouseDown() && ImGui::IsWindowHovered())
			m_SelectedEntity = {};

		ImGui::End();
	}

	void HierarchyView::EntityTreeNode(Entity entity, int id)
	{
		std::string name = entity.GetComponent<IdentityComponent>().Name;
		name += "##" + std::to_string(id);

		ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		bool opened = ImGui::TreeNodeEx(name.c_str(), flags);
		if (ImGui::IsItemClicked(0))
			m_SelectedEntity = entity;

		if (ImGui::BeginPopupContextItem())
		{
			m_SelectedEntity = entity;
			if (ImGui::MenuItem("Delete Entity"))
			{
				entity.Destroy();
				if (m_SelectedEntity == entity)
					m_SelectedEntity = {};
			}
			ImGui::EndPopup();
		}

		if (opened)
			ImGui::TreePop();
	}

	//This part contains inspector stuff
	void HierarchyView::BeginInspectorView(bool& state)
	{
		ImGui::Begin(ICON_FK_SEARCH "Inspector", &state);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Close")) ;
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (m_SelectedEntity)
		{
			ImGuiUtils::TextCentered("Name", true);
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().FramePadding.x);
			ImGui::InputText("##Name", &m_SelectedEntity.GetComponent<IdentityComponent>().Name);

			ImGuiUtils::AddPadding(0, 5);
			ImGui::Separator();
			ImGuiUtils::AddPadding(0, 5);

			//Translation Start
			ImGui::Text("Transform");
			TransformComponent& transform = m_SelectedEntity.GetComponent<TransformComponent>();
			UtilVectorColumn("Position", transform.position);
			UtilVectorColumn("Size", transform.size, 1.0f);
			//Translation End

			ImGuiUtils::AddPadding(0, 5);
			ImGui::Separator();
			ImGuiUtils::AddPadding(0, 5);

			ImGuiUtils::TextCentered("Add Component", true);

			if (ImGui::Button(" + ", { 25.0f, 25.0f }))
				ImGui::OpenPopup("AddComponents");
			if (ImGui::BeginPopup("AddComponents"))
			{
				if (ImGui::MenuItem("Sprite"))
					if (!m_SelectedEntity.HasComponent<SpriteComponent>())
						m_SelectedEntity.AddComponent<SpriteComponent>();

				if (ImGui::MenuItem("Camera"))
					if (!m_SelectedEntity.HasComponent<CameraComponent>())
						m_SelectedEntity.AddComponent<CameraComponent>();
				ImGui::EndPopup();
			}

			ImGuiUtils::AddPadding(0, 5);
			ComponentView();
		}
		ImGui::End();
	}

	void HierarchyView::ComponentView()
	{
		float color = Maths::Normalize(75.0f, 0.0f, 255.0f);
		float alpha = Maths::Normalize(150.0f, 0.0f, 255.0f);
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(color, color, color, alpha));
		alpha = Maths::Normalize(175.0f, 0.0f, 255.0f);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(color, color, color, alpha));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(color, color, color, alpha));

		ShowComponent<SpriteComponent>("Sprite", [](auto& component)
		{
			ImGui::ColorEdit3("Color", &component.color[0]);
		});

		ShowComponent<CameraComponent>("Camera", [](auto& component)
		{
			float winWidth = 100.0f;

			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, winWidth);
			ImGuiUtils::TextCentered("Is Main", false);
			ImGui::NextColumn();
			ImGui::Checkbox("##IsMain", &component.isMain);
			ImGui::Columns(1);

			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, winWidth);
			ImGuiUtils::TextCentered("Distance", true);
			ImGui::NextColumn();
			ImGui::DragFloat("##Distance", &component.distance, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::Columns(1);
		});

		ImGui::PopStyleColor(3);
	}

	template<typename T, typename Function>
	void HierarchyView::ShowComponent(std::string componentName, Function function)
	{
		if (m_SelectedEntity.HasComponent<T>())
		{
			float buttonSize = 25.0f;
			ImVec2 contentRegion = ImGui::GetContentRegionAvail();
			T& component = m_SelectedEntity.GetComponent<T>();

			ImGui::AlignTextToFramePadding();
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed;
			bool treeopen = ImGui::TreeNodeEx(componentName.c_str(), flags);
			ImGui::SameLine(contentRegion.x - (buttonSize / 2) - ImGui::GetStyle().FramePadding.x);
			if (ImGui::Button(":", { buttonSize, ImGui::GetFrameHeight() }))
				ImGui::OpenPopup("SettingList");

			if (ImGui::BeginPopup("SettingList"))
			{
				if (ImGui::MenuItem("Remove"))
				{
					m_SelectedEntity.RemoveComponent<T>();
				}
				ImGui::EndPopup();
			}

			if (treeopen)
			{
				function(component);
				ImGui::TreePop();
			}
		}
	}

	void HierarchyView::UtilVectorColumn(const std::string& vecName, glm::vec2& vector, float resetValue, float columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::PushID(vecName.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGuiUtils::TextCentered(vecName.c_str(), false);
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 2.5f });

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.75f, 0.15f, 0.1f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 1.0f, 0.15f, 0.1f, 1.0f });

		if (ImGui::Button(" X "))
			vector.x = resetValue;
		ImGui::SameLine();
		ImGui::DragFloat("##X", &vector.x, 0.1f, 0.0f, 0.0f, "%.2f");

		ImGui::PopItemWidth();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.25f, 0.6f, 0.0f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.25f, 0.75f, 0.1f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.25f, 0.85f, 0.0f, 1.0f });

		if (ImGui::Button(" Y "))
			vector.y = resetValue;

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &vector.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::PopStyleColor(3);

		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
	}
}