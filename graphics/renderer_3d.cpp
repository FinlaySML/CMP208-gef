#include <graphics/renderer_3d.h>
#include <graphics/shader.h>
#include <system/platform.h>
#include <graphics/texture.h>

namespace gef
{
	Renderer3D::Renderer3D(Platform& platform) :
		shader_(NULL),
		override_material_(NULL),
		platform_(platform),
		default_shader_(platform),
		default_skinned_mesh_shader_(platform),
		clear_render_target_enabled_(true),
		clear_depth_buffer_enabled_(true),
		clear_stencil_buffer_enabled_(true),
		fov_(0.0f),
		draw_count_{0}
	{
		projection_matrix_.SetIdentity();
		view_matrix_.SetIdentity();
		world_matrix_.SetIdentity();
	}

	Renderer3D::~Renderer3D()
	{
	}

	void Renderer3D::SetShader( Shader* shader)
	{
		if(shader == NULL)
			set_shader(&default_shader_);
		else
			set_shader(shader);
	}

	void Renderer3D::DrawSkinnedMesh(const MeshInstance& mesh_instance, const std::vector<Matrix44>& bone_matrices, bool use_default_shader)
	{
		Shader* previous_shader = shader_;
		if(use_default_shader)
		{
			skinned_data_.set_bone_matrices(&bone_matrices);

			SetShader(&default_skinned_mesh_shader_);

			default_skinned_mesh_shader_.SetSceneData(skinned_data_, light_data_, view_matrix_, projection_matrix_);
		}

		DrawMesh(mesh_instance);

		if(use_default_shader)
			SetShader(previous_shader);
	}

	void Renderer3D::CalculateInverseWorldTransposeMatrix()
	{
		Matrix44 inv_world;
		inv_world.Inverse(world_matrix_);
		inv_world_transpose_matrix_.Transpose(inv_world);
	}

	void Renderer3D::set_world_matrix(const  Matrix44& matrix)
	{
		world_matrix_ = matrix;
		CalculateInverseWorldTransposeMatrix();
	}
}

