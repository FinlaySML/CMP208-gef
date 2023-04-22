#ifndef _GEF_SKINNED_MESH_SHADER_DATA_H
#define _GEF_SKINNED_MESH_SHADER_DATA_H

#include <maths/matrix44.h>
#include <vector>

namespace gef
{
	class SkinnedMeshShaderData
	{
	public:
		SkinnedMeshShaderData();

		const std::vector<Matrix44>* const bone_matrices() const { return bone_matrices_; }
		
		void set_bone_matrices(const std::vector<Matrix44>* const bone_matrices) { bone_matrices_ = bone_matrices; }

	private:
		const std::vector<Matrix44>* bone_matrices_;
	};
}


#endif // _GEF_SKINNED_MESH_SHADER_DATA_H
