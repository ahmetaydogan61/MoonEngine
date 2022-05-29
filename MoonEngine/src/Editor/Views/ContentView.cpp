#include "mpch.h"
#include "ContentView.h"
#include "../ImGuiUtils.h"

#include <imgui/imgui.h>

namespace MoonEngine
{
	extern const std::filesystem::path resourcesPath = "res/Assets";

	ContentView::ContentView()
		: m_CurrentDirectory(resourcesPath)
	{
		m_FolderIcon = CreateRef<Texture>("res/EditorIcons/Folder.png");
		m_FileIcon = CreateRef<Texture>("res/EditorIcons/File.png");
	}

	void ContentView::BeginContentView(bool& state)
	{
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);
		ImGui::Begin("Content Browser", &state, ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
			if (m_CurrentDirectory != std::filesystem::path(resourcesPath))
				if (ImGui::Button(" < "))
					m_CurrentDirectory = m_CurrentDirectory.parent_path();

			//TODO: Search file
			float searchBarWidth = 200.0f;
			static char searchPath[255];
			ImGuiUtils::AddPadding(ImGui::GetContentRegionAvail().x - searchBarWidth, 0.0f);
			ImGui::SetNextItemWidth(searchBarWidth);
			if (ImGui::InputTextWithHint("##SearchFile", "Search...", searchPath, 255, ImGuiInputTextFlags_EnterReturnsTrue))
				memset(searchPath, 0, 255);
			//file search
			ImGui::EndMenuBar();
		}

		float padding = 16.0f;
		float thumbnailSize = 70.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);
		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, resourcesPath);
			std::string filenameString = relativePath.filename().string();

			ImGui::PushID(filenameString.c_str());
			Ref<Texture> icon = directoryEntry.is_directory() ? m_FolderIcon : m_FileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGuiUtils::ImageButton((ImTextureID)icon->GetID(), { thumbnailSize, thumbnailSize });
			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("MNE_AssetItem", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();
			}
			ImGui::TextWrapped(filenameString.c_str());

			ImGui::NextColumn();
			ImGui::PopID();
		}

		ImGui::Columns(1);
		ImGui::End();
		ImGui::PopStyleColor();
	}
}