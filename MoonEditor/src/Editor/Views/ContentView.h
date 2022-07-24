#pragma once
#include "MoonEngine.h"

namespace MoonEngine
{
	class ContentView
	{
	public:
		ContentView();
		void SetDirectoryPath(const std::string& path) { m_CurrentDirectory = path;  m_StartDirectory = path; }
		void BeginContentView(bool& state);
	private:
		std::filesystem::path m_CurrentDirectory;
		std::filesystem::path m_StartDirectory;

		Ref<Texture> m_FolderIcon;
		Ref<Texture> m_FileIcon;
	};
}