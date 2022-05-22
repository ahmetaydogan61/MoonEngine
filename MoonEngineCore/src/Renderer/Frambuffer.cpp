#include "mpch.h"
#include "Frambuffer.h"
#include "glad/glad.h"

namespace MoonEngine
{
	Framebuffer::Framebuffer()
		:m_Width(0), m_Height(0)
	{
		Invalidate();
	}

	Framebuffer::Framebuffer(uint32_t& width, uint32_t& height)
		:m_Width(width), m_Height(height)
	{
		Invalidate();
	}

	void Framebuffer::Invalidate()
	{
		if (m_Buffer)
			Destroy();
		glGenFramebuffers(1, &m_Buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_Buffer);

		glGenTextures(1, &m_TexBuffer);
		glBindTexture(GL_TEXTURE_2D, m_TexBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TexBuffer, 0);

		/*
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, m_Width, m_Height, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 1, GL_TEXTURE_2D, m_TexBuffer, 0);
		*/

		glGenTextures(1, &m_DepthBuffer);
		glBindTexture(GL_TEXTURE_2D, m_DepthBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Width, m_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthBuffer, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;
		Invalidate();
	}

	void Framebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_Buffer);
		glViewport(0, 0, m_Width, m_Height);
	}

	void Framebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::Destroy()
	{
		glDeleteFramebuffers(1, &m_Buffer);
		glDeleteTextures(1, &m_TexBuffer);
		glDeleteTextures(1, &m_DepthBuffer);

		m_TexBuffer = 0;
		m_DepthBuffer = 0;
	}
}