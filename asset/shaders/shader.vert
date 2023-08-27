#version 450

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_uv;

layout (location = 0) out vec2 v_uv;

layout(set = 0, binding = 0) uniform CameraUniformBuffer {
	mat4 view;
	mat4 projection;
} camera;

layout(set = 1, binding = 0) uniform InstanceUniformBuffer {
	mat4 model;
} instance;

layout(set = 2, binding = 0) uniform MaterialUniformBuffer {
	vec4 color;
} material;

void main(void)
{
	v_uv = a_uv;//a_position.xy * 0.5 + 0.5;
	gl_Position = camera.projection * camera.view * instance.model * vec4(a_position, 1.0);
}
