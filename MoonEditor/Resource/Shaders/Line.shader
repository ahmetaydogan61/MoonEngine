#Vertex
#version 450 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in int aEntityId;

out vec4 fColor;
flat out int fEntityId;

uniform mat4 uVP;

void main()
{
	gl_Position = uVP * vec4(aPosition.x, aPosition.y, aPosition.z, 1.0);

	fColor = aColor;
	fEntityId = aEntityId;
}

#Fragment
#version 450 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int EntityId;

in vec4 fColor;
flat in int fEntityId;

void main()
{
	FragColor = fColor;
	EntityId = fEntityId;
}
