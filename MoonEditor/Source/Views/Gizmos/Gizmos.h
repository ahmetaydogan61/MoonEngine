#pragma once

namespace MoonEngine
{
	enum class GIZMOSELECTION
	{
		//These are the values from ImGuizmo OPERATION enum
		NONE = -1,
		TRANSLATE = 7,
		RORTATE = 120,
		SCALE = 896,
	};

	struct GizmosData
	{
		GIZMOSELECTION GizmoSelection = GIZMOSELECTION::TRANSLATE;
		float SnapAmount = 0.25f;
		bool IsSnapping = false;
		bool IsUsing = false;
	};

	struct Gizmos
	{

	};
}