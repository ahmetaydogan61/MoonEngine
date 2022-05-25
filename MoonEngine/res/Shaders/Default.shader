#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aOffset;
layout(location = 2) in vec3 aSize;
layout(location = 3) in vec4 aColor;

out vec4 fColor;

uniform mat4 u_VP;

void main()
{
    fColor = aColor;
    gl_Position = u_VP * vec4((aPos * aSize) + aOffset, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec4 fColor;

void main()
{
    FragColor = fColor;
}