#pragma once
namespace MoonEngine
{
	class Scene;
	class Serializer
	{
	private:
		Ref<Scene> m_Scene;
	public:
		Serializer(const Ref<Scene>& scene);
		void Serialize(const std::string& filepath);
		bool Deserialize(const std::string& filepath);
		static bool IsValid(const std::string& filepath, const char* fileFormat);
	};
}