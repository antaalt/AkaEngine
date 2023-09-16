#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec4 v_worldPosition;
layout(location = 1) in vec3 v_worldNormal;
layout(location = 2) in vec2 v_uv;
layout(location = 3) in vec4 v_color;

layout(location = 0) out vec4 o_color;


layout(set = 2, binding = 0) uniform MaterialUniformBuffer {
	vec4 color;
} u_material;

layout(set = 2, binding = 1) uniform sampler2D u_albedoTexture;
layout(set = 2, binding = 2) uniform sampler2D u_normalTexture;


void main()
{
	vec3 sunDir = vec3(0, 1, 0);
	float cosTheta = dot(sunDir, v_worldNormal);

	vec4 albedo = texture(u_albedoTexture, v_uv) * u_material.color * v_color;
	vec4 ambientLight = albedo * 0.2;
	vec4 sunLight = albedo * cosTheta;
	o_color = ambientLight + sunLight;
}
