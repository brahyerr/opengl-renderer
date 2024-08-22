#version 330 core
// #extension GL_ARB_separate_shader_objects : require

in vec2 v_uv;
in vec3 v_normal;
in vec3 v_fragPos;

// layout (location = 0) in vec2 v_uv;
// layout (location = 1) in vec3 v_normal;
// layout (location = 2) in vec2 v_fragPos;

out vec4 f_color;

uniform float u_time;
uniform float u_time2;
uniform sampler2D u_tex; // this is a built-in function
uniform sampler2D u_tex2;

uniform vec3 u_lightSourcePos;
uniform vec3 u_lightColor;
uniform vec3 u_objectColor;

void main()
{
	float ambientStrength = 0.3;

	// textures and trig shenanigans
	// vec3 color = 0.5 + 0.5 * sin(u_time2 * 0.5 + v_uv.xyx + vec3(0.0, 2.75, 4.0));
	// color.r *= 0.3;
	// color.g *= 0.3;
	// color.b *= 0.3;
	// f_uv.x += 0.2 * cos(u_time * 0.5 + v_uv.y);
	// f_uv.x += 0.1;
	// f_uv.y += 1.0 * sin(u_time * 0.5 + v_uv.x);
	// f_color = mix(texture(u_tex, f_uv), texture(u_tex2, f_uv), 0.0) + vec4(u_objectColor * u_lightColor, 1.0);
	// f_color = mix(texture(u_tex, f_uv), texture(u_tex2, f_uv), 0.0) + vec4(color, 1.0);
	// f_color = texture(u_tex, v_uv + 0.1 * sin(u_time + v_uv.yx + vec2(0.0, 4.0)));
	// f_color = texture(u_tex, v_uv + 0.1 * sin(u_time + v_uv.yx + vec2(0.0, 4.0))) * vec4(u_lightColor, 1.0);

	vec2 uv = v_uv;
	uv.x += 0.2 * cos(u_time * 0.5 + v_uv.y);
	uv.x += 0.1;
	uv.y += 1.0 * sin(u_time * 0.5 + v_uv.x);

	vec3 normal = normalize(v_normal);
	vec3 lightDir = normalize(u_lightSourcePos - v_fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * u_lightColor;
	vec3 ambient = ambientStrength * u_lightColor;
	vec3 result = (ambient + diffuse) * u_objectColor;
	
	f_color = mix(texture(u_tex, uv), texture(u_tex2, uv), 0.0) * vec4(result, 1.0);
	// f_color = vec4(result, 1.0);
}
