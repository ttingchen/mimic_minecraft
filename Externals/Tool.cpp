#include "Tool.h"
#include "Include/STB/stb_image.h"

using namespace std;
using namespace glm;

GLuint loadTexture(const char* const pngFilepath)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	TextureData data = loadPNG(pngFilepath);
	printf("face: %s \n", pngFilepath);
	glTexImage2D(GL_TEXTURE_2D,
		0, GL_RGBA, data.width, data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data
	);
	delete[] data.data;

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	return textureID;

}
TextureData loadPNG(const char* const pngFilepath)
{
	TextureData texture;
	int components;

	stbi_uc *data = stbi_load(pngFilepath, &texture.width, &texture.height, &components, 4);

	if (data != NULL)
	{
		// copy the raw data
		size_t dataSize = texture.width * texture.height * 4 * sizeof(unsigned char);
		texture.data = new unsigned char[dataSize];
		memcpy(texture.data, data, dataSize);

		// release the loaded image
		stbi_image_free(data);
	}
	else {
		printf("Picture not found.\n");
	}

	return texture;

}
