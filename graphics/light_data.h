#ifndef _GEF_DEFAULT_3D_SHADER_DATA_H
#define _GEF_DEFAULT_3D_SHADER_DATA_H

#include <graphics/colour.h>
#include <vector>
#include <graphics/point_light.h>
#include <unordered_map>

constexpr int MAX_LIGHTS = 512;

namespace gef
{
	class LightData
	{
	public:
		//Must conform to hlsl packing rules
		struct Light {
			gef::Vector4 position_;
			gef::Vector4 direction_;
			gef::Vector4 colour_;
			float radius_;
			float fall_off_;
			float angle_;
			float scatter_;
			Light(gef::Vector4 position, gef::Vector4 direction, gef::Colour colour, float radius, float fall_off, float angle, float scatter);
			Light();
		};
		static_assert(sizeof(Light) == 16 * 4, "LightData::Light is the wrong size for HLSL!");

		LightData();
		UInt64 AddLight(const Light& point_light);
		void RemoveLight(UInt64);
		const Light& GetLight(const UInt64 light_num) const;
		Light& GetLight(const UInt64 light_num);
		const std::unordered_map<UInt64, Light>& GetLights() const;
		const Colour& AmbientLightColour() const;
		void SetAmbientLightColour(const Colour& colour);
		void ClearLights();
	private:
		Colour ambient_light_colour_;
		std::unordered_map<UInt64, Light> lights_;
		UInt64 next_light_id_;
	};
}
#endif // _GEF_DEFAULT_3D_SHADER_H

