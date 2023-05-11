/*
 * default_3d_shader.h
 *
 *  Created on: 29 Jan 2015
 *      Author: grant
 */

#ifndef GRAPHICS_DEFAULT_3D_SHADER_H_
#define GRAPHICS_DEFAULT_3D_SHADER_H_

#include <graphics/shader.h>
#include <gef.h>
#include <maths/vector4.h>
#include <maths/matrix44.h>
#include <graphics/shader_interface.h>

namespace gef
{
	class MeshInstance;
	class Matrix44;
	class Primitive;
	class Texture;
	class Material;
	class LightData;

	class Default3DShader: public Shader
	{
	public:

		struct PrimitiveData
		{
			Vector4 material_colour;
			const gef::Texture* material_texture;
		};

		Default3DShader(const Platform& platform);
		virtual ~Default3DShader();
		void SetSceneData(const LightData& shader_data, const Matrix44& view_matrix, const Matrix44& projection_matrix);
		void SetMeshData(const gef::MeshInstance& mesh_instance);
		void SetMeshData(const gef::Matrix44& transform);
		void SetMaterialData(const gef::Material* material);

		inline PrimitiveData& primitive_data() { return primitive_data_; }
	protected:
		Default3DShader();

		gef::ShaderInterface::VVIndex wvp_matrix_variable_index_;
		gef::ShaderInterface::VVIndex world_matrix_variable_index_;

		gef::ShaderInterface::PVIndex ambient_variable_index_;
		gef::ShaderInterface::PVIndex diffuse_variable_index_;
		gef::ShaderInterface::PVIndex specular_variable_index_;
		gef::ShaderInterface::PVIndex shininess_variable_index_;

		gef::ShaderInterface::LVIndex viewer_position_variable_index_;
		gef::ShaderInterface::LVIndex ambient_light_colour_variable_index_;
		gef::ShaderInterface::LVIndex light_data_variable_index_;

		gef::ShaderInterface::TSIndex diffuse_sampler_index_;
		gef::ShaderInterface::TSIndex specular_sampler_index_;
		gef::ShaderInterface::TSIndex normal_sampler_index_;

		PrimitiveData primitive_data_;

		gef::Matrix44 view_projection_matrix_;

	};

} /* namespace gef */

#endif /* GRAPHICS_DEFAULT_3D_SHADER_H_ */
