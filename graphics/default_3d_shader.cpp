/*
 * default_3d_shader.cpp
 *
 *  Created on: 29 Jan 2015
 *      Author: grant
 */

#include <graphics/default_3d_shader.h>
#include <graphics/shader_interface.h>
#include <graphics/mesh_instance.h>
#include <graphics/primitive.h>
#include <system/debug_log.h>
#include <graphics/mesh.h>
#include <graphics/material.h>
#include <graphics/colour.h>
#include <graphics/light_data.h>
#include <array>

#ifdef _WIN32
#include <platform/d3d11/graphics/shader_interface_d3d11.h>
#endif


namespace gef
{
	Default3DShader::Default3DShader(const Platform& platform)
	:Shader(platform)
	,wvp_matrix_variable_index_{0}
	,world_matrix_variable_index_{0}
	,ambient_variable_index_{0}
	,diffuse_variable_index_{0}
	,specular_variable_index_{0}
	,shininess_variable_index_{0}
	,ambient_light_colour_variable_index_{0}
	,light_data_variable_index_{0}
	,diffuse_sampler_index_{0}
	,specular_sampler_index_{0}
	,normal_sampler_index_{0}
	{
		// Compile shaders
		device_interface_->SetVertexShaderPath(L"default_3d_shader_vs", L"shaders/gef", platform);
		device_interface_->SetPixelShaderPath(L"default_3d_shader_ps", L"shaders/gef", platform);

		// Vertex Shader
		wvp_matrix_variable_index_ = device_interface_->AddVertexShaderVariable("wvp", ShaderInterface::kMatrix44);
		world_matrix_variable_index_ = device_interface_->AddVertexShaderVariable("world", ShaderInterface::kMatrix44);

		// Pixel Shader
		ambient_variable_index_ = device_interface_->AddPixelShaderVariable("ambient", ShaderInterface::kVector4);
		diffuse_variable_index_ = device_interface_->AddPixelShaderVariable("diffuse", ShaderInterface::kVector4);
		specular_variable_index_ = device_interface_->AddPixelShaderVariable("specular", ShaderInterface::kVector4);
		shininess_variable_index_ = device_interface_->AddPixelShaderVariable("shininess", ShaderInterface::kFloat);

		//Light Shader Variables
		viewer_position_variable_index_ = device_interface_->AddLightShaderVariable("viewer_position", gef::ShaderInterface::kVector4);
		ambient_light_colour_variable_index_ = device_interface_->AddLightShaderVariable("ambient_light_colour", gef::ShaderInterface::kVector4);
		light_data_variable_index_ = device_interface_->AddLightShaderVariable("lights", gef::ShaderInterface::kLightData, MAX_LIGHTS);

		diffuse_sampler_index_ = device_interface_->AddTextureSampler("diffuse_sampler", gef::ShaderInterface::TextureType::DIFFUSE);
		specular_sampler_index_ = device_interface_->AddTextureSampler("specular_sampler", gef::ShaderInterface::TextureType::SPECULAR);
		normal_sampler_index_ = device_interface_->AddTextureSampler("normal_sampler", gef::ShaderInterface::TextureType::NORMAL);

		device_interface_->AddVertexParameter("position", ShaderInterface::kVector3, 0, "POSITION", 0);
		device_interface_->AddVertexParameter("normal", ShaderInterface::kVector3, 12, "NORMAL", 0);
		device_interface_->AddVertexParameter("uv", ShaderInterface::kVector2, 24, "TEXCOORD", 0);
		device_interface_->set_vertex_size(sizeof(Mesh::Vertex));
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

	Default3DShader::Default3DShader()
		: wvp_matrix_variable_index_{0}
		, world_matrix_variable_index_{0}
		, ambient_variable_index_{ 0 }
		, diffuse_variable_index_{ 0 }
		, specular_variable_index_{ 0 }
		, shininess_variable_index_{ 0 }
		, ambient_light_colour_variable_index_{0}
		, light_data_variable_index_{0}
		, diffuse_sampler_index_{0}
		, specular_sampler_index_{0}
		, normal_sampler_index_{0}
	{

	}

	Default3DShader::~Default3DShader()
	{
	}



	void Default3DShader::SetSceneData(const LightData& shader_data, const Matrix44& view_matrix, const Matrix44& projection_matrix)
	{
		//gef::Matrix44 wvp = world_matrix * view_matrix * projection_matrix;
		gef::Vector4 ambient_light_colour = shader_data.AmbientLightColour().GetRGBAasVector4();
		std::array<gef::LightData::Light, MAX_LIGHTS> shader_lights{};
		int i = 0;
		for (auto& light : shader_data.GetLights())
		{
			shader_lights[i] = light.second;
			i++;
			if (i == MAX_LIGHTS) break;
		}
		//A Radius that is -1 is to determine end of lights in shader
		if (i < MAX_LIGHTS) shader_lights[i].radius_ = -1.f;

		view_projection_matrix_ = view_matrix * projection_matrix;
		gef::Matrix44 inverse_vp{};
		inverse_vp.Inverse(view_projection_matrix_);
		gef::Vector4 viewer_position{ inverse_vp.GetRow(3) };
		viewer_position *= 1/viewer_position.w();
		viewer_position.set_w(1);
		device_interface_->SetLightShaderVariable(viewer_position_variable_index_, (void*)&viewer_position);
		device_interface_->SetLightShaderVariable(ambient_light_colour_variable_index_, (void*)&ambient_light_colour);
		device_interface_->SetLightShaderVariable(light_data_variable_index_, (void*)shader_lights.data());
	}

	void Default3DShader::SetMeshData(const gef::MeshInstance& mesh_instance)
	{
		SetMeshData(mesh_instance.transform());
	}

	void Default3DShader::SetMeshData(const gef::Matrix44& transform)
	{
		gef::Matrix44 wvpT, worldT;
		wvpT.Transpose(transform * view_projection_matrix_);
		worldT.Transpose(transform);

		device_interface_->SetVertexShaderVariable(wvp_matrix_variable_index_, &wvpT);
		device_interface_->SetVertexShaderVariable(world_matrix_variable_index_, &worldT);
	}

	void Default3DShader::SetMaterialData(const gef::Material* material)
	{
		Material default_material{};

		const Material* mat = (material != nullptr) ? material : &default_material;
		device_interface_->SetPixelShaderVariable(ambient_variable_index_, &mat->ambient_);
		device_interface_->SetPixelShaderVariable(diffuse_variable_index_, &mat->diffuse_);
		device_interface_->SetPixelShaderVariable(specular_variable_index_, &mat->specular_);
		device_interface_->SetPixelShaderVariable(shininess_variable_index_, &mat->shininess_);
		device_interface_->SetTextureSampler(diffuse_sampler_index_, mat->texture_diffuse_);
		device_interface_->SetTextureSampler(specular_sampler_index_, mat->texture_specular_);
		device_interface_->SetTextureSampler(normal_sampler_index_, mat->texture_normal_);
	}

} /* namespace gef */
