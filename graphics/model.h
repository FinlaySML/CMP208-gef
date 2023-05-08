#ifndef _GEF_MODEL_H
#define _GEF_MODEL_H

#include <gef.h>
#include <vector>
#include <memory>
#include <graphics/texture.h>
#include <graphics/mesh.h>
#include <graphics/material.h>

namespace gef
{
	class Model
	{
	public:
		void SetMesh(std::unique_ptr<gef::Mesh> mesh);
		Mesh* GetMesh();
		void AddTexture(std::unique_ptr<Texture> textures);
		void AddMaterial(std::unique_ptr<Material> material);
	private:
		std::unique_ptr<gef::Mesh> mesh_;
		std::vector<std::unique_ptr<Texture>> textures_;
		std::vector<std::unique_ptr<gef::Material>> materials_;
	};
}

#endif // _GEF_MODEL_H
