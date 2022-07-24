#shader vertex
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in float aTexID;

out vec4 fColor;
out vec2 fTexCoords;
out float fTexID;

uniform mat4 uVP;

void main()
{
	fColor = aColor;
	fTexCoords = aTexCoords;
	fTexID = aTexID;
	
	gl_Position = uVP * vec4(aPosition, 1.0);
};

#shader fragment
#version 330 core
out vec4 FragColor;

in vec4 fColor;
in vec2 fTexCoords;
in float fTexID;

uniform sampler2D uTexture[32];

void main()
{
	int index = int(fTexID);
	FragColor = texture(uTexture[index], fTexCoords) * fColor;
};