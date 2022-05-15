#pragma once
namespace MoonEngine
{
	class Framebuffer
	{
	private:
		unsigned int m_Buffer;
		uint32_t m_Width;
		uint32_t m_Height;
		unsigned int m_TexBuffer;
		unsigned int m_DepthBuffer;

	public:
		Framebuffer();
		Framebuffer(uint32_t& width, uint32_t& height);
		void Invalidate();
		void Resize(uint32_t width, uint32_t height);
		void Bind();
		void Unbind();
		void Destroy();
		unsigned int GetTexID() { return m_TexBuffer; }
		inline uint32_t GetHeight() const { return m_Height; };
		inline uint32_t GetWidth() const { return m_Width; };
	};
}