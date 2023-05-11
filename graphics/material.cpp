#include "material.h"
#include <stdlib.h>
#include <graphics/colour.h>

namespace gef
{
	Material::Material() : 
		texture_diffuse_{nullptr},
		texture_specular_{nullptr},
		texture_normal_{nullptr},
		ambient_{0,0,0},
		diffuse_{1,1,1,1},
		specular_{0,0,0},
		shininess_{1}
	{
	}
	void Material::SetDiffuse(UInt32 abgr) {
		gef::Colour colour{};
		colour.SetFromAGBR(abgr);
		diffuse_ = colour.GetRGBAasVector4();
	}
}