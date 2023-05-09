#pragma once

#include <Renderer/Texture.h>

namespace MoonEngine
{
	struct EditorAssets
	{
		static Shared<Texture> PlayTexture, StopTexture, PauseTexture, SettingsTexture;
		static Shared<Texture> SelectTexture, TranslateTexture, ResizeTexture, RotateTexture, TransformationTexture;
		static Shared<Texture> CameraTexture, FlareTexture;

		static void LoadTextures();
	};
}