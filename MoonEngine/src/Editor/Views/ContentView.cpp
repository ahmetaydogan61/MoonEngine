#include "mpch.h"
#include "ContentView.h"

#include "../ImGuiUtils.h"

#include <imgui/imgui.h>

#include <filesystem>

namespace MoonEngine
{
	ContentView::ContentView()
	{
		m_FolderIcon = CreateRef<Texture>("res/EditorIcons/Folder.png");
		m_FileIcon = CreateRef<Texture>("res/EditorIcons/File.png");
	}

	std::string searchPath;
	std::string searchFormat;
	static bool FilterSearch(const std::string& fileName, const std::string& searchedFile, const std::string& fileFormat = "")
	{
		if (!fileFormat.empty())
			if (fileName.substr(fileName.find_last_of(".") + 1) != fileFormat)
			{
				return 0;
			}

		auto it = std::search(
			fileName.begin(), fileName.end(),
			searchedFile.begin(), searchedFile.end(),
			[](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); }
		);
		return (it != fileName.end());
	}

	void ContentView::BeginContentView(bool& state)
	{
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);
		ImGui::Begin("Content Browser", &state, ImGuiWindowFlags_MenuBar);

		static bool isSearching = false;

		if (ImGui::BeginMenuBar())
		{
			if (m_CurrentDirectory != std::filesystem::path(m_StartDirectory) && !isSearching)
				if (ImGui::Button(" < "))
					m_CurrentDirectory = m_CurrentDirectory.parent_path();

			//+Search file
			
			float searchBarWidth = 200.0f;
			ImGuiUtils::AddPadding(ImGui::GetContentRegionAvail().x - searchBarWidth, 0.0f);
			ImGui::SetNextItemWidth(searchBarWidth);
			if (ImGui::InputTextWithHint("##SearchFile", "Search...", searchPath.data(), 255))
			{
				searchPath = searchPath.data();
				isSearching = !searchPath.empty();
			}

			float offsetToSearchbar = 10.0f;
			auto& textSize = ImGui::CalcTextSize("Filter:");
			ImGuiUtils::AddPadding(ImGui::GetContentRegionAvail().x - (textSize.x + searchBarWidth) * 2.0f, 0.0f);
			ImGuiUtils::Label("Filter:", false);
			ImGuiUtils::AddPadding(ImGui::GetContentRegionAvail().x - offsetToSearchbar - searchBarWidth * 2.0f, 0.0f);
			static int currentSearchFormat = 0;
			ImGui::SetNextItemWidth(searchBarWidth);
			if (ImGui::Combo("##Filter", &currentSearchFormat, "All\0Images\0Scenes\0"))
			{
				switch (currentSearchFormat)
				{
					case 0:
					{
						searchFormat.clear();
						break;
					}
					case 1:
					{
						searchFormat = "png";
						break;
					}
					case 2:
					{
						searchFormat = "moon";
						break;
					}
					default:
					{
						searchFormat.clear();
						break;
					}
				}
			}
			//-file search
			ImGui::EndMenuBar();
		}

		float padding = 16.0f;
		float thumbnailSize = 70.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		bool itemPopup = false;

		if (!isSearching)
		{
			ImGui::Columns(columnCount, 0, false);
			for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
			{
				const auto& path = directoryEntry.path();
				auto relativePath = std::filesystem::relative(path, ResourceManager::GetAssetPath());

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

				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					ImGui::OpenPopup("ContentItemPopup");
					itemPopup = true;
				}

				if (ImGui::BeginPopup("ContentItemPopup"))
				{
					if (directoryEntry.is_directory())
					{
						if (ImGui::MenuItem("Delete Folder"))
							std::filesystem::remove_all(path);
					}
					else
					{
						if (ImGui::MenuItem("Delete File"))
						{
							std::filesystem::remove(path);
							ResourceManager::UnloadTexture(relativePath.string());
						}
					}
					ImGui::EndPopup();
				}

				ImGui::TextWrapped(filenameString.c_str());

				ImGui::NextColumn();
				ImGui::PopID();
			}
			ImGui::Columns(1);
		}
		else //ContentBrowser while searching a directory
		{
			ImGui::Columns(columnCount, 0, false);
			for (auto& directoryEntry : std::filesystem::recursive_directory_iterator(m_StartDirectory))
			{
				if (!FilterSearch(directoryEntry.path().filename().string(), searchPath, searchFormat))
					continue;
				const auto& path = directoryEntry.path();
				auto relativePath = std::filesystem::relative(path, ResourceManager::GetAssetPath());

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
					else
					{
						searchPath.clear();
						isSearching = false;
						m_CurrentDirectory = directoryEntry.path().parent_path();
					}
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					ImGui::OpenPopup("ContentItemPopup");
					itemPopup = true;
				}

				if (ImGui::BeginPopup("ContentItemPopup"))
				{
					if (directoryEntry.is_directory())
					{
						if (ImGui::MenuItem("Delete Folder"))
							std::filesystem::remove_all(path);
					}
					else
					{
						if (ImGui::MenuItem("Delete File"))
						{
							std::filesystem::remove(path);
							ResourceManager::UnloadTexture(relativePath.string());
						}
					}
					ImGui::EndPopup();
				}

				ImGui::TextWrapped(filenameString.c_str());

				ImGui::NextColumn();
				ImGui::PopID();
			}
			ImGui::Columns(1);
		}

		ImGui::End();
		ImGui::PopStyleColor();
	}
}