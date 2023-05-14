#pragma once

namespace MoonEngine
{
	class Texture;

	class TextureSheet
	{
	public:
		void Create(Shared<Texture>& texture);

		const glm::vec2& GetTexCoord(int i) const { return m_TextureCoords[i]; }
		const Shared<Texture>& GetTexture() const { return m_Texture; }

		void Resize(const glm::vec2& size) { m_SpriteSize = size; CalculateTexCoords(); }
		void SetCoordinate(const glm::vec2& coordinate) { m_Coordinates = coordinate; CalculateTexCoords(); }
	private:
		Shared<Texture> m_Texture;

		glm::vec2 m_Coordinates;
		glm::vec2 m_SpriteSize;
		glm::vec2 m_TextureCoords[4];

		void CalculateTexCoords();
	};
}