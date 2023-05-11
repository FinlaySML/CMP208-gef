#ifndef _GEF_OBJ_LOADER_H
#define _GEF_OBJ_LOADER_H

#include <gef.h>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <graphics/texture.h>
#include <graphics/material.h>
#include <unordered_map>

namespace gef
{
	class Platform;
	class Model;

	class OBJLoader
	{
	public:
		bool Load(const std::string& filename, Platform& platform, Model& model);
	private:
		//OBJ uses Phong shading model
		struct OBJMaterial {
			OBJMaterial();
			gef::Vector4 ambient_;//Ka r g b
			gef::Vector4 diffuse_;//Kd r g b
			gef::Vector4 specular_;//Ks r g b
			float shininess_;//Ns x
			std::string ambient_texture_;//map_Ka path (not used by shader)
			std::string diffuse_texture_;//map_Kd path 
			std::string specular_texture_;//map_Ks path (not used by shader)
			std::string normal_texture_;
		};
		bool LoadMaterials(Platform& platform, const std::string& filename, std::unordered_map<std::string, OBJMaterial>& materials);
	};
}


#endif // _OBJ_LOADER_H