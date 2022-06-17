#pragma once

namespace MoonEngine
{
	class Texture;
	class Scene;

	struct ResourceManagerDesc
	{
		Ref<Scene> Scene;
		std::string ProjectFolder;
		std::string AssetFolder;
	};

	class ResourceManager
	{
	private:
		static ResourceManagerDesc m_Desc;
		static std::unordered_map<std::string, Ref<Texture>> m_TextureCache;
	public:
		static void SetResourceManager(ResourceManagerDesc desc);
		static void OnSceneChange(Ref<Scene> scene) 
		{ 
			m_Desc.Scene = scene; 
			for (auto& it : m_TextureCache)
				UnloadTexture(it.first);
			CollectGarbage();
		}
		static const std::string& GetAssetPath() { return m_Desc.AssetFolder; }

		static Ref<Texture> LoadTexture(const std::string& path);
		static void UnloadTexture(const std::string& path);
		static int TextureCount() { return m_TextureCache.size(); }

		static const std::unordered_map<std::string, Ref<Texture>>& GetMap() { return m_TextureCache; }
		static void CollectGarbage();
	};
}