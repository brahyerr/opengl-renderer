#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 vertexColors;

out vec3 v_vertexColors;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    v_vertexColors = vertexColors;
}
