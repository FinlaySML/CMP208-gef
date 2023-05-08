#ifndef _MATERIAL_H
#define _MATERIAL_H

#include <gef.h>
#include <maths/vector4.h>

namespace gef
{
	class Texture;

	class Material
	{
	public:
		Material();
		void SetDiffuse(UInt32 abgr);
		Texture* texture_;
		gef::Vector4 ambient_;
		gef::Vector4 diffuse_;
		gef::Vector4 specular_;
		float shininess_;
	};
}

#endif // _MATERIAL_H