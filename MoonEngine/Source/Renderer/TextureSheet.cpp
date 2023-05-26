#include "mpch.h"
#include "Renderer/TextureSheet.h"

#include "Renderer/Texture.h"

namespace MoonEngine
{
	void TextureSheet::Create(Shared<Texture>& texture)
	{
		m_Texture = texture;
		CalculateTexCoords();
	}

	void TextureSheet::CalculateTexCoords()
	{
		float width = (float)m_Texture->GetWidth();
		float height = (float)m_Texture->GetHeight();

		float spriteX = m_SpriteSize.x;
		float spriteY = m_SpriteSize.y;

		m_TextureCoords[0] = { (m_Coordinates.x * spriteX) / width, (m_Coordinates.y * spriteY) / height };
		m_TextureCoords[1] = { ((m_Coordinates.x + 1) * spriteX) / width, (m_Coordinates.y * spriteY) / height };
		m_TextureCoords[2] = { ((m_Coordinates.x + 1) * spriteX) / width, ((m_Coordinates.y + 1) * spriteY) / height };
		m_TextureCoords[3] = { (m_Coordinates.x * spriteX) / width, ((m_Coordinates.y + 1) * spriteY) / height };
	}
}