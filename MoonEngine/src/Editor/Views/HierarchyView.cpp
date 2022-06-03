#include "HierarchyView.h"
#include "Engine/Components.h"
#include "../ImGuiUtils.h"
#include "Utils/IconsFontAwesome.h"

#include <imgui/imgui.cpp>
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "imgui/misc/cpp/imgui_stdlib.cpp"
#include <filesystem>

namespace MoonEngine
{
	Ref<Texture> m_NoSpriteTexture;
	UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;

	void HierarchyView::SetScene(Ref<Scene> scene)
	{
		m_Scene = scene;
		m_SelectedEntity = {};
		m_NoSpriteTexture = CreateRef<Texture>("res/EditorIcons/Frame.png");
	}

	void HierarchyView::MouseSelect()
	{
		bool selected = false;
		float x = Input::OrthoX();
		float y = Input::OrthoY();

		auto transforms = m_Scene->m_Registry.view<TransformComponent>();
		for (auto entity : transforms)
		{
			glm::vec2 pos = transforms.get<TransformComponent>(entity).Position;
			glm::vec2 scale = transforms.get<TransformComponent>(entity).Size;
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
		bool createEntity = false;
		bool createCamera = false;

		float height = ImGui::GetFrameHeight();
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;

		ImGui::Begin(ICON_FK_LIST_UL "Hierarchy", &state, window_flags);

		if (ImGui::BeginMenuBar())
		{
			auto& contentRegion = ImGui::GetContentRegionAvail();
			float buttonSize = 25.0f;
			ImGuiUtils::AddPadding(contentRegion.x - (buttonSize / 2.0f) - (ImGui::GetStyle().FramePadding.x * 2.0f), 0);
			if (ImGui::Button(":", { 25.0f, 25.0f }))
				ImGui::OpenPopup("CreateStuff");
			if (ImGui::BeginPopup("CreateStuff"))
			{
				if (ImGui::MenuItem("Create Camera"))
					createCamera = true;

				if (ImGui::MenuItem("Create Entity"))
					createEntity = true;
				ImGui::EndPopup();
			}
			ImGui::EndMenuBar();
		}

		int id = 0;
		m_Scene->m_Registry.each([&](auto entityID)
		{
			Entity entity{ entityID };
			EntityTreeNode(entity, id++);
		});

		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create Camera"))
				createCamera = true;

			if (ImGui::MenuItem("Create Entity"))
				createEntity = true;

			ImGui::EndPopup();
		}

		if (!ImGui::IsAnyItemHovered() && ImGui::IsAnyMouseDown() && ImGui::IsWindowHovered())
			m_SelectedEntity = {};

		if (createEntity)
			m_Scene->CreateEntity();

		if (createCamera)
		{
			Entity& entity = m_Scene->CreateEntity();
			entity.GetComponent<IdentityComponent>().Name = "Camera";
			entity.AddComponent<CameraComponent>();
			entity.RemoveComponent<SpriteComponent>();
		}

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
			UtilVectorColumn("Position", transform.Position);
			glm::vec3 rotation = glm::degrees(transform.Rotation);
			UtilVectorColumn("Rotation", rotation);
			transform.Rotation = glm::radians(rotation);
			UtilVectorColumn("Size", transform.Size, 1.0f);
			//Translation End

			ImGuiUtils::AddPadding(0.0f, 5.0f);
			ImGui::Separator();
			ImGuiUtils::AddPadding(0.0f, 5.0f);

			ImGuiUtils::TextCentered("Add Component", true);

			if (ImGui::Button("+", { 25.0f, 25.0f }))
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

			ImGuiUtils::AddPadding(0.0f, 5.0f);
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

		ShowComponent<SpriteComponent>("Sprite", [](SpriteComponent& component)
		{
			ImGuiUtils::AddPadding(0.0f, 5.0f);

			ImGui::ColorEdit4("Color", &component.Color[0]);

			ImGuiUtils::AddPadding(0.0f, 5.0f);

			float cellMultp = 3.0f;
			float cellSize = 25.0f;
			float cellPadding = 2.0f;

			ImGui::Text("Image:"); ImGui::SameLine();
			auto& imagePos = ImGui::GetCursorPos();
			ImGui::Image((ImTextureID)m_NoSpriteTexture->GetID(), { cellSize * cellMultp, cellSize * cellMultp });
			Ref<Texture> componentTexture = component.Texture;
			if (componentTexture)
			{
				ImGui::SetCursorPos(imagePos);
				ImGuiUtils::AddPadding(cellSize / cellPadding, cellSize / cellPadding);
				ImGuiUtils::Image((ImTextureID)componentTexture->GetID(), { cellSize * cellPadding, cellSize * cellPadding });
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MNE_AssetItem"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path("res/Assets") / path;
					Ref<Texture> texture = CreateRef<Texture>(texturePath.string());
					if (texture->IsValid())
						component.Texture = texture;
					else
						DebugErr(texturePath.string());
				}
				ImGui::EndDragDropTarget();
			}

			if (componentTexture)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.0f, 0.0f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });
				ImGui::SameLine();
				ImGui::SetCursorPos(imagePos);
				ImGuiUtils::AddPadding(2.0f * cellSize, 0.0f);
				if (ImGui::Button("X##DeleteButton", { cellSize, cellSize }))
				{
					componentTexture = nullptr;
					component.Texture = nullptr;
				}

				ImGuiUtils::AddPadding(0.0f, cellSize / cellPadding);
				ImGui::PopStyleColor(2);
			}
			ImGuiUtils::AddPadding(0.0f, 5.0f);
		});

		ShowComponent<CameraComponent>("Camera", [](CameraComponent& component)
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
			ImGui::DragFloat("##Distance", &component.Distance, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::Columns(1);
		});
		ImGui::PopStyleColor(3);
	}

	template<typename T, typename Function>
	void HierarchyView::ShowComponent(std::string componentName, Function function)
	{
		if (m_SelectedEntity.HasComponent<T>())
		{
			ImGui::PushID(componentName.c_str());
			float buttonSize = 25.0f;
			ImVec2 contentRegion = ImGui::GetContentRegionAvail();
			T& component = m_SelectedEntity.GetComponent<T>();

			ImGui::AlignTextToFramePadding();
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed;
			bool treeopen = ImGui::TreeNodeEx(componentName.c_str(), flags);
			ImGui::SameLine(contentRegion.x - (buttonSize / 2.0f) - ImGui::GetStyle().FramePadding.x);
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
			ImGui::PopID();
		}
	}

	void HierarchyView::UtilVectorColumn(const std::string& vecName, glm::vec3& vector, float resetValue, float columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::PushID(vecName.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGuiUtils::TextCentered(vecName.c_str(), false);
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
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

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.2f, 0.75f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.0f, 0.2f, 0.85f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.0f, 0.2f, 1.0f, 1.0f });

		if (ImGui::Button(" Z "))
			vector.z = resetValue;

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &vector.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::PopStyleColor(3);

		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
	}
}