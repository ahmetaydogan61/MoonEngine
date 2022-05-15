#include "mpch.h"
#include "Texture.h"
#include "glad/glad.h"
#include "stb_image/stb_image.h"

namespace MoonEngine
{
	Texture::Texture(const std::string& filepath)
		:m_TexBuffer(0), m_FilePath(filepath), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
	{
		stbi_set_flip_vertically_on_load(1);
		m_LocalBuffer = stbi_load(m_FilePath.c_str(), &m_Width, &m_Height, &m_BPP, 4);
		
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
	
	Texture::~Texture()
	{
		glDeleteTextures(1, &m_TexBuffer);
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
}