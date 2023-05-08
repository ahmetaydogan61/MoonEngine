#include "mpch.h"
#include "Views/AssetsView/AssetsView.h"
#include "Editor/EditorAssets.h"

#include <Renderer/Texture.h>
#include <Gui/ImGuiUtils.h>

#include <IconsMaterialDesign.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <imgui_internal.h>

namespace MoonEngine
{
	static bool FilterSearch(const std::string& fileName, const std::string& searchedFile)
	{
		auto it = std::search
		(
			fileName.begin(), fileName.end(),
			searchedFile.begin(), searchedFile.end(),
			[](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
		);
		return (it != fileName.end());
	}

	static void ContentItemPopup(const std::filesystem::directory_entry& entry)
	{
		bool itemPopup = false;

		const auto& path = entry.path();

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("ContentItemPopup");
			itemPopup = true;
		}

		if (ImGui::BeginPopup("ContentItemPopup"))
		{
			std::string explorerPath = "explorer ";

			std::string fileName = path.filename().string();
			ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_FrameBg, { 0.0f, 0.0f, 0.0f, 0.0f });

			if (ImGui::InputText("##Name", &fileName, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				std::filesystem::path newPath = path;
				newPath.replace_filename(fileName);
				std::filesystem::rename(path, newPath);
			}
			ImGui::PopStyleColor();
			ImGuiUtils::SeparatorDistanced(2.5f, 5.0f);

			if (entry.is_directory())
			{
				if (ImGui::MenuItem("Show Explorer"))
				{
					explorerPath += std::filesystem::canonical(path).parent_path().string();
					system(explorerPath.c_str());
				}

				ImGuiUtils::AddPadding(2.5f, 2.5f);

				if (ImGui::MenuItem("Delete Folder"))
				{
					std::filesystem::remove_all(path);
				}
			}
			else
			{
				if (ImGui::MenuItem("Show Explorer"))
				{
					explorerPath += std::filesystem::canonical(path.parent_path()).string();
					system(explorerPath.c_str());
				}

				ImGuiUtils::SeparatorDistanced(2.5f, 2.5f);

				if (ImGui::MenuItem("Delete File"))
				{
					std::filesystem::remove(path);
				}
			}
			ImGui::EndPopup();
		}
	}

	AssetsView::AssetsView(const std::filesystem::path& startPath)
	{
		Name = ICON_MD_TOKEN;
		Name += "Assets";

		m_FileIcon = MakeShared<Texture>("Resource/EditorIcons/File.png");
		m_FolderIcon = MakeShared<Texture>("Resource/EditorIcons/Folder.png");

		m_StartPath = startPath;
		m_CurrentPath = m_StartPath;
	}

	void AssetsView::ShowEntry(const std::filesystem::directory_entry& entry, float thumbnailSize)
	{
		ImGui::TableNextColumn();

		const auto& path = entry.path();
		const std::string& filenameString = path.filename().string();
		bool isDirectory = entry.is_directory();

		ImGui::PushID(filenameString.c_str());

		Texture* icon = isDirectory ? m_FolderIcon.get() : m_FileIcon.get();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGuiUtils::ImageButton((ImTextureID)icon->GetTextureId(), { thumbnailSize, thumbnailSize });
		ImGui::PopStyleColor();

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (isDirectory)
				m_CurrentPath /= path.filename();
		}

