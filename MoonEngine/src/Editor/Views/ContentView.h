#pragma once

#include <filesystem>

#include "Renderer/Texture.h"

namespace MoonEngine
{
	class ContentView
	{
	public:
		ContentView();

		void BeginContentView(bool& state);
	private:
		std::filesystem::path m_CurrentDirectory;

		Texture* m_FolderIcon;
		Texture* m_FileIcon;
	};
}