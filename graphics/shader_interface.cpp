/*
 * shader_interface.cpp
 *
 *  Created on: 28 Jan 2015
 *      Author: grant
 */

#include <graphics/shader_interface.h>
#include <cstdlib>
#include <cstring>
#include <assert.h>
#include <graphics/light_data.h>

namespace gef
{
	ShaderInterface::ShaderInterface() 
        :	vertex_shader_variable_data_(NULL),
			vertex_shader_variable_data_size_(0),
			pixel_shader_variable_data_(NULL),
			pixel_shader_variable_data_size_(0),
			light_shader_variable_data_(NULL),
			light_shader_variable_data_size_(0),
			vertex_size_(0),
			vs_shader_source_(NULL),
			vs_shader_source_size_(0),
			ps_shader_source_(NULL),
			ps_shader_source_size_(0)
	{
	}
	ShaderInterface::~ShaderInterface()
	{
		free(vertex_shader_variable_data_);
		free(pixel_shader_variable_data_);
		free(light_shader_variable_data_);
		free(vs_shader_source_);
		free(ps_shader_source_);
	}
	ShaderInterface::VVIndex ShaderInterface::AddVertexShaderVariable(const char* variable_name, VariableType variable_type, Int32 variable_count)
	{
		return {AddVariable(vertex_shader_variables_, variable_name, variable_type, variable_count)};
	}

	void ShaderInterface::SetVertexShaderVariable(ShaderInterface::VVIndex variable_index, const void* value, Int32 variable_count)
	{
		SetVariable(vertex_shader_variables_, vertex_shader_variable_data_, variable_index.val_, value, variable_count);
	}

	ShaderInterface::PVIndex ShaderInterface::AddPixelShaderVariable(const char* variable_name, VariableType variable_type, Int32 variable_count)
	{
		return {AddVariable(pixel_shader_variables_, variable_name, variable_type, variable_count)};
	}

	void ShaderInterface::SetPixelShaderVariable(ShaderInterface::PVIndex variable_index, const void* value)
	{
		SetVariable(pixel_shader_variables_, pixel_shader_variable_data_, variable_index.val_, value);
	}

	ShaderInterface::LVIndex ShaderInterface::AddLightShaderVariable(const char* variable_name, VariableType variable_type, Int32 variable_count)
	{
		return {AddVariable(light_shader_variables_, variable_name, variable_type, variable_count)};
	}

	void ShaderInterface::SetLightShaderVariable(ShaderInterface::LVIndex variable_index, const void* value)
	{
		SetVariable(light_shader_variables_, light_shader_variable_data_, variable_index.val_, value);
	}

	UInt32 ShaderInterface::AddVariable(std::vector<ShaderVariable>& variables, const char* variable_name, VariableType variable_type, Int32 variable_count)
	{
		ShaderVariable shader_variable;
		shader_variable.name = variable_name;
		shader_variable.type = variable_type;
		shader_variable.byte_offset = 0;
		shader_variable.count = variable_count;
		variables.push_back(shader_variable);
		return variables.size()-1;
	}

	int RoundUpToNearest(int value, int factor) {
		assert(factor > 0);
		if (factor == 0 || factor == 1) return value;
		int offset = value % factor;
		if (offset == 0) return value;
		return value + factor - offset;
	}

	void ShaderInterface::SetVariable(std::vector<ShaderVariable>& variables, UInt8* variables_data, UInt32 variable_index, const void* value, Int32 variable_count)
	{
		ShaderVariable& shader_variable = variables[variable_index];
		if (variable_count == -1) variable_count = shader_variable.count;
		void* variable_data = &static_cast<UInt8*>(variables_data)[shader_variable.byte_offset];
		Int32 data_size = GetTypeSize(shader_variable.type);
		Int32 block_data_size = RoundUpToNearest(data_size, 16);
		if (variable_count == 1 || data_size == block_data_size) {
			memcpy(variable_data, value, data_size*variable_count);
		}
		else { 
			for (int i = 0; i < variable_count; i++) {
				memcpy(static_cast<std::uint8_t*>(variable_data)+block_data_size*i, static_cast<const std::uint8_t*>(value)+data_size*i, data_size);
			}
		}
	}


