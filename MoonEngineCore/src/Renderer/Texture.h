#pragma once
namespace MoonEngine
{
	class Texture
	{
	private:
		uint32_t m_TexBuffer;
		int m_Width, m_Height, m_BPP;

		void CreateTexture(unsigned char* data);
	public:
		Texture();
		Texture(uint32_t width, uint32_t height);
		Texture(const std::string& filepath);
		~Texture();

		std::string Filepath;

		void Bind(unsigned int slot = 0) const;
		void Unbind() const;

		bool IsValid() { return GetID(); }

		int GetWidth()  const { return m_Width; }
		int GetHeight() const { return m_Height; }
		uint32_t GetID() const { return m_TexBuffer; }
	};
}