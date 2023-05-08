#include <graphics/model.h>
#include <vector>
#include <maths/vector4.h>
#include <maths/vector2.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <system/platform.h>
#include <graphics/texture.h>


namespace gef
{
	void Model::SetMesh(std::unique_ptr<gef::Mesh> mesh)
	{
		mesh_ = std::move(mesh);
	}
	Mesh* Model::GetMesh()
	{
		return mesh_.get();
	}
	void Model::AddTexture(std::unique_ptr<Texture> texture)
	{
		textures_.push_back(std::move(texture));
	}
	void Model::AddMaterial(std::unique_ptr<gef::Material> material)
	{
		materials_.push_back(std::move(material));
	}
}