#version 330 core
layout (location = 0) in vec3 vert;
layout (location = 2) in vec2 uv;

out vec2 v_uv;

uniform float u_time;
// out vec3 v_vertexColors;

void main()
{
	gl_Position = vec4(vert, 1.0);
	// gl_Position.xyz += 0.25 * sin(u_time + vec3(0.0, 1.2, 1.0));
	// gl_Position.z += 0.25 * sin(u_time);
	// v_uv = vert.xy;
	v_uv = vec2(uv.s, uv.t);
}
