#version 330 core

in vec3 v_vertexColors;

out vec4 FragColor;

void main()
{
    // FragColor = vec4(0.2f, 1.0f, 0.8f, 1.0f);
    FragColor = vec4(v_vertexColors.r, v_vertexColors.g, v_vertexColors.b, 1.0f);
}
