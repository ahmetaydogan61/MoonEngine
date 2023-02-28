#include "mpch.h"
#include "Renderer/Framebuffer.h"

#include "Core/Debug.h"

#include <glad/glad.h>

namespace MoonEngine
{
	Framebuffer::Framebuffer(uint32_t& width, uint32_t& height)
		:m_Width(width), m_Height(height)
	{
		Invalidate();
	}

	static void GenerateTexture(uint32_t& textureId, uint32_t index, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, textureId, 0);
	}

	void Framebuffer::Invalidate()
	{
		if (m_FramebufferId)
			Destroy();

		glCreateFramebuffers(1, &m_FramebufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);

		if (m_Props.ColorAttachments.size())
		{
			uint32_t index = 0;
			for (auto& format : m_Props.ColorAttachments)
			{
				switch (format.TextureFormat)
				{
					case FramebufferTextureFormat::RGBA8:
						GenerateTexture(format.ID, index++, GL_RGBA8, GL_RGBA, m_Width, m_Height);
						break;
					case FramebufferTextureFormat::RED_INTEGER:
						GenerateTexture(format.ID, index++, GL_R32I, GL_RED_INTEGER, m_Width, m_Height);
						break;
				}
			}
		}

		if (m_Props.DepthAttachment.TextureFormat == FramebufferTextureFormat::DEPTH)
		{
			auto& depthAttachment = m_Props.DepthAttachment;
			glCreateTextures(GL_TEXTURE_2D, 1, &depthAttachment.ID);
			glBindTexture(GL_TEXTURE_2D, depthAttachment.ID);

			glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Width, m_Height);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAttachment.ID, 0);
		}

		uint32_t colorAttachmentsSize = m_Props.ColorAttachments.size();
		if (colorAttachmentsSize > 1 && colorAttachmentsSize < 4)
		{
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(colorAttachmentsSize, buffers);
		}

		ME_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Failed To Create Framebuffer");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::ClearColorAttachment(uint32_t attachmentIndex, void* clearData)
	{
		auto& attachment = m_Props.ColorAttachments[attachmentIndex];

		switch (attachment.TextureFormat)
		{
			case FramebufferTextureFormat::RGBA8:
				glClearTexImage(attachment.ID, 0, GL_RGBA, GL_FLOAT, &clearData);
				break;
			case FramebufferTextureFormat::RED_INTEGER:
				glClearTexImage(attachment.ID, 0, GL_RED_INTEGER, GL_INT, &clearData);
				break;
		}
	}

	int Framebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		if (attachmentIndex >= m_Props.ColorAttachments.size())
			return -1;

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);

		return pixelData;
	}

	void Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width <= 0 || height <= 0)
		{
			return;
		}

		m_Width = width;
		m_Height = height;
		Invalidate();
	}

	void Framebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);
		glViewport(0, 0, m_Width, m_Height);
	}

	void Framebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::Destroy()
	{
		glDeleteFramebuffers(1, &m_FramebufferId);

		if (m_Props.ColorAttachments.size())
			for (auto& format : m_Props.ColorAttachments)
			{
				glDeleteTextures(1, &format.ID);
			}

		if (m_Props.DepthAttachment.TextureFormat == FramebufferTextureFormat::DEPTH)
			glDeleteTextures(1, &m_Props.DepthAttachment.ID);
	}

	Framebuffer::~Framebuffer()
	{
		Destroy();
	}
}