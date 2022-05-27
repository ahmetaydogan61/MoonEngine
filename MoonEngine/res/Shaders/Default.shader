#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aOffset;
layout(location = 2) in vec3 aSize;
layout(location = 3) in vec4 aColor;
layout(location = 4) in vec2 aTexCoords;
layout(location = 5) in float aTexIndex;

out vec4 fColor;
out vec2 fTexCoords;
out float fTexIndex;

uniform mat4 u_VP;

void main()
{
    fColor = aColor;
    fTexCoords = aTexCoords;
    fTexIndex = aTexIndex;
    gl_Position = u_VP * vec4((aPos * aSize) + aOffset, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec4 fColor;
in vec2 fTexCoords;
in float fTexIndex;

uniform sampler2D u_Texture[8];

void main()
{
    int textureIndex = int(fTexIndex);
    FragColor = texture(u_Texture[textureIndex], fTexCoords) * fColor;
}