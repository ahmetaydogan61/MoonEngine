#include "mpch.h"
#include "Texture.h"
#include "glad/glad.h"
#include "stb_image.h"
#include "stb_image.cpp"

namespace MoonEngine
{
	Texture::Texture()
		:m_TexBuffer(0), m_LocalBuffer(nullptr), m_Width(1), m_Height(1), m_BPP(4)
	{
		int size = m_Width * m_Height * m_BPP;
		m_LocalBuffer = new unsigned char[size];
		for (int i = 0; i < size; i++)
			m_LocalBuffer[i] = 0xFF;

		glGenTextures(1, &m_TexBuffer);
		glBindTexture(GL_TEXTURE_2D, m_TexBuffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture::Texture(unsigned int width, unsigned int height)
		:m_TexBuffer(0), m_LocalBuffer(nullptr), m_Width(width), m_Height(height), m_BPP(4)
	{
		int size = m_Width * m_Height * m_BPP;
		m_LocalBuffer = new unsigned char[size];
		for (int i = 0; i < size; i++)
			m_LocalBuffer[i] = 0xFF;

		glGenTextures(1, &m_TexBuffer);
		glBindTexture(GL_TEXTURE_2D, m_TexBuffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture::Texture(const std::string& filepath)
		:m_TexBuffer(0), Filepath(filepath), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
	{
		stbi_set_flip_vertically_on_load(1);
		m_LocalBuffer = stbi_load(Filepath.c_str(), &m_Width, &m_Height, &m_BPP, 4);
		
		if (!m_LocalBuffer)
		{
			DebugWar("Texture Creation Failed");
			return;
		}

		glGenTextures(1, &m_TexBuffer);
		glBindTexture(GL_TEXTURE_2D, m_TexBuffer);
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		if (m_LocalBuffer)
			stbi_image_free(m_LocalBuffer);
	}
	
	void Texture::Bind(unsigned int slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_TexBuffer);
	}
	
	void Texture::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &m_TexBuffer);
	}
}