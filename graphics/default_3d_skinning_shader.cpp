/*
 * default_3d_shader.cpp
 *
 *  Created on: 29 Jan 2015
 *      Author: grant
 */

#include <graphics/default_3d_skinning_shader.h>
#include <graphics/shader_interface.h>
#include <graphics/mesh_instance.h>
#include <graphics/primitive.h>
#include <system/debug_log.h>
#include <graphics/mesh.h>
#include <graphics/material.h>
#include <graphics/colour.h>
#include <graphics/skinned_mesh_shader_data.h>
#include <array>

#ifdef _WIN32
#include <platform/d3d11/graphics/shader_interface_d3d11.h>
#endif


namespace gef
{
	Default3DSkinningShader::Default3DSkinningShader(const Platform& platform)
	:Shader(platform)
	,wvp_matrix_variable_index_{0}
	,world_matrix_variable_index_{0}
	,material_colour_variable_index_{0}
	,ambient_light_colour_variable_index_{0}
	,light_data_variable_index_{0}
	,texture_sampler_index_{0}
	,bone_matrices_variable_index_{0}
	{
		// load vertex shader source in from a file
		char* vs_shader_source = NULL;
		Int32 vs_shader_source_length = 0;
		LoadShader("default_3d_skinning_shader_vs", "shaders/gef", &vs_shader_source, vs_shader_source_length, platform);

		char* ps_shader_source = NULL;
		Int32 ps_shader_source_length = 0;
		LoadShader("default_3d_shader_ps", "shaders/gef", &ps_shader_source, ps_shader_source_length, platform);

		device_interface_->SetVertexShaderSource(vs_shader_source, vs_shader_source_length);
		device_interface_->SetPixelShaderSource(ps_shader_source, ps_shader_source_length);

		delete[] vs_shader_source;
		vs_shader_source = NULL;
		delete[] ps_shader_source;
		ps_shader_source = NULL;

		//Vertex Shader Variables
		wvp_matrix_variable_index_ = device_interface_->AddVertexShaderVariable("wvp", gef::ShaderInterface::kMatrix44);
		world_matrix_variable_index_ = device_interface_->AddVertexShaderVariable("world", gef::ShaderInterface::kMatrix44);
		bone_matrices_variable_index_ = device_interface_->AddVertexShaderVariable("bone_matrices", ShaderInterface::kMatrix44, 128);

		//Pixel Shader Variables
		material_colour_variable_index_ = device_interface_->AddPixelShaderVariable("material_colour", gef::ShaderInterface::kVector4);

		//Light Shader Variables
		ambient_light_colour_variable_index_ = device_interface_->AddLightShaderVariable("ambient_light_colour", gef::ShaderInterface::kVector4);
		light_data_variable_index_ = device_interface_->AddLightShaderVariable("lights", gef::ShaderInterface::kLightData, MAX_LIGHTS);

		texture_sampler_index_ = device_interface_->AddTextureSampler("texture_sampler");

		device_interface_->AddVertexParameter("position", ShaderInterface::kVector3, 0, "POSITION", 0);
		device_interface_->AddVertexParameter("normal", ShaderInterface::kVector3, 12, "NORMAL", 0);
		device_interface_->AddVertexParameter("bone_indices", ShaderInterface::kUByte4, 24, "BLENDINDICES", 0);
		device_interface_->AddVertexParameter("bone_weights", ShaderInterface::kVector4, 28, "BLENDWEIGHT", 0);
		device_interface_->AddVertexParameter("uv", ShaderInterface::kVector2, 44, "TEXCOORD", 0);
		device_interface_->set_vertex_size(sizeof(Mesh::SkinnedVertex));
		device_interface_->CreateVertexFormat();

#ifdef _WIN32
		gef::ShaderInterfaceD3D11* shader_interface_d3d11 = static_cast<gef::ShaderInterfaceD3D11*>(device_interface_);

		// Create a texture sampler state description.
		D3D11_SAMPLER_DESC sampler_desc;
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.MipLODBias = 0.0f;
		sampler_desc.MaxAnisotropy = 1;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampler_desc.BorderColor[0] = 0;
		sampler_desc.BorderColor[1] = 0;
		sampler_desc.BorderColor[2] = 0;
		sampler_desc.BorderColor[3] = 0;
		sampler_desc.MinLOD = 0;
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

		shader_interface_d3d11->AddSamplerState(sampler_desc);
#endif

		device_interface_->CreateProgram();
	}

	Default3DSkinningShader::Default3DSkinningShader()
		: wvp_matrix_variable_index_{0}
		, world_matrix_variable_index_{0}
		, material_colour_variable_index_{0}
		, ambient_light_colour_variable_index_{0}
		, light_data_variable_index_{0}
		, texture_sampler_index_{0}
	{
	}

	Default3DSkinningShader::~Default3DSkinningShader()
	{
	}

	void Default3DSkinningShader::SetSceneData(const SkinnedMeshShaderData& shader_data, const LightData& light_data, const Matrix44& view_matrix, const Matrix44& projection_matrix)
	{
		gef::Vector4 ambient_light_colour = light_data.AmbientLightColour().GetRGBAasVector4();
		std::array<gef::LightData::Light, MAX_LIGHTS> shader_lights{};
		int i = 0;
		for (auto& light : light_data.GetLights())
		{
			shader_lights[i] = light.second;
			i++;
			if (i == MAX_LIGHTS) break;
		}
		//A Radius that is -1 is to determine end of lights in shader
		if (i < MAX_LIGHTS) shader_lights[i].radius_ = -1.f;

		view_projection_matrix_ = view_matrix * projection_matrix;

		device_interface_->SetLightShaderVariable(ambient_light_colour_variable_index_, (void*)&ambient_light_colour);
		device_interface_->SetLightShaderVariable(light_data_variable_index_, (void*)shader_lights.data());

		UInt32 k = 0;
		for(const gef::Matrix44& bm : *shader_data.bone_matrices()){
			bones_matrices[i].Transpose(bm);
			k++;
		}

		device_interface_->SetVertexShaderVariable(bone_matrices_variable_index_, bones_matrices, (Int32)shader_data.bone_matrices()->size());
		
	}

	void Default3DSkinningShader::SetMeshData(const gef::MeshInstance& mesh_instance)
	{
		SetMeshData(mesh_instance.transform());
	}

	void Default3DSkinningShader::SetMeshData(const gef::Matrix44& transform)
	{
		gef::Matrix44 wvpT, worldT;
		wvpT.Transpose(transform * view_projection_matrix_);
		worldT.Transpose(transform);

		device_interface_->SetVertexShaderVariable(wvp_matrix_variable_index_, &wvpT);
		device_interface_->SetVertexShaderVariable(world_matrix_variable_index_, &worldT);
	}

	void Default3DSkinningShader::SetMaterialData(const gef::Material* material)
	{
		if (material) {
			primitive_data_.material_texture = material->texture();
			gef::Colour colour{};
			colour.SetFromAGBR(material->colour());
			primitive_data_.material_colour = colour.GetRGBAasVector4();
		}
		else {
			primitive_data_.material_texture = NULL;
			primitive_data_.material_colour = { 1,1,1,1 };
		}

		device_interface_->SetPixelShaderVariable(material_colour_variable_index_, (float*)&primitive_data_.material_colour);
		device_interface_->SetTextureSampler(texture_sampler_index_, primitive_data_.material_texture);
	}

} /* namespace gef */
