#pragma once

#include <Renderer/Texture.h>

namespace MoonEngine
{
	struct EditorAssets
	{
		static Shared<Texture> PlayTexture, StopTexture, PauseTexture;
		static Shared<Texture> SelectTexture, TranslateTexture, ResizeTexture, RotateTexture;

		static void LoadTextures();
	};
}