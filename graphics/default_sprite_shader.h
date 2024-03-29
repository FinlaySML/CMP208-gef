/*
 * default_sprite_shader.h
 *
 *  Created on: 28 Jan 2015
 *      Author: grant
 */

#ifndef GRAPHICS_DEFAULT_SPRITE_SHADER_H_
#define GRAPHICS_DEFAULT_SPRITE_SHADER_H_

#include <graphics/shader.h>
#include <gef.h>
#include <graphics/shader_interface.h>

namespace gef
{
	class Sprite;
	class Matrix44;
	class Texture;

	class DefaultSpriteShader : public Shader
	{
	public:
		DefaultSpriteShader(const Platform& platform);
		~DefaultSpriteShader();

		void SetSceneData(const Matrix44& projection_matrix);
		void SetSpriteData(const Sprite& sprite, const Texture* texture);
	protected:
		DefaultSpriteShader();
		void BuildSpriteShaderData(const Sprite& sprite, Matrix44& sprite_data);


		gef::ShaderInterface::VVIndex sprite_data_variable_index_;
		gef::ShaderInterface::VVIndex projection_matrix_variable_index_;
		gef::ShaderInterface::TSIndex texture_sampler_index_;
	};
}



#endif /* GRAPHICS_DEFAULT_SPRITE_SHADER_H_ */
