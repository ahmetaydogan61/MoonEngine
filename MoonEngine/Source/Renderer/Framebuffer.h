#pragma once

namespace MoonEngine
{
	enum class FramebufferTextureFormat
	{
		None,
		RGBA8,
		RED_INTEGER,
		DEPTH
	};

	struct FramebufferTextureProps
	{
		FramebufferTextureProps() = default;
		FramebufferTextureProps(FramebufferTextureFormat format)
			: TextureFormat(format) {}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		uint32_t ID = 0;
	};

	struct FramebufferProps
	{
		FramebufferProps() = default;
		FramebufferProps(std::initializer_list<FramebufferTextureProps> attachments)
		{
			for (auto& attachment : attachments)
			{
				if (attachment.TextureFormat != FramebufferTextureFormat::None)
				{
					if (attachment.TextureFormat != FramebufferTextureFormat::DEPTH)
						ColorAttachments.push_back(attachment);
					else
						DepthAttachment = attachment;
				}
			}
		}

		std::vector<FramebufferTextureProps> ColorAttachments;
		FramebufferTextureProps DepthAttachment = FramebufferTextureFormat::None;
	};

	class Framebuffer
	{
	public:
		Framebuffer() = delete;
		Framebuffer(FramebufferProps prop)
			:m_Props(prop)
		{
			Invalidate();
		}
		Framebuffer(uint32_t& width, uint32_t& height);
		~Framebuffer();

		void Invalidate();
		void Resize(uint32_t width, uint32_t height);

		void Bind();
		void Unbind();
		void Destroy();

		uint32_t GetTexID(uint32_t attachmentIndex = 0) const { return m_Props.ColorAttachments[attachmentIndex].ID; }
		uint32_t GetHeight() const { return m_Height; };
		uint32_t GetWidth() const { return m_Width; };

		void ClearColorAttachment(uint32_t attachmentIndex, void* clearData);
		int ReadPixel(uint32_t index, int x, int y);
	private:
		uint32_t m_FramebufferId = 0;
		uint32_t textureId = 0;
		uint32_t redId = 0;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;

		FramebufferProps m_Props;
	};
}