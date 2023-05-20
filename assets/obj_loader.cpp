#include <assets/obj_loader.h>
#include <vector>
#include <maths/vector4.h>
#include <maths/vector2.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <system/platform.h>
#include <graphics/model.h>
#include <assets/png_loader.h>
#include <graphics/texture.h>
#include <graphics/image_data.h>
#include <system/file.h>
#include <system/memory_stream_buffer.h>
#include <graphics/material.h>
#include <system/debug_log.h>

#include <cstdio>
#include <string>
#include <istream>
#include <cfloat>
#include <sstream>
#include <unordered_map>
#include <cassert>
#include <unordered_set>

namespace gef
{


bool OBJLoader::Load(const std::string& filename, Platform& platform, Model& model)
{
	//Vertex data
	std::vector<gef::Vector4> positions;
	std::vector<gef::Vector4> normals;
	std::vector<gef::Vector2> uvs;
	//9 indices for each triangle
	std::vector<Int32> face_indices;
	std::vector<Int32> primitive_start_indices;
	std::vector<std::string> primitive_materials;
	//Filename to Texture object (so we don't load the same texture twice)
	std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
	std::unordered_map<std::string, OBJMaterial> obj_materials;

	std::unique_ptr<gef::File> file{gef::File::Create()};
	if(!file->Open(filename.c_str())) return false;
	Int32 file_size = 0;
	if(!file->GetSize(file_size)) return false;
	std::vector<char> obj_file_data(file_size, 0);
	Int32 bytes_read;
	if(!file->Read(obj_file_data.data(), file_size, bytes_read)) return false;
	if(bytes_read != file_size) return false;
	file->Close();

	gef::MemoryStreamBuffer buffer(obj_file_data.data(), file_size);
	std::istream file_stream(&buffer);
	{
		std::string line;
		while (std::getline(file_stream, line)) {
			std::istringstream line_stream(line);
			std::string keyword;
			line_stream >> keyword;
			if (keyword == "mtllib")
			{
				std::string material_filename;
				line_stream >> material_filename;
				LoadMaterials(platform, material_filename, obj_materials);
			}

			// vertices
			else if (keyword == "v")
			{
				float x, y, z;
				line_stream >> x;
				line_stream >> y;
				line_stream >> z;
				positions.push_back(gef::Vector4(x, y, z));
			}

			// normals
			else if (keyword == "vn" )
			{
				float nx, ny, nz;
				line_stream >> nx;
				line_stream >> ny;
				line_stream >> nz;
				normals.push_back(gef::Vector4(nx, ny, nz));
			}

			// uvs
			else if (keyword == "vt" )
			{
				float u, v;
				line_stream >> u;
				line_stream >> v;
				uvs.push_back(gef::Vector2(u, v));
			}

			else if(keyword == "usemtl")
			{

				std::string material_name;
				line_stream >> material_name;

				// any time the material is changed
				// a new primitive is created
				primitive_start_indices.push_back((Int32)face_indices.size());
				primitive_materials.push_back(material_name);
			}
			else if (keyword == "f")
			{
				Int32 vertexIndex[3], uvIndex[3], normalIndex[3];

				line_stream >> vertexIndex[0]; line_stream.ignore(); line_stream >> uvIndex[0]; line_stream.ignore(); line_stream >> normalIndex[0];
				line_stream >> vertexIndex[1]; line_stream.ignore(); line_stream >> uvIndex[1]; line_stream.ignore(); line_stream >> normalIndex[1];
				line_stream >> vertexIndex[2]; line_stream.ignore(); line_stream >> uvIndex[2]; line_stream.ignore(); line_stream >> normalIndex[2];

				face_indices.push_back(vertexIndex[2]);
				face_indices.push_back(uvIndex[2]);
				face_indices.push_back(normalIndex[2]);

				face_indices.push_back(vertexIndex[1]);
				face_indices.push_back(uvIndex[1]);
				face_indices.push_back(normalIndex[1]);

				face_indices.push_back(vertexIndex[0]);
				face_indices.push_back(uvIndex[0]);
				face_indices.push_back(normalIndex[0]);
			}
		}
		// finished reading the file
		// start building the mesh
		Int32 num_faces = (Int32)face_indices.size() / 9;
		Int32 num_vertices = num_faces*3;
		// create vertex buffer
		std::vector<gef::Mesh::Vertex> vertices(num_vertices);

		// need to record min and max position values for mesh bounds
		gef::Vector4 pos_min(FLT_MAX, FLT_MAX, FLT_MAX), pos_max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		for(Int32 vertex_num = 0; vertex_num < num_vertices; ++vertex_num)
		{
			gef::Mesh::Vertex* vertex = &vertices[vertex_num];
			gef::Vector4 position = positions[face_indices[vertex_num*3]-1];
			gef::Vector2 uv = uvs[face_indices[vertex_num*3+1]-1];
			gef::Vector4 normal = normals[face_indices[vertex_num*3+2]-1];

			vertex->px = position.x();
			vertex->py = position.y();
			vertex->pz = position.z();
			vertex->nx = normal.x();
			vertex->ny = normal.y();
			vertex->nz = normal.z();
			vertex->u = uv.x;
			vertex->v = -uv.y;

			// update min and max positions for bounds
			if (position.x() < pos_min.x())
				pos_min.set_x(position.x());
			if (position.y() < pos_min.y())
				pos_min.set_y(position.y());
			if (position.z() < pos_min.z())
				pos_min.set_z(position.z());
			if (position.x() > pos_max.x())
				pos_max.set_x(position.x());
			if (position.y() > pos_max.y())
				pos_max.set_y(position.y());
			if (position.z() > pos_max.z())
				pos_max.set_z(position.z());
		}


		std::unique_ptr<Mesh> mesh(new Mesh(platform));

		// Set bounds
		gef::Aabb aabb(pos_min, pos_max);
		gef::Sphere sphere(aabb);
		mesh->set_aabb(aabb);
		mesh->set_bounding_sphere(sphere);
		std::unordered_map<std::string, Texture*> loaded_texture;
		std::unordered_map<std::string, Material*> loaded_material;

		mesh->InitVertexBuffer(platform, vertices.data(), vertices.size(), sizeof(gef::Mesh::Vertex));

		// create primitives
		mesh->AllocatePrimitives(primitive_start_indices.size());
		
		std::vector<std::vector<UInt32>> indices(primitive_start_indices.size());
		for(UInt32 primitive_num=0;primitive_num<primitive_start_indices.size();++primitive_num)
		{
			Int32 index_count = 0;

			if(primitive_num == primitive_start_indices.size()-1)
				index_count = (Int32)face_indices.size() - primitive_start_indices[primitive_num];
			else
				index_count = primitive_start_indices[primitive_num+1] - primitive_start_indices[primitive_num];

			// 9 indices per triangle, index count is the number of vertices in this primitive
			index_count /= 3;

			indices[primitive_num].resize(index_count);

			for(Int32 index=0;index<index_count;++index)
				indices[primitive_num][index] = primitive_start_indices[primitive_num]+index;

			mesh->GetPrimitive(primitive_num)->set_type(gef::TRIANGLE_LIST);
			mesh->GetPrimitive(primitive_num)->InitIndexBuffer(platform, indices[primitive_num].data(), index_count, sizeof(UInt32));

			//Find material

			//1. Use existing if it has already been created
			//2. Else create material
			//3. If it has a diffuse texture:
			//4. 	Use existing texture if it has already been created
			//5.    Else load texture from file
			//6. Copy over other material parameters
			std::string material_name = primitive_materials[primitive_num];
			auto lm_it = loaded_material.find(material_name);
			if(lm_it != loaded_material.end()){
				mesh->GetPrimitive(primitive_num)->set_material(lm_it->second);
			}else{
				auto om_it = obj_materials.find(material_name);
				if(om_it != obj_materials.end()) {
					Material* new_mat = new Material();
					{//Get Diffuse Texture
						std::string name = om_it->second.diffuse_texture_;
						auto lt_it = loaded_texture.find(name);
						if(lt_it != loaded_texture.end()){
							new_mat->texture_diffuse_ = lt_it->second;
						}else if(!name.empty()){
							Texture* new_texture = gef::Texture::Create(platform, {name.c_str()});
							loaded_texture.insert({name, new_texture });
							new_mat->texture_diffuse_ = new_texture;
						}
					}
					{//Get Specular Texture
						std::string name = om_it->second.specular_texture_;
						auto lt_it = loaded_texture.find(name);
						if (lt_it != loaded_texture.end()) {
							new_mat->texture_specular_ = lt_it->second;
						}
						else if (!name.empty()) {
							Texture* new_texture = gef::Texture::Create(platform, { name.c_str() });
							loaded_texture.insert({ name, new_texture });
							new_mat->texture_specular_ = new_texture;
						}
					}
					{//Get Normal Texture
						std::string name = om_it->second.normal_texture_;
						auto lt_it = loaded_texture.find(name);
						if (lt_it != loaded_texture.end()) {
							new_mat->texture_normal_ = lt_it->second;
						}
						else if (!name.empty()) {
							Texture* new_texture = gef::Texture::Create(platform, { name.c_str() });
							loaded_texture.insert({ name, new_texture });
							new_mat->texture_normal_ = new_texture;
						}
					}
					//Other parameters
					new_mat->ambient_ = om_it->second.ambient_;
					new_mat->diffuse_ = om_it->second.diffuse_;
					new_mat->specular_ = om_it->second.specular_;
					new_mat->shininess_ = om_it->second.shininess_;
					mesh->GetPrimitive(primitive_num)->set_material(new_mat);
				}else{
					gef::DebugOut(("No material '"+material_name+"' found while loading model '"+filename+"'\n").c_str());
				}
			}
		}
		for (auto tex : loaded_texture) model.AddTexture(std::unique_ptr<Texture>{tex.second});
		for (auto mat : loaded_material) model.AddMaterial(std::unique_ptr<Material>{mat.second});
		model.SetMesh(std::move(mesh));
	}
#ifdef _DEBUG
	gef::DebugOut(("Loaded: " + filename + "\n").c_str());
#endif
	return true;
}

bool OBJLoader::LoadMaterials(Platform& platform, const std::string& filename, std::unordered_map<std::string, OBJMaterial>& materials)
{
	std::unique_ptr<gef::File> file{gef::File::Create()};
	if(!file->Open(filename.c_str())) return false;
	Int32 file_size = 0;
	if(!file->GetSize(file_size)) return false;
	std::vector<char> mtl_file_data(file_size, 0);
	Int32 bytes_read;
	if(!file->Read(mtl_file_data.data(), file_size, bytes_read)) return false;
	if(bytes_read != file_size) return false;

	gef::MemoryStreamBuffer buffer(mtl_file_data.data(), file_size);
	std::istream file_stream(&buffer);
	
	OBJMaterial* current{nullptr};
	std::string line;
	while( std::getline(file_stream, line) )
	{
		std::istringstream line_stream(line);
		std::string keyword;
		line_stream >> keyword;
		if (keyword == "newmtl"  )
		{
			std::string material_name;
			line_stream >> material_name;
			materials[material_name] = OBJMaterial();
			current = &materials[material_name];
		}
		else if (keyword == "Ka"){
			assert(current);
			float r, g, b;
			line_stream >> r;
			line_stream >> g;
			line_stream >> b;
			current->ambient_ = {r,g,b, 1};
		}
		else if (keyword == "Kd") {
			assert(current);
			float r, g, b;
			line_stream >> r;
			line_stream >> g;
			line_stream >> b;
			current->diffuse_ = { r,g,b,1 };
		}
		else if (keyword == "Ks") {
			assert(current);
			float r, g, b;
			line_stream >> r;
			line_stream >> g;
			line_stream >> b;
			current->specular_ = { r,g,b,1 };
		}
		else if (keyword == "Ns") {
			assert(current);
			line_stream >> current->shininess_;
		}
		else if (keyword == "map_Ka") {
			assert(current);
			line_stream >> current->ambient_texture_;
		}
		else if(keyword == "map_Kd") {
			assert(current);
			line_stream >> current->diffuse_texture_;
		}
		else if (keyword == "map_Ks") {
			assert(current);
			line_stream >> current->specular_texture_;
		}
		else if (keyword == "norm") {
			assert(current);
			line_stream >> current->normal_texture_;
		}
	}
#ifdef _DEBUG
	gef::DebugOut(("Loaded: "+filename+"\n").c_str());
#endif
	return true;
}

OBJLoader::OBJMaterial::OBJMaterial() :
ambient_{0,0,0},
diffuse_{1,1,1},
specular_{0,0,0},
shininess_{1}
{
}

}
