#pragma once

namespace MoonEngine
{
	enum class WrapMode
	{
		Repeat,
		MirroredRepeat,
		EdgeClamp,
		BorderClamp
	};

	enum FilterType
	{
		Nearest,
		Linear
	};

	struct TextureProps
	{
		WrapMode WrapMode = WrapMode::Repeat;
		FilterType FilterType = FilterType::Linear;
		bool GenerateMipmap = false;
	};

	class Texture
	{
	private:
		std::filesystem::path m_Path;
		TextureProps m_Props;

		uint32_t m_TextureId = 0;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		uint32_t m_Channels = 0;

		void CreateTexture(void* data);
		void GenerateTextureProps();
	public:
		Texture(const std::string& path, TextureProps props = {});
		Texture(uint32_t width, uint32_t height, TextureProps props = {});
		Texture(TextureProps props = {});
		~Texture();

		void SetData(void* data);
		void Bind(uint32_t slot = 0) const;
		void Unbind() const;

		const std::filesystem::path& GetPath() const { return m_Path; }
		uint32_t GetTextureId() const { return m_TextureId; }
		uint32_t GetWidth() const { return m_Width; };
		uint32_t GetHeight() const { return m_Height; };
	};
}