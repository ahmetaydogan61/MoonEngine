#include "mpch.h"
#include "Renderer/Texture.h"

#include "Core/Debug.h"

#include <stb_image.h>
#include <glad/glad.h>

namespace MoonEngine
{
	Texture::Texture(TextureProps props)
		:m_Props(props)
	{
		m_Width = 1;
		m_Height = 1;
		m_Channels = 4;
		uint32_t data = 0xffffffff;
		SetTexture(&data);
	}

	Texture::Texture(uint32_t width, uint32_t height, TextureProps props)
		:m_Props(props)
	{
		m_Width = width;
		m_Height = height;
		m_Channels = 4;

		GenerateTextureProps();
		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureId);
		glTextureStorage2D(m_TextureId, 1, GL_RGBA8, m_Width, m_Height);
	}

	Texture::Texture(const std::string& path, TextureProps props)
		:m_Props(props)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);

		if (data)
		{
			m_Path = path;
			m_Width = width;
			m_Height = height;
			m_Channels = channels;
			SetTexture(data);
		}
		else
			ME_SYS_WAR("Texture Creation Failed!");

		stbi_image_free(data);
	}

	void Texture::SetTexture(void* data)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureId);

		GenerateTextureProps();

		glTextureStorage2D(m_TextureId, 1, GL_RGBA8, m_Width, m_Height);
		glTextureSubImage2D(m_TextureId, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::GenerateTextureProps()
	{
		switch (m_Props.WrapMode)
		{
			case WrapMode::Repeat:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				break;
			case WrapMode::MirroredRepeat:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				break;
			case WrapMode::EdgeClamp:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				break;
			case WrapMode::BorderClamp:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				break;
		}

		switch (m_Props.FilterType)
		{
			case FilterType::Linear:
				if (m_Props.GenerateMipmap)
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				else
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;

			case FilterType::Nearest:
				if (m_Props.GenerateMipmap)
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
				else
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;
		}

		if (m_Props.GenerateMipmap)
			glGenerateMipmap(GL_TEXTURE_2D);
	}

	void Texture::SetData(void* data)
	{
		glTextureSubImage2D(m_TextureId, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	void Texture::Bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_TextureId);
	}

	void Texture::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &m_TextureId);
	}
}