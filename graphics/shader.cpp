#include <graphics/shader.h>
#include <system/platform.h>
#include <system/debug_log.h>
#include <graphics/shader_interface.h>
#include <graphics/mesh_instance.h>
#include <system/file.h>
#include <system/memory_stream_buffer.h>
#include <istream>
#include <iterator>
#include <cstring>

namespace gef
{
	Shader::Shader(const Platform& platform)
		:device_interface_(NULL)
	{
		device_interface_ = gef::ShaderInterface::Create(platform);
	}

	Shader::Shader() :
		device_interface_(NULL)
	{
	}

	Shader::~Shader()
	{
		delete device_interface_;
	}

	//void Shader::SetData(const void* data)
	//{
	//}

	void Shader::SetVertexFormat()
	{
	}

	void Shader::SetMeshData(const gef::MeshInstance& mesh_instance)
	{
		SetMeshData(mesh_instance.transform());
	}

	void Shader::SetMeshData(const gef::Matrix44& transform)
	{

	}


	void Shader::SetMaterialData(const gef::Material* material)
	{

	}
}
