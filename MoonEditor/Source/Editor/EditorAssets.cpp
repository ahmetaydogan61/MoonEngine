#include "mpch.h"
#include "EditorAssets.h"

namespace MoonEngine
{
	Shared<Texture> EditorAssets::PlayTexture, EditorAssets::StopTexture, EditorAssets::PauseTexture;
	Shared<Texture> EditorAssets::SelectTexture, EditorAssets::TranslateTexture, EditorAssets::ResizeTexture, EditorAssets::RotateTexture;

	void EditorAssets::LoadTextures()
	{
		PlayTexture = MakeShared<Texture>("Resource/EditorIcons/Play.png");
		StopTexture = MakeShared<Texture>("Resource/EditorIcons/Stop.png");
		PauseTexture = MakeShared<Texture>("Resource/EditorIcons/Pause.png");

		SelectTexture = MakeShared<Texture>("Resource/EditorIcons/Select.png");
		TranslateTexture = MakeShared<Texture>("Resource/EditorIcons/Translate.png");
		ResizeTexture = MakeShared<Texture>("Resource/EditorIcons/Resize.png");
		RotateTexture = MakeShared<Texture>("Resource/EditorIcons/Rotate.png");
	}
}