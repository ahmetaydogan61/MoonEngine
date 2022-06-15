#pragma once

namespace MoonEngine
{
	class Texture;
	struct ResourceManagerDesc
	{
		std::string ProjectFolder;
		std::string AssetFolder;
	};

	class ResourceManager
	{
	private:
		static ResourceManagerDesc m_Paths;
		static std::unordered_map<std::string, Ref<Texture>> m_TextureCache;
	public:
		static void SetResourceManager(ResourceManagerDesc desc);
		static const std::string& GetAssetPath() { return m_Paths.AssetFolder; }
		
		static Ref<Texture> LoadTexture(const std::string& path);
		static void UnloadTexture(const std::string& path);
		static int TextureCount() { return m_TextureCache.size(); }

		static const std::unordered_map<std::string, Ref<Texture>>& GetMap() { return m_TextureCache; }
	};
}