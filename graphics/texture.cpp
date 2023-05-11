#include <graphics/texture.h>
#include <graphics/image_data.h>
#include <system/platform.h>
#include <vector>

namespace gef
{
	Texture::Texture()
	{
	}

	Texture::Texture(const class Platform& platform, const ImageData& image_data)
	{
	}

	Texture::~Texture()
	{
	}

	Texture* Texture::CreateCheckerTexture(const Int32 size, const Int32 num_checkers, const Platform& platform)
	{
		const UInt32 check_size = size / num_checkers;
		UInt32* checker_texture = new UInt32[size*size];

		const UInt32 kBlack = 0xff000000;
		const UInt32 kWhite = 0xffffffff;

		for(Int32 y = 0; y < size; ++y)
			for(Int32 x = 0; x < size; ++x)
			{
				if(((y / check_size) % 2) == 0)
				{
					if(((x / check_size) % 2) == 0)
						checker_texture[y*size+x] = kWhite;
					else
						checker_texture[y*size+x] = kBlack;
				}
				else
				{
					if(((x / check_size) % 2) == 0)
						checker_texture[y*size+x] = kBlack;
					else
						checker_texture[y*size+x] = kWhite;
				}
			}

		ImageData image_data;
		image_data.set_image(reinterpret_cast<UInt8*>(checker_texture));
		image_data.set_width(size);
		image_data.set_height(size);
		Texture* texture = gef::Texture::Create(platform, image_data);

		// checker_texture will get freed up when
		// image_data goes out of scope
		return texture;
	}

	Texture* Texture::CreateSolidTexture(const Int32 size, gef::Colour colour, const Platform& platform)
	{
		UInt32* texture_data = new UInt32[size * size];
		UInt32 value = colour.GetABGR();
		for(int i = 0; i < (size*size); i++) texture_data[i] = value;
		ImageData image_data;
		image_data.set_image((UInt8*)texture_data);
		image_data.set_width(size);
		image_data.set_height(size);
		Texture* texture = gef::Texture::Create(platform, image_data);

		// checker_texture will get freed up when
		// image_data goes out of scope
		return texture;
	}
}