#include "Editor/ImGuiUtils.h"
#include "Editor/Views/HierarchyView.h"

#include "Utils/IconsFontAwesome.h"

#include <imgui.cpp>
#include <misc/cpp/imgui_stdlib.h>
#include <misc/cpp/imgui_stdlib.cpp>

namespace MoonEngine
{
	Ref<Texture> m_NoSpriteTexture;

	template<typename T>
	struct ComponentCopier
	{
		inline static T Component;
		static void ApplyCopy(Entity& copyTo)
		{
			if (copyTo.HasComponent<T>())
				copyTo.GetComponent<T>() = Component;
		}
	};

	HierarchyView::HierarchyView()
	{
		m_NoSpriteTexture = CreateRef<Texture>("res/EditorIcons/Frame.png");
	}

	void HierarchyView::SetScene(Ref<Scene> scene)
	{
		m_Scene = scene;
		m_SelectedEntity = {};
	}

	void HierarchyView::MouseSelect()
	{
		bool selected = false;
		float x = Input::GetX();
		float y = Input::GetY();

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
					m_SelectedEntity = Entity{ entity, m_Scene.get() };
					break;
				}
			}
		}

		if (!selected)
			m_SelectedEntity = {};
	}

	void HierarchyView::DuplicateSelectedEntity()
	{
		if (m_SelectedEntity)
		{
			Entity e = m_Scene->DuplicateEntity(m_SelectedEntity);
			m_SelectedEntity = e;
		}
	}

	void HierarchyView::DeleteSelectedEntity()
	{
		if (m_SelectedEntity)
		{
			m_SelectedEntity.Destroy();
			m_SelectedEntity = {};
		}
	}

	void HierarchyView::CreateOptions()
	{
		if (ImGui::MenuItem("Camera"))
		{
			Entity& entity = m_Scene->CreateCameraEntity();
			m_SelectedEntity = entity;
		}

		if (ImGui::MenuItem("Entity"))
		{
			Entity entity = m_Scene->CreateEntity();
			m_SelectedEntity = entity;
		}

		ImGuiUtils::SeparatorDistanced(2.5f);

		if (ImGui::BeginMenu("Effects"))
		{
			if (ImGui::MenuItem("Particle"))
			{
				Entity& entity = m_Scene->CreateEntity();
				entity.RemoveComponent<SpriteComponent>();
				entity.GetComponent<IdentityComponent>().Name = "Particle";
				entity.AddComponent<ParticleComponent>();
				m_SelectedEntity = entity;
			}
			ImGui::End();
		}
	}

	void HierarchyView::BeginHierarchyView(bool& state)
	{
		float height = ImGui::GetFrameHeight();
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
		bool openPopup = false;

		if (ImGui::Begin(ICON_FK_LIST_UL "Hierarchy", &state, window_flags))
		{
			if (m_Scene)
			{
				if (ImGui::BeginMenuBar())
				{
					auto& contentRegion = ImGui::GetContentRegionAvail();
					float buttonSize = 25.0f;
					ImGuiUtils::AddPadding(contentRegion.x - (buttonSize / 2.0f) - (ImGui::GetStyle().FramePadding.x * 2.0f), 0);
					if (ImGui::Button(":", { buttonSize, buttonSize }))
						openPopup = true;
					ImGui::EndMenuBar();
				}

				std::vector<entt::entity> entites;
				m_Scene->m_Registry.each([&](auto entityID)
				{
					entites.push_back(entityID);
				});

				int id = 0;
				for (auto e = entites.end(); e != entites.begin();)
				{
					Entity entity{ (*--e), m_Scene.get() };
					EntityTreeNode(entity, id++);
				}

				ImGui::PushStyleColor(ImGuiCol_Separator, { 0.75f, 0.75f, 0.75f, 1.0f });

				if (openPopup)
					ImGui::OpenPopup("CreateStuff");

				if (ImGui::BeginPopup("CreateStuff"))
				{
					CreateOptions();
					ImGui::EndPopup();
				}

				if (ImGui::BeginPopupContextWindow(0, 1, false))
				{
					CreateOptions();
					ImGui::EndPopup();
				}
				ImGui::PopStyleColor();

				if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
					m_SelectedEntity = {};
			}
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

		if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1))
			m_SelectedEntity = entity;

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Duplicate Entity"))
			{
				Entity e = m_Scene->DuplicateEntity(m_SelectedEntity);
				m_SelectedEntity = e;
			}

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

	//------------------------------------INSPECTOR---------------------------------------------------------

	void HierarchyView::BeginInspectorView(bool& state)
	{
		ImGui::Begin(ICON_FK_SEARCH "Inspector", &state);

		if (m_SelectedEntity)
		{
			ImGuiUtils::Label("Name", true);
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().FramePadding.x);
			ImGui::InputText("##Name", &m_SelectedEntity.GetComponent<IdentityComponent>().Name);

			ImGuiUtils::AddPadding(0, 5.0f);

			float color = 75.0f / 255.0f;
			float alpha = 150.0f / 255.0f;
			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(color, color, color, alpha));
			alpha = 175.0f / 255.0f;
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(color, color, color, alpha));
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(color, color, color, alpha));

			//+Transform Component
			ImGui::PushID("Transform");
			float buttonSize = 25.0f;
			ImVec2 contentRegion = ImGui::GetContentRegionAvail();
			TransformComponent& component = m_SelectedEntity.GetComponent<TransformComponent>();

			ImGui::AlignTextToFramePadding();
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed;
			bool treeopen = ImGui::TreeNodeEx("Transform", flags);
			ImGui::SameLine(contentRegion.x - (buttonSize / 2.0f) - ImGui::GetStyle().FramePadding.x);
			if (ImGui::Button(":", { buttonSize, ImGui::GetFrameHeight() }))
				ImGui::OpenPopup("SettingList");

			if (ImGui::BeginPopup("SettingList"))
			{
				if (ImGui::MenuItem("Copy"))
					ComponentCopier<TransformComponent>::Component = m_SelectedEntity.GetComponent<TransformComponent>();
				if (ImGui::MenuItem("Paste"))
					ComponentCopier<TransformComponent>::ApplyCopy(m_SelectedEntity);
				ImGui::EndPopup();
			}

			if (treeopen)
			{
				ImGuiUtils::AddPadding(0, 5.0f);
				TransformComponent& transform = m_SelectedEntity.GetComponent<TransformComponent>();
				UtilVectorColumn("Position", transform.Position);
				glm::vec3 rotation = glm::degrees(transform.Rotation);
				UtilVectorColumn("Rotation", rotation);
				transform.Rotation = glm::radians(rotation);
				UtilVectorColumn("Size", transform.Size, 1.0f);
				ImGui::TreePop();

				ImGuiUtils::AddPadding(0.0f, 5.0f);
				ImGui::Separator();
			}
			ImGui::PopID();
			//-Transform Component

			ImGuiUtils::AddPadding(0.0f, 5.0f);

			ImGuiUtils::Label("Add Component", true);

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

				if (ImGui::MenuItem("Particle"))
					if (!m_SelectedEntity.HasComponent<ParticleComponent>())
						m_SelectedEntity.AddComponent<ParticleComponent>();

				ImGui::EndPopup();
			}

			ImGuiUtils::AddPadding(0.0f, 5.0f);
			ComponentView();
		}
		ImGui::End();
	}

	void HierarchyView::ComponentView()
	{
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
					std::filesystem::path texturePath = path;
					Ref<Texture> texture = ResourceManager::LoadTexture(texturePath.string());
					if (texture)
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
			ImGuiUtils::Label("Is Main", false);
			ImGui::NextColumn();
			ImGui::Checkbox("##IsMain", &component.IsMain);
			ImGui::Columns(1);

			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, winWidth);
			ImGuiUtils::Label("Distance", true);
			ImGui::NextColumn();
			ImGui::DragFloat("##Distance", &component.Distance, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::Columns(1);
		});

		ShowComponent<ParticleComponent>("Particle", [](ParticleComponent& component)
		{
			float columnWidth = 100.0f;
			float columnSpacing = 10.0f;

			ImGui::Columns(2, 0, false);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGuiUtils::Label("Play Options", false);
			ImGui::NextColumn();
			if (ImGui::Button("Play"))
				component.Play = true;
			ImGui::SameLine();
			ImGuiUtils::Label("Auto Play", true);
			ImGui::Checkbox("##AutoPlay", &component.AutoPlay);
			ImGui::Columns(1);

			ImGui::Columns(2, 0, false);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGuiUtils::Label("Duration", false);
			ImGui::NextColumn();
			ImGui::DragFloat("##Duration", &component.Duration, 0.1f);
			ImGui::Columns(1);

			ImGuiUtils::AddPadding(0.0f, columnSpacing);

			ImGui::Columns(2, 0, false);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGuiUtils::Label("Max Particles", false);
			ImGui::NextColumn();
			ImGui::DragInt("##MaxParts", &component.MaxParticles);
			ImGui::SameLine();
			if (component.MaxParticles != component.PoolSize())
				if (ImGui::Button("Apply"))
					component.Resize();
			ImGui::Columns(1);

			ImGui::Columns(2, 0, false);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGuiUtils::Label("Burst Mode", false);
			ImGui::NextColumn();
			ImGui::Checkbox("##Burst", &component.BurstMode);
			ImGui::Columns(1);

			ImGui::Columns(2, 0, false);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGuiUtils::Label("Rate", false);
			ImGui::NextColumn();
			ImGui::DragInt("##Rate", &component.Rate, 0.1f);
			ImGui::Columns(1);

			ImGuiUtils::AddPadding(0.0f, columnSpacing);

			ImGui::Columns(2, 0, false);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGuiUtils::Label("Lifetime", false);
			ImGui::NextColumn();
			ImGui::DragFloat("##Lifetime", &component.Lifetime, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::Columns(1);

			//Speed
			ImGuiUtils::AddPadding(0.0f, columnSpacing);

			ImGui::Columns(2, 0, false);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGuiUtils::Label("Direction", false);
			ImGui::NextColumn();
			ImGui::DragFloat3("##Direction", &component.Direction[0], 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::Columns(1);

			ImGui::Columns(2, 0, false);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGuiUtils::Label("Direction Velocity", false);
			ImGui::NextColumn();
			ImGui::DragFloat3("##DirectionVelocity", &component.DirectionVelocity[0], 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::Columns(1);

			ImGui::Columns(2, 0, false);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGuiUtils::Label("Shape Size", false);
			ImGui::NextColumn();
			ImGui::DragFloat3("##ShapeSize", &component.ShapeSize[0], 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::Columns(1);

			//Size
			ImGuiUtils::AddPadding(0.0f, columnSpacing);

			ImGui::Columns(2, 0, false);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGuiUtils::Label("Start Size", false);
			ImGui::NextColumn();
			ImGui::DragFloat3("##StartSize", &component.SizeStart[0], 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::Columns(1);

			ImGui::Columns(2, 0, false);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGuiUtils::Label("End Size", false);
			ImGui::NextColumn();
			ImGui::DragFloat3("##EndSize", &component.SizeEnd[0], 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::Columns(1);

			ImGui::Columns(2, 0, false);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGuiUtils::Label("Size Change Speed", false);
			ImGui::NextColumn();
			ImGui::DragFloat("##SizeChangeSpeed", &component.SizeChangeSpeed, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::Columns(1);

			//Color
			ImGuiUtils::AddPadding(0.0f, columnSpacing);

			ImGui::Columns(2, 0, false);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGuiUtils::Label("Start Color", false);
			ImGui::NextColumn();
			ImGui::ColorEdit4("##StartColor", &component.ColorStart[0]);
			ImGui::Columns(1);

			ImGui::Columns(2, 0, false);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGuiUtils::Label("End Color", false);
			ImGui::NextColumn();
			ImGui::ColorEdit4("##EndColor", &component.ColorEnd[0]);
			ImGui::Columns(1);

			ImGui::Columns(2, 0, false);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGuiUtils::Label("Color Change Speed", false);
			ImGui::NextColumn();
			ImGui::DragFloat("##ColorChangeSpeed", &component.ColorChangeSpeed, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::Columns(1);

			//Texture
			float cellMultp = 3.0f;
			float cellSize = 25.0f;
			float cellPadding = 2.0f;

			ImGui::Columns(2, 0, false);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGuiUtils::Label("Texture", false);
			ImGui::NextColumn();
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
					std::filesystem::path texturePath = path;
					Ref<Texture> texture = ResourceManager::LoadTexture(texturePath.string());
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
			ImGui::Columns(1);

			//Physics
			ImGui::Columns(2, 0, false);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGuiUtils::Label("Use Physics", false);
			ImGui::NextColumn();
			ImGui::Checkbox("##UsePhysics", &component.UsePhysics);
			ImGui::Columns(1);

			if (component.UsePhysics)
			{
				ImGui::Columns(2, 0, false);
				ImGui::SetColumnWidth(0, columnWidth);
				ImGuiUtils::Label("Gravity", false);
				ImGui::NextColumn();
				ImGui::DragFloat("##Gravity", &component.Gravity, 0.1f, 0.0f, 0.0f, "%.2f");
				ImGui::Columns(1);

				ImGui::Columns(2, 0, false);
				ImGui::SetColumnWidth(0, columnWidth);
				ImGuiUtils::Label("Mass", false);
				ImGui::NextColumn();
				ImGui::DragFloat("##Mass", &component.Mass, 0.1f, 0.0f, 0.0f, "%.2f");
				ImGui::Columns(1);
			}

		});
		ImGui::PopStyleColor(3);
	}

	//Tools
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
				if (ImGui::MenuItem("Copy"))
				{
					ComponentCopier<T>::Component = m_SelectedEntity.GetComponent<T>();
				}

				if (ImGui::MenuItem("Paste"))
				{
					ComponentCopier<T>::ApplyCopy(m_SelectedEntity);
				}

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
				ImGuiUtils::AddPadding(0.0f, 5.0f);
			}
			ImGui::PopID();
		}
	}

	void HierarchyView::UtilVectorColumn(const std::string& vecName, glm::vec3& vector, float resetValue, float columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::PushID(vecName.c_str());
		ImGui::Columns(2, 0, false);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGuiUtils::Label(vecName.c_str(), false);
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