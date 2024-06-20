#version 330 core

in vec2 v_uv;
out vec4 f_color;

uniform float u_time;
uniform sampler2D u_tex; // this is a built-in function

void main()
{
	vec3 color = 0.2 + 0.5 * sin(u_time * 0.5 + v_uv.xyx + vec3(0.0, 2.75, 4.0));
	color.r *= 0.6;
	color.g *= 0.2;
	color.b *= 0.3;

	vec2 f_uv = v_uv;
	f_uv.x += 0.05 * cos(u_time * 0.25 + v_uv.y);
	// f_uv.x += 0.1;
	f_uv.y += 0.3 * sin(u_time * 0.25 + v_uv.x);
	f_color = texture(u_tex, f_uv) * vec4(color, 1.0);
	// f_color = texture(u_tex, v_uv + 0.1 * sin(u_time + v_uv.yx + vec2(0.0, 4.0))) * vec4(color, 1.0);
}