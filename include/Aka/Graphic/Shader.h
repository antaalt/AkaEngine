#pragma once

#include <vector>

#include "../Core/Geometry.h"
#include "../Core/StrictType.h"
#include "Texture.h"

namespace aka {

enum class ShaderType {
	Vertex,
	Fragment,
	Compute,
	Geometry,
	TesselationControl,
	TesselationEvaluation,
};

enum class UniformType {
	None,
	Float,
	Int,
	UnsignedInt,
	Vec2,
	Vec3,
	Vec4,
	Mat3,
	Mat4,
	Image2D,
	Texture2D,
	Texture2DMultisample,
	TextureCubemap,
	Sampler2D,
	SamplerCube,
	SamplerBuffer
};

using ShaderID = StrictType<uintptr_t, struct ShaderStrictType>;
using ProgramID = StrictType<uintptr_t, struct ProgramStrictType>;
using UniformID = StrictType<uintptr_t, struct UniformStrictType>;
using AttributeID = StrictType<uint32_t, struct UniformStrictType>;

struct Uniform {
	UniformID id;
	UniformType type; // type of uniform
	ShaderType shaderType; // attached shader
	uint32_t bufferIndex;
	uint32_t arrayLength;
	std::string name; // name of uniform
};

struct Attributes {
	AttributeID id;
	std::string name;
};

class Shader
{
public:
	using Ptr = std::shared_ptr<Shader>;
protected:
	Shader(const std::vector<Attributes>& attributes);
	Shader(const Shader&) = delete;
	const Shader& operator=(const Shader&) = delete;
	virtual ~Shader();
public:
	static ShaderID compile(const std::string &content, ShaderType type);
	static ShaderID compile(const char* content, ShaderType type);

	static Shader::Ptr create(ShaderID vert, ShaderID frag, ShaderID compute, const std::vector<Attributes>& attributes);

	bool valid() const { return m_valid; }
protected:
	std::vector<Attributes> m_attributes;
	bool m_valid;
};



}