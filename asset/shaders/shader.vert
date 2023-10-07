#version 450
#extension GL_KHR_vulkan_glsl : enable

// Vertex
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in vec4 a_color;
// Instance
layout(location = 4) in mat4 a_worldMatrix;
layout(location = 8) in mat4 a_normalMatrix;
layout(location = 12) in uint a_batchIndex;

// Output
layout (location = 0) out vec4 v_worldPosition;
layout (location = 1) out vec3 v_worldNormal;
layout (location = 2) out vec2 v_uv;
layout (location = 3) out vec4 v_color;
layout (location = 4) out flat uint v_materialID;

layout(set = 0, binding = 0) uniform CameraUniformBuffer {
	mat4 view;
	mat4 projection;
} u_camera;

struct MaterialData
{
	vec4 color;
	uint albedoID;
	uint normalID;
};

layout(std140, set = 1, binding = 0) readonly buffer MaterialDataBuffer {
	MaterialData data[];
} u_material;

// TODO share these struct between C++ & glsl
struct AssetData
{
	uint batchOffset;
	uint batchCount; // valid batches
};

struct BatchData
{
	uint vertexOffset;
	uint indexOffset;
	uint indexCount;

	uint materialIndex;
	// BBOX
	vec4 min;
	vec4 max;
};

layout(std140, set = 3, binding = 0) readonly buffer AssetDataBuffer {
	AssetData data[];
} u_assets;

layout(std140, set = 3, binding = 1) readonly buffer BatchDataBuffer {
	BatchData data[];
} u_batches;

void main(void)
{
	v_worldPosition = a_worldMatrix * vec4(a_position, 1.0);
	v_worldNormal = mat3(a_normalMatrix) * a_normal;
	v_uv = a_uv;
	v_color = a_color;
	v_materialID = u_batches.data[a_batchIndex].materialIndex;
	gl_Position = u_camera.projection * u_camera.view * v_worldPosition;
}
