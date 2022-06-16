#include "mpch.h"
#include "ResourceManager.h"
#include "Renderer/Texture.h"

#include "uuid_v4.h"

namespace MoonEngine
{
	ResourceManagerDesc ResourceManager::m_Paths;
	std::unordered_map<std::string, Ref<Texture>> ResourceManager::m_TextureCache;

	void ResourceManager::SetResourceManager(ResourceManagerDesc desc) { m_Paths = desc; }

	Ref<Texture> ResourceManager::LoadTexture(const std::string& path)
	{
		std::string newPath(path);
		size_t directorySize = m_Paths.AssetFolder.size() - 1;

		if (newPath.substr(0, directorySize) != m_Paths.AssetFolder.substr(0, directorySize))
			newPath = m_Paths.AssetFolder + "/" + path;

		if (m_TextureCache.find(newPath) != m_TextureCache.end())
			return m_TextureCache.at(newPath);

		Ref<Texture> texture = CreateRef<Texture>(newPath);
		if (texture->IsValid())
		{
			m_TextureCache[newPath] = texture;
			return texture;
		}
		return nullptr;
	}

	void ResourceManager::UnloadTexture(const std::string& path)
	{
		std::string newPath(path);
		size_t directorySize = m_Paths.AssetFolder.size() - 1;

		if (newPath.substr(0, directorySize) != m_Paths.AssetFolder.substr(0, directorySize))
			newPath = m_Paths.AssetFolder + "/" + path;

		if (m_TextureCache.find(newPath) != m_TextureCache.end())
		{
			auto& texture = m_TextureCache.at(newPath);
			m_TextureCache.erase(newPath);
			texture = nullptr;
		}
	}
}