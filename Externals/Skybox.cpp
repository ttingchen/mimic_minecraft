#include "Skybox.h"
#include "Include/STB/stb_image.h"

using namespace std;
using namespace glm;


Skybox::Skybox(const char* vsPath, const char* fsPath)
{
	skyboxProg = new Program(vsPath, fsPath);
	loadCubemap();
}

Skybox::~Skybox()
{
	delete skyboxProg;
};

void Skybox::useProg()
{
	skyboxProg->useProg();
}

GLuint Skybox::loadCubemap()
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	vector<std::string> faces =
	{
		"../Assets/face-r.png",
		"../Assets/face-l.png",
		"../Assets/face-t.png",
		"../Assets/face-bot.png",
		"../Assets/face-fr.png",
		"../Assets/face-ba.png"
	};
	for (unsigned int i = 0; i < 6; i++)
	{
		TextureData data = loadPNG(faces[i].c_str());
		printf("face: %s \n", faces[i].c_str());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGBA, data.width, data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data
		);
		delete[] data.data;
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glGenVertexArrays(1, &vao);

	return textureID;

}
TextureData Skybox::loadPNG(const char* const pngFilepath)
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

void Skybox::render(mat4 &proj, mat4 &view)
{
	skyboxProg->useProg();
	pvm = proj * view;
	//draw skybox
	glBindVertexArray(vao);
	skyboxProg->setMat4("pvm", pvm);

	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glEnable(GL_DEPTH_TEST);
	//end skybox

	skyboxProg->unuseProg();

}