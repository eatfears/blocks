#include "Material.h"
#include <libpng/png.h>
#include "Blocks_Definitions.h"
#include "PlatformDefinitions.h"

MaterialLibrary::MaterialLibrary()
{
	texture = 0;
}

MaterialLibrary::~MaterialLibrary()
{
	glDeleteTextures(5, texture);
	free(texture);
}

void MaterialLibrary::AllocGLTextures()
{
	texture = (GLuint *)calloc(5, sizeof(GLuint));
}

void MaterialLibrary::LoadGLTextures()
{
	texture[TERRAIN] = loadImage("textures//terrain.png");
	texture[UNDERWATER] = loadImage("textures//misc//water.png");
	texture[VIGNETTE] = loadImage("textures//misc//vignette.png");
	texture[SUN] = loadImage("textures//terrain//sun.png");
	texture[MOON] = loadImage("textures//terrain//moon.png");
	texture[CLOUDS] = loadImage("textures//environment//clouds.png");
}

int MaterialLibrary::GetTextureInfo(int ColorType)
{
	int ret;
	switch(ColorType)
	{
	case PNG_COLOR_TYPE_GRAY:
		ret = 1;
		break;
	case PNG_COLOR_TYPE_GRAY_ALPHA:
		ret = 2;
		break;
	case PNG_COLOR_TYPE_RGB:
		ret = 3;
		break;
	case PNG_COLOR_TYPE_RGB_ALPHA:
		ret = 4;
		break;
	default:
		ret = -1;//bad
	};
	return ret;
};

GLuint MaterialLibrary::loadImage(const char *filename)
{
	GLuint texture;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep *row_pointers = NULL;
	int bitDepth, ColorType;

	FILE *pngFile;
	b_fopen(&pngFile, filename, "rb");

	if(!pngFile)
		return 0;

	png_byte sig[8];

	fread(&sig, 8, sizeof(png_byte), pngFile);
	rewind(pngFile);
	if(!png_check_sig(sig, 8))
		return 0;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,NULL,NULL);

	if(!png_ptr)
		return 0;

	if(setjmp(png_jmpbuf(png_ptr)))
		return 0;

	info_ptr = png_create_info_struct(png_ptr);

	if(!info_ptr)
		return 0;

	png_init_io(png_ptr, pngFile);

	png_read_info(png_ptr, info_ptr);

	bitDepth = png_get_bit_depth(png_ptr, info_ptr);

	ColorType = png_get_color_type(png_ptr, info_ptr);

	if(ColorType == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	if(ColorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
		png_set_expand_gray_1_2_4_to_8(png_ptr);

	if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	if(bitDepth == 16)
		png_set_strip_16(png_ptr);
	else if(bitDepth < 8)
		png_set_packing(png_ptr);

	png_read_update_info(png_ptr, info_ptr);

	png_uint_32 width, height;
	png_get_IHDR(png_ptr, info_ptr, &width, &height,
		&bitDepth, &ColorType, NULL, NULL, NULL);

	int components = GetTextureInfo(ColorType);

	if(components == -1)
	{
		if(png_ptr)
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return 0;
	}

	GLubyte *pixels = (GLubyte *)malloc(sizeof(GLubyte) * (width * height * components));

	row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);

	for(unsigned int i = 0; i < height; ++i)
		row_pointers[i] = (png_bytep)(pixels + (i * width * components));

	png_read_image(png_ptr, row_pointers);
	png_read_end(png_ptr, NULL);


	// make it
	glGenTextures(1, &texture);
	// bind it
	glBindTexture(GL_TEXTURE_2D, texture);

	// here we has the problems
	GLuint glcolours;
	(components==4) ? (glcolours = GL_RGBA): (0);
	(components==3) ? (glcolours = GL_RGB): (0);
	(components==2) ? (glcolours = GL_LUMINANCE_ALPHA): (0);
	(components==1) ? (glcolours = GL_LUMINANCE): (0);


	//GLubyte a[1000];
	//strcpy_s((char*)a, sizeof(a), (char*)glGetString(GL_VERSION));


	// stretch it
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//OpenGL 1.1
	//	gluBuild2DMipmaps(GL_TEXTURE_2D, components, width, height, glcolours, GL_UNSIGNED_BYTE, pixels);

	//OpenGL 1.4
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);


	//OpenGL 3.0
	//	http://www.opengl.org/wiki/Common_Mistakes#gluBuild2DMipmaps
	//On ATI glEnable(GL_TEXTURE_2D);
	//Warning: It has been reported that on some ATI drivers,
	//glGenerateMipmap(GL_TEXTURE_2D) has no effect unless you
	//precede it with a call to glEnable(GL_TEXTURE_2D) in this particular case.
	//Once again, to be clear, bind the texture, glEnable, then glGenerateMipmap.
	//This is a bug and has been in the ATI drivers for a while.
	//Perhaps by the time you read this, it will have been corrected.
	//(glGenerateMipmap doesn't work on ATI as of 2011)
	// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// 	glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!

	glTexImage2D(GL_TEXTURE_2D, 0, components, width, height, 0, glcolours, GL_UNSIGNED_BYTE, pixels);

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	fclose(pngFile);
	free(row_pointers);
	free(pixels);

	return texture;
}

void MaterialLibrary::GetTextureOffsets(double& offsetx, double& offsety, int material, char covered, int side)
{
	offsetx = 14;
	offsety = 0;

	switch(material)
	{
	case MAT_DIRT: offsetx = 2; offsety = 0;
		if (covered & (1 << SNOWCOVERED))
		{
			if(side == TOP) {offsetx = 2; offsety = 4;}
			else if(side == BOTTOM) {offsetx = 2; offsety = 0;}
			else {offsetx = 4; offsety = 4;}
		}
		else if (covered & (1 << GRASSCOVERED))
		{
			if(side == TOP) {offsetx = 0; offsety = 0;}
			else if(side == BOTTOM) {offsetx = 2; offsety = 0;}
			else {offsetx = 3; offsety = 0;}
		}
		break;
	case MAT_STONE: offsetx = 1; offsety = 0;
		break;
	case MAT_SAND: offsetx = 2; offsety = 1;
		break;
	case MAT_WATER: offsetx = 15; offsety = 13;
		break;
	case MAT_GLASS: offsetx = 1; offsety = 3;
		break;
	case MAT_WOOD: if((side == TOP)||((side == BOTTOM))) {offsetx = 5; offsety = 1;} else {offsetx = 4; offsety = 1;}
		break;
	case MAT_COAL: offsetx = 2; offsety = 2;
		break;
	case MAT_BRICKS: offsetx = 7; offsety = 0;
		break;
	case MAT_PUMPKIN: if((side == TOP)||((side == BOTTOM))) {offsetx = 6; offsety = 6;} else if(side == FRONT) {offsetx = 7; offsety = 7;} else {offsetx = 6; offsety = 7;}
		break;
	case MAT_PUMPKIN_SHINE: if((side == TOP)||((side == BOTTOM))) {offsetx = 6; offsety = 6;} else if(side == FRONT) {offsetx = 8; offsety = 7;} else {offsetx = 6; offsety = 7;}
		break;
	case MAT_TNT: if(side == TOP) {offsetx = 9; offsety = 0;} else if(side == BOTTOM) {offsetx = 10; offsety = 0;} else {offsetx = 8; offsety = 0;}
		break;
	case MAT_PLANK: offsetx = 4; offsety = 0;
		break;
	}

	 offsetx *= 0.0625;
	 offsety *= 0.0625;
}