	void ShaderInterface::AddVertexParameter(const char* parameter_name, VariableType parameter_type, Int32 parameter_byte_offset, const char* semantic_name, int semantic_index)
	{
		ShaderParameter shader_parameter;
		shader_parameter.name = parameter_name;
		shader_parameter.type = parameter_type;
		shader_parameter.byte_offset = parameter_byte_offset;
		shader_parameter.semantic_name = semantic_name;
		shader_parameter.semantic_index = semantic_index;
		parameters_.push_back(shader_parameter);
	}

	ShaderInterface::TSIndex ShaderInterface::AddTextureSampler(const char* texture_sampler_name)
	{
		TextureSampler texture_sampler;
		texture_sampler.name = texture_sampler_name;
		texture_sampler.texture = NULL;
		texture_samplers_.push_back(texture_sampler);
		return {(UInt32)texture_samplers_.size()-1};
	}

	void ShaderInterface::SetTextureSampler(ShaderInterface::TSIndex texture_sampler_index, const Texture* texture)
	{
		TextureSampler& texture_sampler = texture_samplers_[texture_sampler_index.val_];
		texture_sampler.texture = texture;
	}

	Int32 ShaderInterface::GetTypeSize(VariableType type)
	{
		switch(type)
		{
		case kUByte4:
		case kFloat: return 4;
		case kVector2: return 8;
		case kVector3: return 12;
		case kVector4: return 16;
		case kMatrix44: return 64;
		case kLightData: return sizeof(LightData::Light);
		}
	}

	void ShaderInterface::AllocateVariableData()
	{
		vertex_shader_variable_data_ = AllocateVariableData(vertex_shader_variables_, vertex_shader_variable_data_size_);
		pixel_shader_variable_data_ = AllocateVariableData(pixel_shader_variables_, pixel_shader_variable_data_size_);
		light_shader_variable_data_ = AllocateVariableData(light_shader_variables_, light_shader_variable_data_size_);
	}

	UInt8* ShaderInterface::AllocateVariableData(std::vector<ShaderVariable>& variables, Int32& variable_data_size)
	{
		variable_data_size = 0;
		for(auto& shader_variable : variables)
		{
			int block_pos = variable_data_size % 16;
			int type_size = GetTypeSize(shader_variable.type);
			if (shader_variable.count == 1) {
				if (block_pos + type_size > 16) {
					variable_data_size = RoundUpToNearest(variable_data_size, 16);
				}
				shader_variable.byte_offset = variable_data_size;
				variable_data_size += type_size;
			}
			else {
				//Align to start of next block
				variable_data_size = RoundUpToNearest(variable_data_size, 16);
				shader_variable.byte_offset = variable_data_size;
				variable_data_size += RoundUpToNearest(type_size, 16) * (shader_variable.count - 1) + type_size;
			}
		}
		variable_data_size = RoundUpToNearest(variable_data_size, 16);
		return static_cast<UInt8*>(malloc(variable_data_size));
	}

	void ShaderInterface::SetVertexShaderSource(const char* vs_shader_source, Int32 vs_shader_source_size)
	{
		vs_shader_source_size_ = vs_shader_source_size;
		vs_shader_source_ = new char[vs_shader_source_size];
		memcpy((void*)vs_shader_source_, vs_shader_source, vs_shader_source_size_);
	}

	void ShaderInterface::SetPixelShaderSource(const char* ps_shader_source, Int32 ps_shader_source_size)
	{
		ps_shader_source_size_ = ps_shader_source_size;
		ps_shader_source_ = new char[ps_shader_source_size];
		memcpy((void*)ps_shader_source_, ps_shader_source, ps_shader_source_size_);
	}
}
