#pragma once

namespace MoonEngine
{
	class Scene;

	class SceneSerializer
	{
	public:
		static void Serialize(const Shared<Scene>& scene, const std::filesystem::path& path);
		static void Deserialize(const Shared<Scene>& scene, const std::filesystem::path& path);
	private:
	};
}