#pragma once

#include <Aka/Core/Geometry.h>

namespace aka {

struct Camera
{
	virtual mat4f perspective() const = 0;
};

struct CameraPerspective : Camera
{
	CameraPerspective() {}
	CameraPerspective(const vec2f& viewport, anglef hFov, float nearZ = 0.1f, float farZ = 100.f);

	mat4f perspective() const override;

	vec2f viewport;
	anglef hFov;
	float nearZ, farZ;
};

struct CameraOrthographic : Camera
{
	CameraOrthographic() {}
	CameraOrthographic(const vec2f& viewport);

	mat4f perspective() const override;

	vec2f viewport;
};

};