		if (!isDirectory && ImGui::BeginDragDropSource())
		{
			const auto& relative = std::filesystem::relative(path);
			const wchar_t* itemPath = relative.c_str();
			ImGui::SetDragDropPayload("ME_AssetItem", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
			ImGui::EndDragDropSource();
		}

		ContentItemPopup(entry);

		ImGui::BeginChild("##childFrame", { thumbnailSize, ImGui::GetFontSize() });

		float texSiz = thumbnailSize * 0.5f - ImGui::CalcTextSize(filenameString.c_str()).x * 0.5f;
		texSiz = std::max(0.0f, texSiz);

		ImGuiUtils::AddPadding(texSiz + 0.1f, 0.0f);

		ImGui::Text(filenameString.c_str());

		if (m_ShowNameTooltip && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
			ImGui::SetTooltip(filenameString.c_str());

		ImGui::EndChild();

		ImGui::PopID();
	}

	void AssetsView::Render()
	{
		if (!Enabled)
			return;

		ImGui::Begin(Name.c_str(), &Enabled, Flags);

		float fontSize = ImGui::GetFontSize();
		static std::string searchPath;
		static bool isSearching = false;
		bool cantReturn = m_CurrentPath == m_StartPath && !isSearching;

		//+Menubar render

		ImGuiUtils::AddPadding(fontSize * 0.3f, fontSize * 0.3f);
		ImGui::BeginGroup();

		float returnButtonSize = (fontSize * 1.5f) + 5.0f;

		if (cantReturn)
			ImGui::BeginDisabled();

		if (ImGui::Button("<", { returnButtonSize, returnButtonSize }))
			m_CurrentPath = m_CurrentPath.parent_path();

		if (cantReturn)
			ImGui::EndDisabled();

		ImGui::SameLine();

		float settingsButtonSize = ImGui::GetFrameHeight();
		if (ImGuiUtils::ImageButton((ImTextureID)EditorAssets::SettingsTexture->GetTextureId(), { settingsButtonSize, settingsButtonSize }))
			ImGui::OpenPopup("AssetSettingsPopup");

		if (ImGui::BeginPopup("AssetSettingsPopup", ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
			RenderSettings();
			ImGui::EndPopup();
		}

		float availableRegion = ImGui::GetContentRegionAvail().x;
		float searchBarWidth = availableRegion * 0.2f + fontSize * 3.0f;
		ImGui::SameLine();
		ImGuiUtils::AddPadding(availableRegion - searchBarWidth, 0.0f);
		ImGui::SetNextItemWidth(searchBarWidth);

		if (ImGui::InputTextWithHint("##SearchFile", "Search...", searchPath.data(), 255))
		{
			searchPath = searchPath.data();
			isSearching = !searchPath.empty();
		}

		ImGui::EndGroup();

		ImGuiUtils::AddPadding(fontSize * 0.3f, fontSize * 0.3f);
		ImGui::BeginGroup();

		const std::filesystem::path& relative = std::filesystem::relative(m_CurrentPath, m_StartPath);
		float buttonSizes = 0;

		if (cantReturn)
			ImGui::BeginDisabled();

		if (ImGui::Button("Assets"))
			m_CurrentPath = m_StartPath;

		if (cantReturn)
			ImGui::EndDisabled();

		if (m_CurrentPath != m_StartPath)
			for (const auto& p : relative)
			{
				ImGui::SameLine();
				ImGui::Text("/");
				ImGui::SameLine();

				const std::string& txt = p.string();
				if (ImGui::Button(txt.c_str()))
				{
					std::string jumpPath = m_StartPath.string() + "\\";
					for (auto& pa : relative)
					{
						jumpPath += pa.string() + "\\";
						if (pa == p)
							break;
					}
					m_CurrentPath = std::filesystem::relative(jumpPath);
				}
			}

		ImGui::EndGroup();

		//-Menubar render

		ImGuiUtils::AddPadding(0.0f, 15.0f + fontSize * 0.5f);

		//+Directory Render

		float padding = m_PaddingFactor + fontSize * 0.5f;
		float thumbnailSize = (fontSize * 2.0f) + m_ThumbFactor;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGuiTableFlags flags = ImGuiTableFlags_PadOuterX;

		if (ImGui::BeginTable("Cols", columnCount, flags))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			if (!isSearching)
				for (const auto& entry : std::filesystem::directory_iterator(m_CurrentPath))
					ShowEntry(entry, thumbnailSize);
			else
			{
				for (const auto& entry : std::filesystem::recursive_directory_iterator(m_StartPath))
				{
					if (!FilterSearch(entry.path().filename().string(), searchPath))
						continue;

					ShowEntry(entry, thumbnailSize);
				}
			}

			ImGui::EndTable();
		}

		//-Directory Render

		ImGui::End();
	}

	void AssetsView::RenderSettings()
	{
		float fontSize = ImGui::GetFontSize();

		ImGui::BeginChild("##assetssettings", ImVec2(150.0f + fontSize, 0));

		ImGuiUtils::Label("Show Tooltip: ", false);
		ImGuiUtils::Label("Add Padding: ", false);
		ImGuiUtils::Label("Thumbnail Size: ", false);

		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginGroup();

		ImGui::SetNextItemWidth(fontSize * 3.0f);
		ImGui::Checkbox("##snt", &m_ShowNameTooltip);
		ImGui::SetNextItemWidth(fontSize * 3.0f);
		ImGui::DragFloat("##p", &m_PaddingFactor, 0.1f);
		ImGui::SetNextItemWidth(fontSize * 3.0f);
		ImGui::DragFloat("##t", &m_ThumbFactor, 0.1f, 0.0f, 100.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

		ImGui::EndGroup();
	}
}