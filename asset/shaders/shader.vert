#version 450

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_uv;

layout (location = 0) out vec2 v_uv;

layout(binding = 0) uniform UniformBuffer {
	mat4 model;
	mat4 view;
	mat4 projection;
} ubo;

void main(void)
{
	v_uv = a_uv;//a_position.xy * 0.5 + 0.5;
	gl_Position = ubo.projection * ubo.view * ubo.model * vec4(a_position, 1.0);
}
