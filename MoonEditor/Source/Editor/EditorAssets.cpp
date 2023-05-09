#include "mpch.h"
#include "EditorAssets.h"

namespace MoonEngine
{
	Shared<Texture> EditorAssets::PlayTexture, EditorAssets::StopTexture, EditorAssets::PauseTexture, EditorAssets::SettingsTexture;
	Shared<Texture> EditorAssets::SelectTexture, EditorAssets::TranslateTexture, EditorAssets::ResizeTexture, EditorAssets::RotateTexture, EditorAssets::TransformationTexture;
	Shared<Texture> EditorAssets::CameraTexture, EditorAssets::FlareTexture;

	void EditorAssets::LoadTextures()
	{
		PlayTexture = MakeShared<Texture>("Resource/EditorIcons/Play.png");
		StopTexture = MakeShared<Texture>("Resource/EditorIcons/Stop.png");
		PauseTexture = MakeShared<Texture>("Resource/EditorIcons/Pause.png");
		SettingsTexture = MakeShared<Texture>("Resource/EditorIcons/Settings.png");

		SelectTexture = MakeShared<Texture>("Resource/EditorIcons/Select.png");
		TranslateTexture = MakeShared<Texture>("Resource/EditorIcons/Translate.png");
		ResizeTexture = MakeShared<Texture>("Resource/EditorIcons/Resize.png");
		RotateTexture = MakeShared<Texture>("Resource/EditorIcons/Rotate.png");
		TransformationTexture = MakeShared<Texture>("Resource/EditorIcons/Transformation.png");

		CameraTexture = MakeShared<Texture>("Resource/EditorIcons/Camera.png");
		FlareTexture = MakeShared<Texture>("Resource/EditorIcons/Flare.png");
	}
}