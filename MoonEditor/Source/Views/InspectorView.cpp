#include "mpch.h"
#include "Views/InspectorView.h"
#include "Editor/EditorLayer.h"

#include <Engine/Components.h>
#include <Gui/ImGuiUtils.h>
#include <Scripting/ScriptEngine.h>

#include <IconsMaterialDesign.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <imgui_internal.h>

namespace MoonEngine
{
	void ImGuiFieldConverter(ScriptInstance& instance, const ScriptField& field)
	{
		switch (field.Type)
		{
			case ScriptFieldType::Float:
			{
				float data = 0.0f;
				instance.GetFieldValue(field, &data);

				if (ImGui::DragFloat(field.FieldName.c_str(), &data))
					instance.SetFieldValue(field, &data);

				break;
			}
			case ScriptFieldType::Entity:
			{
				ImGuiUtils::Label(field.FieldName.c_str());

				bool reset = false;
				if (ImGui::Button(" X "))
					reset = true;

				ImGui::SameLine();

				uint64_t data = 0;

				instance.GetEntityReference(field, &data);

				ImGui::Text("%llu", data);

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ME_Entity"))
					{
						uint64_t id = *(const uint64_t*)payload->Data;
						instance.SetEntityReference(field, &id);
					}

					ImGui::EndDragDropTarget();
				}

				if (reset)
					instance.SetEntityReference(field, 0);
				break;
			}
		}
	}

	void ImGuiInstanceFieldConverter(ScriptInstance& instance, ScriptField& field)
	{
		switch (field.Type)
		{
			case ScriptFieldType::Float:
			{
				ImGui::DragFloat(field.FieldName.c_str(), &*(float*)field.Data);
				break;
			}
			case ScriptFieldType::Entity:
			{
				ImGuiUtils::Label(field.FieldName.c_str());

				bool reset = false;
				if (ImGui::Button(" X "))
					reset = true;

				ImGui::SameLine();

				ImGui::Text("%llu", *(uint64_t*)field.Data);

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ME_Entity"))
					{
						memcpy(field.Data, payload->Data, sizeof(uint64_t));
					}

					ImGui::EndDragDropTarget();
				}

				if (reset)
					memset(field.Data, 0, sizeof(uint64_t));
				break;
			}
		}
	}

	InspectorView::InspectorView()
	{
		Name = ICON_MD_MANAGE_SEARCH;
		Name += "Inspector";
	}

	void BeginDrawProp(const char* label)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_CellPadding, { 8.0f, 2.0f });
		ImGui::BeginTable(label, 2, ImGuiTableFlags_::ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable);
	}

	void EndDrawProp()
	{
		ImGui::EndTable();
		ImGui::PopStyleVar();
	}

	void RenderProp(const char* label, std::function<void()> func)
	{
		ImGui::PushID(label);
		ImGui::TableNextColumn();
		ImGuiUtils::Label(label, false);
		ImGuiUtils::AddPadding(0.0f, 5.0f);
		ImGui::TableNextColumn();
		func();
		ImGui::PopID();
	}

	void DrawTreeProp(const char* label, std::function<void()> func)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanFullWidth;
		bool treeopen = ImGui::TreeNodeEx(label, flags);

		if (treeopen)
		{
			ImGuiUtils::AddPadding(0.0f, 10.0f);
			BeginDrawProp(label);
			func();
			EndDrawProp();
			ImGuiUtils::AddPadding(0.0f, 20.0f);
			ImGui::TreePop();
		}
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
			UtilVectorColumn("Size", component.Scale, 1.0f);
			glm::vec3 rotation = glm::degrees(component.Rotation);
			UtilVectorColumn("Rotation", rotation);
			component.Rotation = glm::radians(rotation);

			ImGuiUtils::AddPadding(0.0f, 10.0f);
			ImGui::Separator();
			ImGui::TreePop();
		}
		ImGui::PopID();
	}

	template<typename T>
	void ShowComponent(const std::string& componentName, std::function<void(T&)> function, Entity selectedEntity)
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
		float dragSliderSpeed = 0.1f;

		ShowComponent<CameraComponent>("Camera", [&](CameraComponent& component)
		{
			BeginDrawProp("##Camera");

			RenderProp("Is Main", [&]
			{
				ImGui::Checkbox("##IsMain", &component.IsMain);
			});

			RenderProp("Size", [&]
			{
				ImGui::DragFloat("##Size", &component.Size, dragSliderSpeed, 0.0f, FLT_MAX, "%.2f");
			});

			EndDrawProp();
		}, selectedEntity);

		ShowComponent<SpriteComponent>("Sprite", [&](SpriteComponent& component)
		{
			BeginDrawProp("##Sprite");

			RenderProp("Color", [&]
			{
				ImGui::ColorEdit4("##Color", &component.Color[0]);
			});

			RenderProp("Tiling {x:y}", [&]
			{
				ImGui::DragFloat2("##Tiling", &component.Tiling[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f");
			});

			RenderProp("Layer", [&]
			{
				ImGui::DragInt("##Layer", &component.Layer, dragSliderSpeed, 0, Renderer::GetStats().MaxLayers - 1, "%d", ImGuiSliderFlags_AlwaysClamp);

			});

			bool hasSpriteSheet = component.GetTextureSheet() != nullptr;
			RenderProp("Texture", [&]
			{
				if (component.GetTexture())
				{
					bool reset = false;
					if (ImGui::Button(" X "))
						reset = true;

					ImGui::SameLine();
					ImGui::Text(component.GetTexture()->GetPath().filename().string().c_str());
					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip(component.GetTexture()->GetPath().string().c_str());
					}
					if (reset)
						component.DeleteTexture();
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
						{
							component.SetTexture(texture);
							if (component.GetTextureSheet())
								component.GenerateSpriteSheet();
						}
						else
							ME_ERR("Failed to load Texture! Path: {}", texturePath.string().c_str());
					}
					ImGui::EndDragDropTarget();
				}

				ImGuiUtils::AddPadding(0.0f, 2.5f);

				if (ImGui::Button(hasSpriteSheet ? "Delete Sprite Sheet" : "Generate Sprite Sheet"))
					hasSpriteSheet ? component.DeleteSpriteSheet() : component.GenerateSpriteSheet();
			});

			EndDrawProp();

			if (!hasSpriteSheet)
				return;

			DrawTreeProp("Sprite Sheet", [&]
			{
				RenderProp("Size", [&]
				{
					if (ImGui::DragFloat2("##siz", &component.SpriteSize[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f"))
						component.GetTextureSheet()->Resize(component.SpriteSize);
				});

				RenderProp("Coordinates", [&]
				{
					if (ImGui::DragFloat2("##coords", &component.SpriteCoords[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f"))
						component.GetTextureSheet()->SetCoordinate(component.SpriteCoords);
				});
			});


		}, selectedEntity);

		ShowComponent<ScriptComponent>("Script", [&](ScriptComponent& component)
		{
			BeginDrawProp("##Script");

			RenderProp("Class", [&]
			{
				bool isEditorPlaying = EditorLayer::State() != EditorLayer::EditorState::Edit;

				std::vector<std::string> classes;

				for (const auto& [name, scriptClass] : ScriptEngine::GetScriptClasses())
					classes.push_back(name);

				const char* currentItem = component.ClassName.c_str();
				if (ImGui::BeginCombo("##Type", currentItem, ImGuiComboFlags_::ImGuiComboFlags_NoArrowButton))
				{
					for (int n = 0; n < classes.size(); n++)
					{
						bool is_selected = (currentItem == classes[n]);

						if (ImGui::Selectable(classes[n].c_str(), is_selected))
						{
							component.ClassName = classes[n];
							ScriptEngine::CreateEntityInstance(selectedEntity, component.ClassName);
							if (isEditorPlaying)
								ScriptEngine::AwakeEntity(selectedEntity, component.ClassName);
						}

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				if (ScriptEngine::CheckScriptClass(component.ClassName))
				{
					Shared<ScriptInstance> scriptInstance = ScriptEngine::GetScriptInstance(selectedEntity.GetUUID());
					if (scriptInstance)
					{
						if (isEditorPlaying)
						{
							const auto& fields = scriptInstance->GetScriptClass()->GetFields();
							for (const auto& [name, field] : fields)
								ImGuiFieldConverter(*scriptInstance, field);
						}
						else
						{
							auto& fields = scriptInstance->GetInstanceFields();
							for (auto& [name, field] : fields)
								ImGuiInstanceFieldConverter(*scriptInstance, field);
						}
					}
				}
			});

			EndDrawProp();
		}, selectedEntity);

		ShowComponent<PhysicsBodyComponent>("Physics Body", [&](PhysicsBodyComponent& component)
		{
			BeginDrawProp("##pb");
			RenderProp("Body", [&]
			{
				ImGuiUtils::AddPadding(0.0f, 5.0f);
			});

			RenderProp("Type", [&]
			{
				int currentType = (int)component.Type;
				if (ImGui::Combo("##Type", &currentType, "Static\0Dynamic\0Kinematic\0"))
					component.Type = (PhysicsBodyComponent::BodyType)currentType;
			});

			RenderProp("Freeze Rotation", [&]
			{
				ImGui::Checkbox("##frot", &component.FreezeRotation);
			});

			RenderProp("Continuous Detection", [&]
			{
				ImGui::Checkbox("##ccd", &component.IsContinuousDetection);
			});

			RenderProp("Is Trigger", [&]
			{
				ImGui::Checkbox("##iTrig", &component.IsTrigger);
			});

			RenderProp("Linear Damping", [&]
			{
				ImGui::DragFloat("##ldamp", &component.LinearDamping, dragSliderSpeed * 0.1f, 0.0f, FLT_MAX, "%.2f");
			});

			RenderProp("Angular Damping", [&]
			{
				ImGui::DragFloat("##adamp", &component.AngularDamping, dragSliderSpeed * 0.1f, 0.0f, FLT_MAX, "%.2f");
			});

			RenderProp("Gravity Scale", [&]
			{
				ImGui::DragFloat("##adamp", &component.GravityScale, dragSliderSpeed, 0.0f, 0.0f, "%.2f");
			});

			RenderProp("Fixture", [&]
			{
				ImGuiUtils::AddPadding(0.0f, 5.0f);
			});

			RenderProp("Offset", [&]
			{
				ImGui::DragFloat2("##offset", &component.Offset[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f");
			});

			RenderProp("Size", [&]
			{
				ImGui::DragFloat2("##siz", &component.Size[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f");
			});

			RenderProp("Density", [&]
			{
				ImGui::DragFloat("##density", &component.Density, dragSliderSpeed * 0.1f, 0.0f, 1.0f, "%.2f");
			});

			RenderProp("Friction", [&]
			{
				ImGui::DragFloat("##friction", &component.Friction, dragSliderSpeed * 0.1f, 0.0f, 1.0f, "%.2f");
			});

			RenderProp("Restitution", [&]
			{
				ImGui::DragFloat("##rt", &component.Restitution, dragSliderSpeed * 0.1f, 0.01f, 1.0f, "%.2f");
			});

			RenderProp("Restitution Threshold", [&]
			{
				ImGui::DragFloat("##rtth", &component.RestitutionThreshold, dragSliderSpeed * 0.1f, 0.01f, 1.0f, "%.2f");
			});

			EndDrawProp();
		}, selectedEntity);

		ShowComponent<ParticleComponent>("Particle System", [&](ParticleComponent& component)
		{
			bool isPlaying = component.ParticleSystem.IsPlaying();
			const char* label = isPlaying ? "Pause" : "Play";
			if (ImGuiUtils::ButtonSelectable(label, 0, 0, isPlaying, ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_Button]))
				isPlaying ? component.ParticleSystem.Pause() : component.ParticleSystem.Play();

			ImGui::SameLine();

			if (ImGuiUtils::ButtonSelectable("Stop", 0, 0, false, ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_Button]))
				component.ParticleSystem.Stop();

			ImGui::SameLine();

			if (ImGuiUtils::ButtonSelectable("Restart", 0, 0, false, ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_Button]))
			{
				component.ParticleSystem.Stop();
				component.ParticleSystem.Play();
			}

			ImGuiUtils::AddPadding(0.0f, 10.0f);

			DrawTreeProp("System", [&]
			{
				RenderProp("Pool Size", [&]
				{
					int32_t size = (int32_t)component.ParticleSystem.Size();
					if (ImGui::DragInt("##psiz", &size, 0.1f, 0, FLT_MAX, "%d", ImGuiInputTextFlags_EnterReturnsTrue | ImGuiSliderFlags_AlwaysClamp))
						component.ParticleSystem.Resize(size);
				});

				RenderProp("Play On Awake", [&]
				{
					ImGui::Checkbox("##POA", &component.ParticleSystem.PlayOnAwake);
				});

				RenderProp("Looping", [&]
				{
					ImGui::Checkbox("##Loop", &component.ParticleSystem.Looping);
				});

				ImGuiUtils::AddPadding(0.0f, 5.0f);

				RenderProp("Duration", [&]
				{
					ImGui::DragFloat("##Dur", &component.ParticleSystem.Duration, dragSliderSpeed, 0.0f, 0.0f, "%.2f");
				});

				ImGuiUtils::AddPadding(0.0f, 5.0f);

				RenderProp("Particles Per Second", [&]
				{
					ImGui::DragFloat("##PPS", &component.ParticleSystem.ParticlePerSecond, dragSliderSpeed, 0.0f, 0.0f, "%.2f");
				});

				RenderProp("Particles Per Unit", [&]
				{
					ImGui::DragFloat("##PPU", &component.ParticleSystem.ParticlePerUnit, dragSliderSpeed, 0.0f, 0.0f, "%.2f");
				});

				ImGuiUtils::AddPadding(0.0f, 5.0f);

				RenderProp("Lifetime", [&]
				{
					ImGui::Checkbox("Constant", &component.Particle.IsLifetimeConstant);

					if (component.Particle.IsLifetimeConstant)
						ImGui::DragFloat("##Lt", &component.Particle.Lifetime[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f");
					else
						ImGui::DragFloat2("##Lt", &component.Particle.Lifetime[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f");
				});
			});

			DrawTreeProp("Emitter", [&]
			{
				RenderProp("Speed", [&]
				{
					ImGui::Checkbox("Constant", &component.Particle.IsSpeedConstant);

					if (component.Particle.IsSpeedConstant)
						ImGui::DragFloat("##Spd", &component.Particle.Speed[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f");
					else
						ImGui::DragFloat2("##Spd", &component.Particle.Speed[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f");
				});

				ImGuiUtils::AddPadding(0.0f, 5.0f);

				RenderProp("Emitter Type", [&]
				{
					int currentStlye = (int)component.ParticleSystem.EmitterType;
					if (ImGui::Combo("##Type", &currentStlye, "None\0Box\0Cone\0"))
						component.ParticleSystem.EmitterType = (EmitterType)currentStlye;
				});

				ImGuiUtils::AddPadding(0.0f, 5.0f);

				EmitterType emitterType = component.ParticleSystem.EmitterType;

				if (emitterType == EmitterType::Cone)
				{
					RenderProp("Radius", [&]()
					{
						ImGui::DragFloat("##Rad", &component.Particle.DirectionRadiusFactor, dragSliderSpeed, 0.0f, 0.0f, "%.2f");
					});
				}

				ImGuiUtils::AddPadding(0.0f, 5.0f);

				if (emitterType != EmitterType::None)
				{
					RenderProp("Spawn Position", [&]()
					{
						ImGui::DragFloat3("##Rad", &component.Particle.SpawnPosition[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f");
					});

					RenderProp("Spawn Radius", [&]()
					{
						ImGui::DragFloat3("##Rad", &component.Particle.SpawnRadius[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f");
					});

					RenderProp("Randomize Direction", [&]()
					{
						ImGui::DragFloat("##Dir", &component.Particle.RandomDirectionFactor, 0.01f, 0.0f, 1.0f, "%.2f");
						component.Particle.RandomDirectionFactor = glm::clamp(component.Particle.RandomDirectionFactor, 0.0f, 1.0f);
					});
				}
			});

			DrawTreeProp("Scale", [&]
			{
				RenderProp("##Scale", [&]
				{
					ImGui::Checkbox("Is3D", &component.Particle.IsScale3D);
				});

				ImGuiUtils::AddPadding(0.0f, 5.0f);

				RenderProp("Start", [&]
				{
					ImGui::Checkbox("Constant", &component.Particle.IsScaleConstant);

					if (component.Particle.IsScale3D)
					{
						ImGui::DragFloat3("##Scale", &component.Particle.ScaleStart[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f");
						if (!component.Particle.IsScaleConstant)
							ImGui::DragFloat3("##Range", &component.Particle.ScaleStartRandom[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f");
					}
					else
					{
						ImGui::DragFloat("##Scale", &component.Particle.ScaleStart[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f");
						if (!component.Particle.IsScaleConstant)
							ImGui::DragFloat("##Range", &component.Particle.ScaleStartRandom[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f");
					}
				});

				ImGuiUtils::AddPadding(0.0f, 5.0f);

				RenderProp("End", [&]
				{
					ImGui::Checkbox("Enabled", &component.Particle.IsScaleCycle);

					if (!component.Particle.IsScaleCycle)
						return;

					ImGui::Checkbox("Constant", &component.Particle.IsScaleEndConstant);

					if (component.Particle.IsScale3D)
					{
						ImGui::DragFloat3("##Scale", &component.Particle.ScaleEnd[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f");
						if (!component.Particle.IsScaleEndConstant)
							ImGui::DragFloat3("##Range", &component.Particle.ScaleEndRandom[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f");
					}
					else
					{
						ImGui::DragFloat("##Scale", &component.Particle.ScaleEnd[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f");
						if (!component.Particle.IsScaleEndConstant)
							ImGui::DragFloat("##Range", &component.Particle.ScaleEndRandom[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f");
					}
				});
			});

			DrawTreeProp("Rotation", [&]
			{
				RenderProp("##Rot", [&]
				{
					ImGui::Checkbox("Is3D", &component.Particle.IsRotation3D);
				});

				ImGuiUtils::AddPadding(0.0f, 5.0f);

				RenderProp("Start", [&]
				{
					ImGui::Checkbox("Constant", &component.Particle.IsRotationConstant);

					glm::vec3 rotation = glm::degrees(component.Particle.RotationStart);
					glm::vec3 rotationRandom = glm::degrees(component.Particle.RotationStartRandom);
					if (component.Particle.IsRotation3D)
					{
						ImGui::DragFloat3("##RotStart", &rotation[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f");
						if (!component.Particle.IsRotationConstant)
							ImGui::DragFloat3("##RotRandom", &rotationRandom[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f");
					}
					else
					{
						ImGui::DragFloat("##RotStart", &rotation.z, dragSliderSpeed, 0.0f, 0.0f, "%.2f");
						if (!component.Particle.IsRotationConstant)
							ImGui::DragFloat("##RotRandom", &rotationRandom.z, dragSliderSpeed, 0.0f, 0.0f, "%.2f");
					}
					component.Particle.RotationStart = glm::radians(rotation);
					component.Particle.RotationStartRandom = glm::radians(rotationRandom);
				});

				ImGuiUtils::AddPadding(0.0f, 5.0f);

				RenderProp("End", [&]
				{
					ImGui::Checkbox("Enabled", &component.Particle.IsRotationCycle);

					if (!component.Particle.IsRotationCycle)
						return;

					glm::vec3 rotation = glm::degrees(component.Particle.RotationEnd);
					glm::vec3 rotationRandom = glm::degrees(component.Particle.RotationEndRandom);

					ImGui::Checkbox("Constant", &component.Particle.IsRotationEndConstant);

					if (component.Particle.IsRotation3D)
					{
						ImGui::DragFloat3("##Rot", &rotation[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f");
						if (!component.Particle.IsRotationEndConstant)
							ImGui::DragFloat3("##Range", &rotationRandom[0], dragSliderSpeed, 0.0f, 0.0f, "%.2f");
					}
					else
					{
						ImGui::DragFloat("##Rot", &rotation.z, dragSliderSpeed, 0.0f, 0.0f, "%.2f");
						if (!component.Particle.IsRotationEndConstant)
							ImGui::DragFloat("##Range", &rotationRandom.z, dragSliderSpeed, 0.0f, 0.0f, "%.2f");
					}
					component.Particle.RotationEnd = glm::radians(rotation);
					component.Particle.RotationEndRandom = glm::radians(rotationRandom);
				});
			});

			DrawTreeProp("Rendering", [&]
			{
				RenderProp("Sort Mode", [&]
				{
					int currentMode = (int)component.ParticleSystem.SortMode;
					if (ImGui::Combo("##Type", &currentMode, "Oldest in Front\0Youngest in Front\0"))
						component.ParticleSystem.SortMode = (SortMode)currentMode;
				});

				ImGuiUtils::AddPadding(0.0f, 5.0f);

				RenderProp("Texture", [&]
				{
					if (component.Particle.Texture)
					{
						bool reset = false;
						if (ImGui::Button(" X "))
							reset = true;

						ImGui::SameLine();
						ImGui::Text(component.Particle.Texture->GetPath().filename().string().c_str());
						if (ImGui::IsItemHovered())
						{
							ImGui::SetTooltip(component.Particle.Texture->GetPath().string().c_str());
						}
						if (reset)
							component.Particle.Texture = nullptr;
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
								component.Particle.Texture = texture;
							else
								ME_ERR("Failed to load Texture! Path: {}", texturePath.string().c_str());
						}
						ImGui::EndDragDropTarget();
					}
				});

				ImGuiUtils::AddPadding(0.0f, 5.0f);

				RenderProp("Layer", [&]
				{
					ImGui::DragInt("##Layer", &component.ParticleSystem.Layer, dragSliderSpeed, 0, Renderer::GetStats().MaxLayers - 1, "%d", ImGuiSliderFlags_AlwaysClamp);
				});

				ImGuiUtils::AddPadding(0.0f, 5.0f);

				RenderProp("Color Start", [&]
				{
					ImGui::Checkbox("Constant", &component.Particle.IsColorConstant);

					ImGui::ColorEdit4("##RotStart", &component.Particle.ColorStart[0]);
					if (!component.Particle.IsColorConstant)
						ImGui::ColorEdit4("##RotRand", &component.Particle.ColorStartRandom[0]);
				});

				ImGuiUtils::AddPadding(0.0f, 5.0f);

				RenderProp("Color End", [&]
				{
					ImGui::Checkbox("Enabled", &component.Particle.IsColorCycle);

					if (!component.Particle.IsColorCycle)
						return;

					ImGui::Checkbox("Constant", &component.Particle.IsColorEndConstant);

					ImGui::ColorEdit4("##ColEnd", &component.Particle.ColorEnd[0]);
					if (!component.Particle.IsColorEndConstant)
						ImGui::ColorEdit4("##RColRand", &component.Particle.ColorEndRandom[0]);
				});
			});
		}, selectedEntity);
	}

	void InspectorView::Render()
	{
		if (!Enabled)
			return;

		Entity selectedEntity = EditorLayer::Get()->GetSelectedEntity();

		float color = 75.0f / 255.0f;
		float alpha = 150.0f / 255.0f;
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(color, color, color, alpha));
		alpha = 175.0f / 255.0f;
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(color, color, color, alpha));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(color, color, color, alpha));

		ImGui::Begin(Name.c_str(), &Enabled, Flags);

		if (selectedEntity)
		{
			ImGuiUtils::AddPadding(0.0f, 5.0f);

			ImGuiUtils::Label("Name", true);
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().FramePadding.x);
			ImGui::InputText("##Name", &selectedEntity.GetComponent<IdentityComponent>().Name);

			ImGuiUtils::Label("ID", true);
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().FramePadding.x);
			ImGui::Text("%llu", selectedEntity.GetUUID());

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

					if (ImGui::MenuItem("Script"))
						if (!selectedEntity.HasComponent<ScriptComponent>())
							selectedEntity.AddComponent<ScriptComponent>();

					if (ImGui::MenuItem("Camera"))
						if (!selectedEntity.HasComponent<CameraComponent>())
							selectedEntity.AddComponent<CameraComponent>();

					if (ImGui::MenuItem("Particle System"))
						if (!selectedEntity.HasComponent<ParticleComponent>())
							selectedEntity.AddComponent<ParticleComponent>();

					if (ImGui::MenuItem("Physics Body"))
						if (!selectedEntity.HasComponent<PhysicsBodyComponent>())
							selectedEntity.AddComponent<PhysicsBodyComponent>();

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