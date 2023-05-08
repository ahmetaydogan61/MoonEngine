#pragma once
#include "Views/Views.h"

namespace MoonEngine
{
	class Texture;

	struct AssetsView : public BasicView
	{
	public:
		AssetsView() = default;
		AssetsView(const std::filesystem::path& startPath);
		~AssetsView() = default;

		void Render();
		void SetPath(const std::filesystem::path& path) { m_StartPath = path; }
		void RenderSettings();
	private:
		Shared<Texture> m_FileIcon, m_FolderIcon, m_SettingsIcon;
		std::filesystem::path m_StartPath, m_CurrentPath;

		float m_PaddingFactor = 16.0f;
		float m_ThumbFactor = 40.0f;
		bool m_ShowNameTooltip = true;

		void ShowEntry(const std::filesystem::directory_entry& entry, float thumbnailSize);
	};
}