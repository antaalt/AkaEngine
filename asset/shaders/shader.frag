#version 450

layout(location = 0) in vec2 v_uv;

layout(location = 0) out vec4 o_color;

layout(binding = 1) uniform sampler2D u_texture;

void main()
{
	o_color = vec4(v_uv, 1, 1);
	o_color = texture(u_texture, v_uv);
}
