#version 450

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_uv;

layout (location = 0) out vec4 v_worldPosition;
layout (location = 1) out vec2 v_uv;

layout(set = 0, binding = 0) uniform CameraUniformBuffer {
	mat4 view;
	mat4 projection;
} u_camera;

layout(set = 1, binding = 0) uniform InstanceUniformBuffer {
	mat4 model;
	mat3 normal;
} u_instance;

layout(set = 2, binding = 0) uniform MaterialUniformBuffer {
	vec4 color;
} u_material;

void main(void)
{
	v_worldPosition = u_instance.model * vec4(a_position, 1.0);
	v_uv = a_uv;
	gl_Position = u_camera.projection * u_camera.view * v_worldPosition;
}
