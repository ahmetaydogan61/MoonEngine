#include "mpch.h"
#include "ResourceManager.h"

#include "Engine/Scene.h"
#include "Engine/Components.h"
#include "Renderer/Texture.h"

namespace MoonEngine
{
	ResourceManagerDesc ResourceManager::m_Desc;
	std::unordered_map<std::string, Ref<Texture>> ResourceManager::m_TextureCache;

	void ResourceManager::SetResourceManager(ResourceManagerDesc desc) { m_Desc = desc; }

	std::vector<std::string> garbage;

	Ref<Texture> ResourceManager::LoadTexture(const std::string& path)
	{
		std::string newPath(path);
		std::replace(newPath.begin(), newPath.end(), '\\', '/');
		size_t directorySize = m_Desc.AssetFolder.size() - 1;

		if (newPath.substr(0, directorySize) != m_Desc.AssetFolder.substr(0, directorySize))
			newPath = m_Desc.AssetFolder + "/" + path;

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
		std::replace(newPath.begin(), newPath.end(), '\\', '/');
		size_t directorySize = m_Desc.AssetFolder.size() - 1;

		if (newPath.substr(0, directorySize) != m_Desc.AssetFolder.substr(0, directorySize))
			newPath = m_Desc.AssetFolder + "/" + path;

		if (m_TextureCache.find(newPath) != m_TextureCache.end())
		{
			garbage.push_back(newPath);

			auto spriteView = m_Desc.Scene->m_Registry.view<SpriteComponent>();
			for (auto e : spriteView)
			{
				SpriteComponent& component = spriteView.get<SpriteComponent>(e);
				if(component.Texture && !component.Texture->Filepath.empty())
					if (component.Texture->Filepath == newPath)
						component.Texture = nullptr;
			}

			auto particleView = m_Desc.Scene->m_Registry.view<ParticleComponent>();
			for (auto e : particleView)
			{
				ParticleComponent& component = particleView.get<ParticleComponent>(e);
				if (component.Texture && !component.Texture->Filepath.empty())
					component.Texture = nullptr;
			}
		}
	}

	void ResourceManager::CollectGarbage()
	{
		for (auto& it : m_TextureCache)
			if (it.second.use_count() == 1)
				garbage.push_back(it.first);

		bool collected = false;
		for (auto& it : garbage)
		{
			m_TextureCache.erase(it);
			collected = true;
		}

		if (collected)
			garbage.clear();
	}
}