#pragma once
namespace MoonEngine
{
	class Texture
	{
	private:
		unsigned int m_TexBuffer;
		unsigned char* m_LocalBuffer;
		int m_Width, m_Height, m_BPP;
	public:
		Texture();
		Texture(unsigned int width, unsigned int height);
		Texture(const std::string& filepath);
		std::string Filepath;
		~Texture();
	
		void Bind(unsigned int slot = 0) const;
		void Unbind() const;
	
		bool IsValid() { return GetID(); }

		inline int GetWidth()  const { return m_Width; }
		inline int GetHeight() const { return m_Height; }
		unsigned int GetID() const { return m_TexBuffer;  }
	};
}