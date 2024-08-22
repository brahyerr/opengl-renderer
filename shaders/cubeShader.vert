#version 330 core
// #extension GL_ARB_separate_shader_objects : require

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

// layout (location = 0) out vec2 v_uv;
// layout (location = 1) out vec3 v_normal;
// layout (location = 2) out vec3 v_fragPos;

out vec2 v_uv;
out vec3 v_normal;
out vec3 v_fragPos;

uniform float u_time;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform mat3 u_normal;

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(vert, 1.0);
	// gl_Position.xyz += 0.25 * sin(u_time + vec3(0.0, 1.2, 1.0));
	// gl_Position.z += 0.25 * sin(u_time);
	// v_uv = vert.xy;
	v_uv = vec2(uv.s, uv.t);
	v_normal = u_normal * normal;
	v_fragPos = vec3(u_model * vec4(vert, 1.0));
}
