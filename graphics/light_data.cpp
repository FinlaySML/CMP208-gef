#include <graphics/light_data.h>

namespace gef
{
	LightData::Light::Light(gef::Vector4 position, gef::Vector4 direction, gef::Colour colour, float radius, float fall_off, float angle, float scatter) : 
	position_{position},
	direction_{direction},
	colour_{colour.GetABGRasVector4()},
	radius_{radius},
	fall_off_{fall_off},
	angle_{angle},
	scatter_{scatter}
	{
	}

	LightData::Light::Light() : Light{{0,0,0},{1,0,0},{1,1,1},5,1,180,0.2f}
	{
	}
	
	LightData::LightData() : ambient_light_colour_{1.0,1.0,1.0}, next_light_id_{1}
	{
	}
	UInt64 LightData::AddLight(const Light& point_light)
	{
		lights_[next_light_id_] = point_light;
		next_light_id_++;
		return next_light_id_-1;
	}
	void LightData::RemoveLight(UInt64 key)
	{
		lights_.erase(key);
	}
	const LightData::Light& LightData::GetLight(const UInt64 key) const
	{
		return lights_.at(key);
	}
	LightData::Light& LightData::GetLight(const UInt64 key)
	{
		return lights_.at(key);
	}
	const std::unordered_map<UInt64, LightData::Light>& LightData::GetLights() const
	{
		return lights_;
	}
	const Colour& LightData::AmbientLightColour() const
	{
		return ambient_light_colour_;
	}
	void LightData::SetAmbientLightColour(const Colour& colour)
	{
		ambient_light_colour_ = colour;
	}
}
