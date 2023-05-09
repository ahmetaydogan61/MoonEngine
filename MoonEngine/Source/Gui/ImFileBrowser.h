#pragma once
#ifndef IMGUI_VERSION
#include <imgui.h>
#endif

#include <misc/cpp/imgui_stdlib.h>
//define NOT_ADDED_STDLIB_CPP if you unresolved external symbol "bool __cdecl ImGui::InputText error.
#ifdef NOT_ADDED_STDLIB_CPP 
#include <misc/cpp/imgui_stdlib.cpp> 
#endif

#include <filesystem>
#include <fstream>

#include "Core/Debug.h"

typedef int ImGuiFilebrowserFlags;

enum ImGuiFileBrowserFlags_
{
	//Decorations
	ImGuiFileBrowserFlags_None = 0,
	ImGuiFileBrowserFlags_NoTitleBar = 1 << 0, // Removes the titlebar.
	ImGuiFileBrowserFlags_ShowOnlyDirectories = 1 << 1, //No files will be shown.
	ImGuiFileBrowserFlags_FilterExtensions = 1 << 2, //Filters files by given extensions.

	//Close Actions
	ImGuiFileBrowserFlags_NoCancelButton = 1 << 3, //Removes the cancel button.
	ImGuiFileBrowserFlags_CloseWithEsc = 1 << 4, //Enables to close with esc key.
	ImGuiFileBrowserFlags_DoubleClickOkay = 1 << 5, //Double clicking will trigger the Okay Action (m_DoneFlag = true).
	ImGuiFileBrowserFlags_NoOkayButton = 1 << 6, //Removes the okay button.
	ImGuiFileBrowserFlags_DontCloseOnDone = 1 << 7, // If you trigger done the popup wont close.
	ImGuiFileBrowserFlags_ResetSelected = 1 << 8, // On Close, it will reset the selected file.
	ImGuiFileBrowserFlags_ResetCurrentDirectory = 1 << 9, //On Close, it will reset Current directory to the starting directory.

	//File Actions
	ImGuiFileBrowserFlags_SelectFiles = 1 << 10, //Allows to select files.
	ImGuiFileBrowserFlags_SelectDirectories = 1 << 11, // Allows to select folders.
	ImGuiFileBrowserFlags_AllowCreateFolder = 1 << 12, //Adds a button that allows you to create folders.
	ImGuiFileBrowserFlags_AllowDelete = 1 << 13, //You can delete files and folders
	ImGuiFileBrowserFlags_AllowRename = 1 << 14, //Allows you to rename files, does not support renaming directories.
	ImGuiFileBrowserFlags_SelectAll = ImGuiFileBrowserFlags_SelectFiles | ImGuiFileBrowserFlags_SelectDirectories, // Allows to select files & folders.

	//File Creation System
	ImGuiFileBrowserFlags_CreateFile = 1 << 15, //This flag allows you to create new files and rename existing files.
	ImGuiFileBrowserFlags_DoneAfterCreateFile = 1 << 16, // If you Create or rename a file with the CreateFile flag, it triggers Okay Action (m_DoneFlag = true).
	ImGuiFileBrowserFlags_OkayTriggersEnter = 1 << 17, // If you click the okay button the InputText content will be triggered.
	ImGuiFileBrowserFlags_ClearSelectRenamedPath = 1 << 18, // If you dont want to select the created or renamed file use this flag.
};

struct ImGuiFileBrowserConfig
{
	const char* Label;
	ImGuiFilebrowserFlags Flags;
	std::filesystem::path StartDirectory;
	std::filesystem::path RootDirectory;
	std::vector<std::string> Extensions;
};

namespace ImGui
{
	class ImGuiFileBrowser
	{
	public:
		ImGuiFileBrowser() = default;
		ImGuiFileBrowser(const ImGuiFileBrowserConfig& config)
			:m_Config(config)
		{
			SetDirectory(config.StartDirectory, config.RootDirectory);
		}

		ImGuiFileBrowser(const ImGuiFileBrowser& other)
		{
			m_Config = other.m_Config;
			m_IsRendering = other.m_IsRendering;
			m_CloseFlag = other.m_CloseFlag;
			m_OpenFlag = other.m_OpenFlag;
			m_DoneFlag = other.m_DoneFlag;
			m_CurrentPath = other.m_CurrentPath;
			m_SelectedPath = other.m_SelectedPath;
			SetDirectory(m_Config.StartDirectory, m_Config.RootDirectory);
		};

