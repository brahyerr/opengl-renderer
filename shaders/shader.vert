#version 330 core
layout (location = 0) in vec3 vert;
layout (location = 1) in vec2 uv;

out vec2 v_uv;

uniform float u_time;
// uniform mat4 u_trans;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(vert, 1.0);
	// gl_Position.xyz += 0.25 * sin(u_time + vec3(0.0, 1.2, 1.0));
	// gl_Position.z += 0.25 * sin(u_time);
	// v_uv = vert.xy;
	v_uv = vec2(uv.s, uv.t);
}
