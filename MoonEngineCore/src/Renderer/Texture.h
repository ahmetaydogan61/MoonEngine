#pragma once
namespace MoonEngine
{
	class Texture
	{
	private:
		unsigned int m_TexBuffer;
		std::string m_FilePath;
		unsigned char* m_LocalBuffer;
		int m_Width, m_Height, m_BPP;
	
	public:
		Texture(unsigned int width, unsigned int height);
		Texture(const std::string& filepath);
		~Texture();
	
		void Bind(unsigned int slot = 0) const;
		void Unbind() const;
	
		inline int GetWidth()  const { return m_Width; }
		inline int GetHeight() const { return m_Height; }
		unsigned int GetID() const { return m_TexBuffer;  }
	};
}