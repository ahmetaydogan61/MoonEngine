#include "mpch.h"
#include "Views/InspectorView.h"
#include "EditorLayer.h"

#include "Core/Debug.h"
#include "Engine/Components.h"
#include <Gui/ImGuiUtils.h>
#include <IconsMaterialDesign.h>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <misc/cpp/imgui_stdlib.cpp>
#include <imgui_internal.h>

namespace MoonEngine
{
	void BeginDrawProp(const char* label)
	{
		ImGui::BeginTable(label, 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings);
	}

	void EndDrawProp()
	{
		ImGui::EndTable();
	}

	void RenderProp(const char* label, std::function<void()> func)
	{
		ImGui::TableNextColumn();
		ImGuiUtils::Label(label, false);
		ImGui::TableNextColumn();
		func();
	}

	void UtilVectorColumn(const std::string& vecName, glm::vec3& vector, float resetValue = 0.0f, float columnWidth = 100.0f)
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

	void RenderTransformComponent(Entity selectedEntity)
	{
		ImGui::PushID("Transform");

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed;
		bool treeopen = ImGui::TreeNodeEx("Tranform", flags);

		if (treeopen)
		{
			ImGuiUtils::AddPadding(0.0f, 10.0f);

			TransformComponent& component = selectedEntity.GetComponent<TransformComponent>();
			glm::vec3& position = component.Position;
			UtilVectorColumn("Position", component.Position);
			glm::vec3 rotation = glm::degrees(component.Rotation);
			UtilVectorColumn("Rotation", rotation);
			component.Rotation = glm::radians(rotation);
			UtilVectorColumn("Size", component.Scale, 1.0f);

			ImGuiUtils::AddPadding(0.0f, 10.0f);
			ImGui::Separator();
			ImGui::TreePop();
		}
		ImGui::PopID();
	}

	template<typename T, typename Function>
	void ShowComponent(std::string componentName, Function function, Entity selectedEntity)
	{
		if (selectedEntity.HasComponent<T>())
		{
			ImGui::PushID(componentName.c_str());
			T& component = selectedEntity.GetComponent<T>();

			float fontSize = ImGui::GetFontSize();
			float buttonSize = (fontSize * 0.5f) + 25.0f;
			ImVec2 contentRegion = ImGui::GetContentRegionAvail();

			ImGui::AlignTextToFramePadding();

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed;
			bool treeopen = ImGui::TreeNodeEx(componentName.c_str(), flags);

			ImGui::SameLine(contentRegion.x - buttonSize + 12.5f - ImGui::GetStyle().FramePadding.x);

			if (ImGui::Button(":", { buttonSize, ImGui::GetFrameHeight() }))
				ImGui::OpenPopup("SettingList");

			if (ImGui::BeginPopup("SettingList"))
			{
				if (ImGui::MenuItem("Remove"))
					selectedEntity.RemoveComponent<T>();

				ImGui::EndPopup();
			}

			if (treeopen)
			{
				ImGuiUtils::AddPadding(0.0f, 10.0f);
				function(component);
				ImGuiUtils::AddPadding(0.0f, 20.0f);
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
	}

	void RenderComponents(Entity& selectedEntity)
	{
		ShowComponent<SpriteComponent>("Sprite", [&](SpriteComponent& component)
		{
			{
				BeginDrawProp("##Sprite");

				RenderProp("Color", [&]
				{
					ImGui::ColorEdit4("##Color", &component.Color[0]);
				});
				
				RenderProp("Tiling {x:y}", [&]
				{
					ImGui::DragFloat2("##Tiling", &component.Tiling[0], 0.1f, 0.0f, 0.0f, "%.2f");
				});
				
				RenderProp("Texture", [&]
				{
					if (component.Texture)
					{
						bool reset = false;
						if (ImGui::Button("X"))
							reset = true;

						ImGui::SameLine();
						ImGui::Text(component.Texture->GetPath().filename().string().c_str());
						if (ImGui::IsItemHovered())
						{
							ImGui::SetTooltip(component.Texture->GetPath().string().c_str());
						}
						if(reset)
							component.Texture = nullptr;
					}
					else
						ImGui::Text("None");

					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ME_AssetItem"))
						{
							const wchar_t* path = (const wchar_t*)payload->Data;
							std::filesystem::path texturePath = path;
							Shared<Texture> texture = MakeShared<Texture>(texturePath.string());
							if (texture)
								component.Texture = texture;
							else
								ME_ERR("Failed to load Texture! Path: {}", texturePath.string().c_str());
						}
						ImGui::EndDragDropTarget();
					}
				});
				EndDrawProp();
			}
		}, selectedEntity);

		ShowComponent<CameraComponent>("Camera", [&](CameraComponent& component)
		{
			{
				BeginDrawProp("##Camera");

				RenderProp("Is Main", [&]
				{
					ImGui::Checkbox("##IsMain", &component.IsMain);
				});

				RenderProp("Size", [&]
				{
					ImGui::DragFloat("##Size", &component.Size, 0.1f, 0.0f, FLT_MAX, "%.2f");
				});

				EndDrawProp();
			}
		}, selectedEntity);
	}

	void InspectorView::Render(bool& render)
	{
		if (!render)
			return;

		Entity selectedEntity = EditorLayer::Get()->GetSelectedEntity();

		float color = 75.0f / 255.0f;
		float alpha = 150.0f / 255.0f;
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(color, color, color, alpha));
		alpha = 175.0f / 255.0f;
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(color, color, color, alpha));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(color, color, color, alpha));

		ImGui::Begin(ICON_MD_MANAGE_SEARCH "Inspector", &render);

		if (selectedEntity)
		{
			ImGuiUtils::AddPadding(0.0f, 5.0f);

			ImGuiUtils::Label("Name", true);
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().FramePadding.x);
			ImGui::InputText("##Name", &selectedEntity.GetComponent<IdentityComponent>().Name);

			ImGuiUtils::AddPadding(0, 10.0f);

			RenderTransformComponent(selectedEntity);

			ImGuiUtils::AddPadding(0.0f, 10.0f);

			{//+Add Component Button
				ImGuiUtils::Label("Add Component", true);

				float buttonSize = ImGui::GetFontSize() * 1.5f;
				if (ImGui::Button("+", { buttonSize, buttonSize }))
					ImGui::OpenPopup("AddComponents");

				if (ImGui::BeginPopup("AddComponents"))
				{
					if (ImGui::MenuItem("Sprite"))
						if (!selectedEntity.HasComponent<SpriteComponent>())
							selectedEntity.AddComponent<SpriteComponent>();

					if (ImGui::MenuItem("Camera"))
						if (!selectedEntity.HasComponent<CameraComponent>())
							selectedEntity.AddComponent<CameraComponent>();
					ImGui::EndPopup();
				}
			}//-Add Component Button

			ImGuiUtils::AddPadding(0, 10.0f);

			RenderComponents(selectedEntity);
		}

		ImGui::End();
		ImGui::PopStyleColor(3);
	}
}