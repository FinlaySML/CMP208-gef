#include <maths/aabb.h>
#include <gef.h>
#include <cfloat>

namespace gef
{
	Aabb::Aabb() :
		min_vtx_(FLT_MAX, FLT_MAX, FLT_MAX),
		max_vtx_(-FLT_MAX, -FLT_MAX, -FLT_MAX)
	{
	}

	Aabb::Aabb(const Vector4& min_vtx, const Vector4& max_vtx)
	{
		min_vtx_ = min_vtx;
		max_vtx_ = max_vtx;
	}

	void Aabb::Update(const Vector4& point)
	{
		if(point.x() < min_vtx_.x())
			min_vtx_.set_x(point.x());
		if(point.y() < min_vtx_.y())
			min_vtx_.set_y(point.y());
		if(point.z() < min_vtx_.z())
			min_vtx_.set_z(point.z());
		if(point.x() > max_vtx_.x())
			max_vtx_.set_x(point.x());
		if(point.y() > max_vtx_.y())
			max_vtx_.set_y(point.y());
		if(point.z() > max_vtx_.z())
			max_vtx_.set_z(point.z());
	}

	const Aabb Aabb::Transform(const Matrix44& transform_matrix) const
	{
		// This can be optimised
		Aabb result;
		for(gef::Vector4& vertex : GetCorners()) {
			vertex.set_w(1);
			gef::Vector4 tformed = vertex.TransformW(transform_matrix);
			float factor = 1.f/tformed.w();
			tformed *= factor;
			tformed.set_w(1);
			result.Update(tformed);
		}

		return result;
	}

	std::vector<gef::Vector4> Aabb::GetCorners() const {
		std::vector<gef::Vector4> vertices(8);
		for(int i = 0; i < 8; i++){
			vertices[i].set_x((i & 0b001) ? min_vtx_.x() : max_vtx_.x());
			vertices[i].set_y((i & 0b010) ? min_vtx_.y() : max_vtx_.y());
			vertices[i].set_z((i & 0b100) ? min_vtx_.z() : max_vtx_.z());
			vertices[i].set_w(1);
		}
		return vertices;
	}

}