		ImGuiFileBrowser& operator=(const ImGuiFileBrowser& other)
		{
			m_Config = other.m_Config;
			m_IsRendering = other.m_IsRendering;
			m_CloseFlag = other.m_CloseFlag;
			m_OpenFlag = other.m_OpenFlag;
			m_DoneFlag = other.m_DoneFlag;
			m_CurrentPath = other.m_CurrentPath;
			m_SelectedPath = other.m_SelectedPath;
			SetDirectory(m_Config.StartDirectory, m_Config.RootDirectory);
			return *this;
		}

		void OpenFileBrowser() { if (m_IsRendering) return; m_OpenFlag = true; }
		void CloseFileBrowser() { if (!m_IsRendering) return;  m_CloseFlag = true; }

		//If you want the root directory to be the same with the starting directory leave it empty or give the same directory address.
		void SetDirectory(const std::filesystem::path& startPath, const std::filesystem::path& rootPath = std::string())
		{
			m_Config.StartDirectory = startPath;
			if (rootPath.empty())
				m_Config.RootDirectory = startPath;
			else
				m_Config.RootDirectory = rootPath;

			SetCurrentPath(startPath);
		}

		void BeginFileBrowser()
		{
			DrawFileBrowser(m_Config.Label, m_Config.Flags);
		}

		//Use this function to use other flags than the m_Config.Flags flags.
		void BeginFileBrowser(const char* label, ImGuiFilebrowserFlags flags = 0)
		{
			DrawFileBrowser(label, flags);
		}

		void ClearBrowser()
		{
			SetCurrentPath(m_Config.StartDirectory);
		}

		void SetCurrentPath(const std::filesystem::path& path)
		{
			m_CurrentPath = path;
			m_SelectedPath.clear();
		}

		bool IsDone() const { return m_DoneFlag; }
		const std::filesystem::path& GetSelected() const { return m_SelectedPath; }
		const std::filesystem::path& GetCurrentPath() const { return m_CurrentPath; }
		bool IsActive() { return m_IsRendering; }
	private:
		ImGuiFileBrowserConfig m_Config;
		bool m_IsRendering = false;
		bool m_CloseFlag = false;
		bool m_OpenFlag = false;
		bool m_DoneFlag = false;
		std::filesystem::path m_CurrentPath;
		std::filesystem::path m_SelectedPath;
		std::string m_FilenameInput;
		std::string m_FilteredExtension;

