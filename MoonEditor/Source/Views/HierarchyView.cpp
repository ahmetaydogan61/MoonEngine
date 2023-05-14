#include "mpch.h"
#include "Views/HierarchyView.h"
#include "Editor/EditorLayer.h"

#include <Engine/Components.h>
#include <Engine/Entity.h>
#include <Gui/ImGuiUtils.h>

#include <IconsMaterialDesign.h>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <misc/cpp/imgui_stdlib.cpp>

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

	HierarchyView::HierarchyView()
	{
		Name = ICON_MD_SORT;
		Name += "Hierarchy";
		Flags = ImGuiWindowFlags_MenuBar;
	}

	void HierarchyView::EntityCreationMenu()
	{
		EditorLayer& editor = *EditorLayer::Get();

		if (ImGui::MenuItem("Entity"))
		{
			Entity entity = Scene->CreateEntity();
			editor.SetSelectedEntity(entity);
		}

		ImGuiUtils::AddPadding(0.0f, 5.0f);

		if (ImGui::BeginMenu("Sprites"))
		{
			if (ImGui::MenuItem("Square"))
			{
				Entity entity = Scene->CreateEntity();
				entity.GetComponent<IdentityComponent>().Name = "Square";
				entity.AddComponent<SpriteComponent>();
				editor.SetSelectedEntity(entity);
			}
			ImGui::EndMenu();
		}

		ImGuiUtils::AddPadding(0.0f, 5.0f);

		if (ImGui::BeginMenu("Phyics"))
		{
			if (ImGui::MenuItem("Dynamic Square"))
			{
				Entity entity = Scene->CreateEntity();
				entity.GetComponent<IdentityComponent>().Name = "Dynamic Square";
				entity.AddComponent<SpriteComponent>();
				auto& pb = entity.AddComponent<PhysicsBodyComponent>();
				pb.Type = PhysicsBodyComponent::BodyType::Dynamic;
				editor.SetSelectedEntity(entity);
			}

			if (ImGui::MenuItem("Static Square"))
			{
				Entity entity = Scene->CreateEntity();
				entity.GetComponent<IdentityComponent>().Name = "Static Square";
				entity.AddComponent<SpriteComponent>();
				auto& pb = entity.AddComponent<PhysicsBodyComponent>();
				pb.Type = PhysicsBodyComponent::BodyType::Static;
				editor.SetSelectedEntity(entity);
			}
			ImGui::EndMenu();
		}

		ImGuiUtils::AddPadding(0.0f, 5.0f);

		if (ImGui::BeginMenu("Effects"))
		{
			if (ImGui::MenuItem("Particle"))
			{
				Entity entity = Scene->CreateEntity();
				entity.GetComponent<IdentityComponent>().Name = "Particle";
				entity.AddComponent<ParticleComponent>().ParticleSystem.Play();
				editor.SetSelectedEntity(entity);
			}
			ImGui::EndMenu();
		}

		ImGuiUtils::AddPadding(0.0f, 5.0f);

		if (ImGui::MenuItem("Camera"))
		{
			Entity entity = Scene->CreateEntity();
			entity.GetComponent<IdentityComponent>().Name = "Camera";
			entity.AddComponent<CameraComponent>();
			editor.SetSelectedEntity(entity);
		}
	}

	void HierarchyView::Render()
	{
		if (!Enabled)
			return;

		float height = ImGui::GetFrameHeight();
		bool openPopup = false;

		ImGui::Begin(Name.c_str(), &Enabled, Flags);

		if (ImGui::BeginMenuBar())
		{
			std::string sceneName = Scene->SceneName;
			ImGuiUtils::Label("Active Scene:");
			if (ImGui::InputText("##scnn", &sceneName, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if(sceneName != "")
					Scene->SceneName = sceneName;
			}

			const auto& contentRegion = ImGui::GetContentRegionAvail();
			float buttonSize = ImGui::GetFontSize() + 5.0f;
			ImGuiUtils::AddPadding(contentRegion.x - (buttonSize * 0.5f) - (ImGui::GetStyle().FramePadding.x * 2.0f), 0);
			if (ImGui::Button(":", { buttonSize, height }))
				openPopup = true;
			ImGui::EndMenuBar();
		}

		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, { 0.0f, 0.0f, 0.0f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_PopupBg, { 1.0f, 1.0f, 1.0f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Header, { 0.75f, 0.75f, 0.75f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_HeaderHovered, { 0.5f, 0.5f, 0.5f, 1.0f });

		if (openPopup)
			ImGui::OpenPopup("CreateStuff");

		if (ImGui::BeginPopup("CreateStuff"))
		{
			EntityCreationMenu();
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupContextWindow(0, 1))
		{
			EntityCreationMenu();
			ImGui::EndPopup();
		}

		ImGui::PopStyleColor(4);

		int id = 0;
		GetRegistry().each_reverse([&](auto entityID)
		{
			Entity entity{ entityID, Scene };
			EntityTreeNode(entity, id++);
		});

		if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			EditorLayer::Get()->SetSelectedEntity({});
		ImGui::End();
	}
}

