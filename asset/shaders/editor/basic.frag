#version 450

layout(location = 0) in vec3 v_normal;
layout(location = 1) in vec3 v_forward;
layout(location = 2) in vec3 v_color;

layout(location = 0) out vec4 o_color;

void main()
{
	float costheta = dot(normalize(v_forward), normalize(v_normal)) * 0.5 + 0.5;
	o_color = vec4(v_color * costheta, 1.0);
}