		void DrawFileBrowser(const char* label, const ImGuiFilebrowserFlags& flags)
		{
			if (m_OpenFlag)
			{
				ImGui::OpenPopup(label);
				m_OpenFlag = false;

				if (flags & ImGuiFileBrowserFlags_ResetCurrentDirectory)
					ClearBrowser();
				if (flags & ImGuiFileBrowserFlags_ResetSelected)
					m_SelectedPath.clear();

				m_FilenameInput.clear();
			}

			if (m_DoneFlag)
				m_DoneFlag = false;

			ImGuiWindowFlags windowFlags = flags & ImGuiFileBrowserFlags_NoTitleBar;
			if (ImGui::BeginPopupModal(label, nullptr, windowFlags))
			{
				m_IsRendering = true;

				if (m_IsRendering)
					DrawContext(flags);

				if (m_DoneFlag)
				{
					if (!(flags & ImGuiFileBrowserFlags_DontCloseOnDone))
						m_CloseFlag = true;
				}

				if ((flags & ImGuiFileBrowserFlags_CloseWithEsc) && ImGui::IsKeyPressed(ImGuiKey_Escape))
					m_CloseFlag = true;

				if (m_CloseFlag)
				{
					m_IsRendering = false;
					m_CloseFlag = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}

		void DrawContext(const ImGuiFilebrowserFlags& flags)
		{
			if (m_Config.StartDirectory.empty())
				return;

			ImVec2 titlebarPadding = ImGui::GetCursorScreenPos();
			titlebarPadding.x += 5.0f;
			titlebarPadding.y += 5.0f;
			ImGui::SetCursorScreenPos(titlebarPadding);

			//Extension Filter Selection
			if (flags & ImGuiFileBrowserFlags_FilterExtensions)
			{
				size_t extensionsSize = m_Config.Extensions.size();
				if (extensionsSize)
				{
					auto& extensions = m_Config.Extensions;
					if (extensionsSize > 1)
						extensions.push_back("All");
					static std::string current_item = extensions[extensions.size() - 1];
					m_FilteredExtension = current_item;
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.125f);
					if (ImGui::BeginCombo("##extensionFilter", current_item.c_str()))
					{
						for (int n = 0; n < extensionsSize + 1; n++)
						{
							bool is_selected = (current_item == extensions[n]);
							if (ImGui::Selectable(extensions[n].c_str(), is_selected))
								current_item = extensions[n];
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
					if (extensionsSize > 1)
						extensions.pop_back();

					ImGui::SameLine();
					ImGui::Text("|");
					ImGui::SameLine();
				}
			}

			//Folder Creation
			if (flags & ImGuiFileBrowserFlags_AllowCreateFolder)
			{
				if (ImGui::Button(" + "))
					ImGui::OpenPopup("AddFolderPopup");

				if (ImGui::BeginPopup("AddFolderPopup"))
				{
					std::string folderName;
					if (ImGui::InputTextWithHint("##folderName", "New Folder Name", &folderName, ImGuiInputTextFlags_EnterReturnsTrue))
					{
						std::filesystem::path cp = m_CurrentPath;
						cp.append(folderName);
						std::filesystem::create_directories(cp);
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}

				ImGui::SameLine();
				ImGui::Text("|");
				ImGui::SameLine();
			}

			if (ImGui::Button(m_Config.RootDirectory.filename().string().c_str()))
				SetCurrentPath(m_Config.RootDirectory);

			if (m_CurrentPath != m_Config.RootDirectory)
			{
				const std::filesystem::path& relative = std::filesystem::relative(m_CurrentPath, m_Config.RootDirectory);
				for (auto& path : relative)
				{
					ImGui::SameLine();
					ImGui::Text("/");
					ImGui::SameLine();

					const std::string& txt = path.string();
					if (ImGui::Button(txt.c_str()))
					{
						std::string jumpPath = m_Config.RootDirectory.string() + "\\";
						for (auto& p : relative)
						{
							jumpPath += p.string() + "\\";
							if (p == path)
								break;
						}
						SetCurrentPath(std::filesystem::relative(jumpPath));
					}
				}
			}

			ImVec2 directoryListPadding = ImGui::GetCursorScreenPos();
			directoryListPadding.y += 5.0f;
			ImGui::SetCursorScreenPos(directoryListPadding);

			float buttonHeight = 10.0f;
			float fontSize = ImGui::GetFontSize();

			ImGui::BeginChild("##entry", { 0.0f, ImGui::GetContentRegionAvail().y - (fontSize + buttonHeight * 2.0f) }, true);

			for (const auto& entry : std::filesystem::directory_iterator(m_CurrentPath))
			{
				const auto& path = entry.path();
				const std::string& filenameString = path.filename().string();
				bool isDirectory = entry.is_directory();

				//Filter by extension
				if (!isDirectory && flags & ImGuiFileBrowserFlags_FilterExtensions)
				{
					bool extensionFound = false;
					for (const auto& extension : m_Config.Extensions)
					{
						if (m_FilteredExtension == "All")
						{
							if (path.filename().extension() == extension)
							{
								extensionFound = true;
								break;
							}
						}
						else
						{
							if (path.filename().extension() == m_FilteredExtension)
							{
								extensionFound = true;
								break;
							}
						}
					}
					if (!extensionFound)
						continue;
				}

				if (flags & ImGuiFileBrowserFlags_ShowOnlyDirectories && !isDirectory)
					continue;

				ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Framed;

				if (m_SelectedPath != path)
					ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Header, { 0.0f, 0.0f, 0.0f, 0.0f });
				else
					ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_HeaderHovered, ImGui::GetStyleColorVec4(ImGuiCol_::ImGuiCol_Header));

				ImGui::TreeNodeEx(filenameString.c_str(), treeFlags);
				if (ImGui::IsItemHovered())
				{
					if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						if (isDirectory)
							SetCurrentPath(path);
						else if (flags & ImGuiFileBrowserFlags_DoubleClickOkay && !isDirectory)
							m_DoneFlag = true;
					}
					else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						if ((isDirectory && flags & ImGuiFileBrowserFlags_SelectDirectories) || (!isDirectory && flags & ImGuiFileBrowserFlags_SelectFiles))
						{
							m_SelectedPath = path;
							m_FilenameInput = m_SelectedPath.filename().string();
						}
					}
				}
				ImGui::PopStyleColor();
			}

			if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				m_SelectedPath.clear();

			ImGui::EndChild();

			ImVec2 actionButtonsPadding = ImGui::GetCursorScreenPos();
			actionButtonsPadding.y += buttonHeight / 2.0f;
			ImGui::SetCursorScreenPos(actionButtonsPadding);

			ImGui::BeginGroup();

			if (!(flags & ImGuiFileBrowserFlags_NoOkayButton))
			{
				if (ImGui::Button("Okay", { 0.0f, fontSize + buttonHeight }))
					m_DoneFlag = true;

				ImGui::SameLine();
			}

			if (!(flags & ImGuiFileBrowserFlags_NoCancelButton))
			{
				if (ImGui::Button("Cancel", { 0.0f, fontSize + buttonHeight }))
					m_CloseFlag = true;

				ImGui::SameLine();
			}

			if ((flags & ImGuiFileBrowserFlags_AllowDelete) && m_SelectedPath != "")
			{
				if (ImGui::Button("Delete", { 0.0f, fontSize + buttonHeight }))
				{
					std::filesystem::remove_all(m_SelectedPath);
					m_SelectedPath.clear();
				}

				ImGui::SameLine();
			}

			if (flags & ImGuiFileBrowserFlags_CreateFile)
			{
				bool entered = false;
				if (m_SelectedPath.string() != "")
				{
					if (ImGui::InputText("##renameFile", &m_FilenameInput, ImGuiInputTextFlags_EnterReturnsTrue))
						entered = true;

					if (entered || (m_DoneFlag && (flags & ImGuiFileBrowserFlags_OkayTriggersEnter)))
					{
						std::filesystem::path newPath = m_CurrentPath / m_FilenameInput;
						std::filesystem::rename(m_SelectedPath, newPath);

						if (flags & ImGuiFileBrowserFlags_ClearSelectRenamedPath)
						{
							m_SelectedPath.clear();
							m_FilenameInput.clear();
						}
						else
							m_SelectedPath = newPath;
					}
				}
				else
				{
					if (ImGui::InputText("##createFile", &m_FilenameInput, ImGuiInputTextFlags_EnterReturnsTrue))
						entered = true;

					if (entered || (m_DoneFlag && (flags & ImGuiFileBrowserFlags_OkayTriggersEnter)))
					{
						std::filesystem::path newPath = m_CurrentPath / m_FilenameInput;
						std::ofstream outfile(newPath);
						outfile.close();

						if (flags & ImGuiFileBrowserFlags_ClearSelectRenamedPath)
						{
							m_SelectedPath.clear();
							m_FilenameInput.clear();
						}
						else
							m_SelectedPath = newPath;
					}
				}

				if (entered && flags & ImGuiFileBrowserFlags_DoneAfterCreateFile)
					m_DoneFlag = true;
			}
			else if (flags & ImGuiFileBrowserFlags_AllowRename && m_SelectedPath != "")
			{
				bool entered = false;

				if (ImGui::InputText("##renameFile", &m_FilenameInput, ImGuiInputTextFlags_EnterReturnsTrue))
					entered = true;

				if (entered || (m_DoneFlag && (flags & ImGuiFileBrowserFlags_OkayTriggersEnter)))
				{
					std::filesystem::path newPath = m_CurrentPath / m_FilenameInput;
					std::filesystem::rename(m_SelectedPath, newPath);

					if (flags & ImGuiFileBrowserFlags_ClearSelectRenamedPath)
					{
						m_SelectedPath.clear();
						m_FilenameInput.clear();
					}
					else
						m_SelectedPath = newPath;
				}
			}
			ImGui::EndGroup();
		}
	};
}
