#ifndef _GEF_RENDERER_3D_H
#define _GEF_RENDERER_3D_H

#include <gef.h>
#include <maths/matrix44.h>
#include <graphics/light_data.h>
#include <graphics/skinned_mesh_shader_data.h>
#include <graphics/default_3d_shader.h>
#include <graphics/default_3d_skinning_shader.h>
#include <vector>
#include <graphics/primitive.h>

namespace gef
{
	class Platform;
	class MeshInstance;
	class Shader;
	class Material;
	class Texture;
	class Mesh;

	class Skeleton;


	class Renderer3D
	{
	public:
		enum FillMode
		{
			kSolid = 0,
			kWireframe,
			kLines
		};

		enum DepthTest
		{
			kLessEqual=0,
			kAlways
		};

		virtual ~Renderer3D();
	//	virtual void ClearZBuffer() = 0;
		virtual void Begin(bool clear = true) = 0;
		virtual void End() = 0;
		virtual void DrawMesh(const  MeshInstance& mesh_instance) = 0;
		virtual void DrawMesh(const Mesh& mesh, const gef::Matrix44& matrix, bool lit=true) = 0;

//		virtual void DrawPrimitive(const  MeshInstance& mesh_instance, Int32 primitive_index, Int32 num_indices = -1) = 0;
		virtual void SetFillMode(FillMode fill_mode) = 0;
		virtual void SetDepthTest(DepthTest depth_test) = 0;
		void DrawSkinnedMesh(const  MeshInstance& mesh_instance, const std::vector<Matrix44>& bone_matrices, bool use_default_shader = true);
		void SetShader( Shader* shader);
		virtual void SetPrimitiveType(gef::PrimitiveType type) = 0;
		virtual void DrawPrimitive(const IndexBuffer* index_buffer, int num_indices) = 0;



		inline  Shader* shader() const { return shader_; }
		inline const Matrix44& view_matrix() const { return view_matrix_; }
		inline void set_view_matrix(const  Matrix44& matrix) {view_matrix_ = matrix;}
		inline const Matrix44& projection_matrix() const { return projection_matrix_; }
		inline void set_projection_matrix(const  Matrix44& matrix) {projection_matrix_ = matrix;}
		inline const Matrix44& world_matrix() const { return world_matrix_; }
		void set_world_matrix(const  Matrix44& matrix);
		inline const Matrix44& inv_world_transpose_matrix() const { return inv_world_transpose_matrix_; }

		inline  const Platform& platform() const {return platform_;}
		inline LightData& default_shader_data() { return light_data_; }
		inline void set_override_material(const Material* material) { override_material_ = material; }
		inline const Material* override_material() const { return override_material_; }

		static Renderer3D* Create(Platform& platform);
		inline bool clear_render_target_enabled() const { return clear_render_target_enabled_; }
		inline void set_clear_render_target_enabled(bool val) { clear_render_target_enabled_ = val; }
		inline bool clear_depth_buffer_enabled() const { return clear_depth_buffer_enabled_; }
		inline void set_clear_depth_buffer_enabled(bool val) { clear_depth_buffer_enabled_ = val; }
		inline bool clear_stencil_buffer_enabled() const { return clear_stencil_buffer_enabled_; }
		inline void set_clear_stencil_buffer_enabled(bool val) { clear_stencil_buffer_enabled_ = val; }
		inline float fov() const { return fov_; }
		inline void set_fov(float val) { fov_ = val; }
		int GetAndResetDrawCount() {
			int value = draw_count_;
			draw_count_ = 0;
			return value;
		};
	protected:
		Renderer3D(Platform& platform);
		void CalculateInverseWorldTransposeMatrix();
		inline void set_shader( Shader* shader) { shader_ = shader; }

		Matrix44 projection_matrix_;
		Matrix44 view_matrix_;
		Matrix44 inv_world_transpose_matrix_;
		Matrix44 world_matrix_;
		Shader* shader_;
		Default3DShader default_shader_;
		Default3DSkinningShader default_skinned_mesh_shader_;
		LightData light_data_;
		LightData full_bright_light_data_;
		SkinnedMeshShaderData skinned_data_;
		const Material* override_material_;

		Platform& platform_;

		bool clear_render_target_enabled_;
		bool clear_depth_buffer_enabled_;
		bool clear_stencil_buffer_enabled_;

		float fov_;
		int draw_count_;
	};
}
#endif // _GEF_RENDERER_3D_H
