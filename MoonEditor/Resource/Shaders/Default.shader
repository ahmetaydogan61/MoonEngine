#Vertex
#version 450 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in int aTexId;
layout(location = 4) in vec2 aTiling;
layout(location = 5) in int aEntityId;

out vec4 fColor;
out vec2 fTexCoord;
flat out int fTexId;
flat out vec2 fTiling;
flat out int fEntityId;

uniform mat4 uVP;

void main()
{
	gl_Position = uVP * vec4(aPosition, 1.0);

	fColor = aColor;
	fTexCoord = aTexCoord;
	fTexId = aTexId;
	fTiling = aTiling;
	fEntityId = aEntityId;
}

#Fragment
#version 450 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int EntityId;

in vec4 fColor;
in vec2 fTexCoord;
flat in int fTexId;
flat in vec2 fTiling;
flat in int fEntityId;

uniform sampler2D uTexture[32];

void main()
{
	vec4 color = texture(uTexture[fTexId], fTexCoord * fTiling) * fColor;

	if (color.a == 0.0)
		discard;

	FragColor = color;
	EntityId = fEntityId;
}
