#include "scene/EditorScene.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "cube_shader_f.h"
#include "cube_shader_v.h"
#include "cube_tex.h"

struct Vertex
{
	float x;
	float y;
	float z;
	int16_t u;
	int16_t v;
	static void init()
	{
		ms_layout
			.begin()
			.add( bgfx::Attrib::Position, 3, bgfx::AttribType::Float )
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Int16, true, true)
			.end();
	}
	static bgfx::VertexLayout ms_layout;
};

bgfx::VertexLayout Vertex::ms_layout;

static Vertex s_cubeVertices[] =
{
	{-25.0f,  25.0f,  25.0f,      0,      0 },
	{ 25.0f,  25.0f,  25.0f, 0x7fff,      0 },
	{-25.0f, -25.0f,  25.0f,      0, 0x7fff },
	{ 25.0f, -25.0f,  25.0f, 0x7fff, 0x7fff },
	{-25.0f,  25.0f, -25.0f,      0,      0 },
	{ 25.0f,  25.0f, -25.0f, 0x7fff,      0 },
	{-25.0f, -25.0f, -25.0f,      0, 0x7fff },
	{ 25.0f, -25.0f, -25.0f, 0x7fff, 0x7fff },
	{-25.0f,  25.0f,  25.0f,      0,      0 },
	{ 25.0f,  25.0f,  25.0f, 0x7fff,      0 },
	{-25.0f,  25.0f, -25.0f,      0, 0x7fff },
	{ 25.0f,  25.0f, -25.0f, 0x7fff, 0x7fff },
	{-25.0f, -25.0f,  25.0f,      0,      0 },
	{ 25.0f, -25.0f,  25.0f, 0x7fff,      0 },
	{-25.0f, -25.0f, -25.0f,      0, 0x7fff },
	{ 25.0f, -25.0f, -25.0f, 0x7fff, 0x7fff },
	{ 25.0f, -25.0f,  25.0f,      0,      0 },
	{ 25.0f,  25.0f,  25.0f, 0x7fff,      0 },
	{ 25.0f, -25.0f, -25.0f,      0, 0x7fff },
	{ 25.0f,  25.0f, -25.0f, 0x7fff, 0x7fff },
	{-25.0f, -25.0f,  25.0f,      0,      0 },
	{-25.0f,  25.0f,  25.0f, 0x7fff,      0 },
	{-25.0f, -25.0f, -25.0f,      0, 0x7fff },
	{-25.0f,  25.0f, -25.0f, 0x7fff, 0x7fff },
};

static const uint16_t s_cubeTriList[] = { 
	0,  2,  1,
	1,  2,  3,
	4,  5,  6,
	5,  7,  6,

	8, 10,  9,
	9, 10, 11,
	12, 13, 14,
	13, 15, 14,

	16, 18, 17,
	17, 18, 19,
	20, 21, 22,
	21, 23, 22,
};


LModelManager::LModelManager(){}
LModelManager::~LModelManager(){}

std::shared_ptr<glm::mat4> LModelManager::addInstance(glm::mat4 transform){
	std::shared_ptr<glm::mat4> modelInstance = std::make_shared<glm::mat4>(transform);
	
	mInstanceData.push_back(modelInstance);
	return modelInstance;
}

void LModelManager::generateInstanceBuffer(){
	if(mInstanceData.size() == 0) return;
	uint32_t drawableInstances = bgfx::getAvailInstanceDataBuffer(mInstanceData.size(), mInstanceStride);
	bgfx::allocInstanceDataBuffer(&mModelInstances, drawableInstances, mInstanceStride);

	uint8_t* instanceData = mModelInstances.data;

	for (auto& instance : mInstanceData) {
		memcpy(instanceData, &(instance.get())[0][0], mInstanceStride);
		instanceData += mInstanceStride;
	}
}


void LModelManager::render(){
	// this can't be implemented until after bin loading.
}

void LCubeManager::render(){
	if(mInstanceData.size() == 0 || mModelInstances.data == nullptr) return;
	for(auto instance : mInstanceData){
		bgfx::setVertexBuffer(0, mCubeVbh);
		bgfx::setIndexBuffer(mCubeIbh);
	
		bgfx::setTexture(0, mCubeTexUniform, mCubeTexture);
	
	    uint64_t  _state = 0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_MSAA;

		bgfx::setState( _state );

		bgfx::setTransform(&(instance.get())[0][0]);
		bgfx::submit(0, mCubeShader);
	}

}

LCubeManager::LCubeManager(){}

void LCubeManager::init(){
	/*
		TODO: System Agnostic Paths. Embed cube shaders using header output option of shaderc? 
	*/
    Vertex::init();

	bgfx::ShaderHandle vs = bgfx::createShader(bgfx::makeRef(cube_shader_v, cube_shader_v_len));
	bgfx::ShaderHandle fs = bgfx::createShader(bgfx::makeRef(cube_shader_f, cube_shader_f_len));
	mCubeShader = bgfx::createProgram(vs, fs, true);

	//mCubeShader = bigg::loadProgram("shaders/cube_shader_v.bin", "shaders/cube_shader_f.bin");

	mCubeVbh = bgfx::createVertexBuffer(bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)), Vertex::ms_layout );
	mCubeIbh = bgfx::createIndexBuffer(bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList)));
	
	int x, y, n;
	uint8_t* data = stbi_load_from_memory(&cube_png[0], cube_png_size, &x, &y, &n, 4);
	
	mCubeTexture = bgfx::createTexture2D((uint16_t)x, (uint16_t)y, false, 1, bgfx::TextureFormat::RGBA8, 0, bgfx::copy(data, x*y*4));
	
	stbi_image_free(data);

	mCubeTexUniform = bgfx::createUniform("s_texColor",  bgfx::UniformType::Sampler);
}

LCubeManager::~LCubeManager(){ 
	bgfx::destroy(mCubeShader);
	bgfx::destroy(mCubeVbh);
	bgfx::destroy(mCubeIbh);
	bgfx::destroy(mCubeTexture);
	bgfx::destroy(mCubeTexUniform);
}


LEditorScene::LEditorScene() : Initialized(false) {}

LEditorScene::~LEditorScene(){}

void LEditorScene::init(){
	Initialized = true;
	mCubeManager.init();

}

glm::mat4 LEditorScene::getCameraView(){
	return mCamera.GetViewMatrix();
}

glm::mat4 LEditorScene::getCameraProj(){
	return mCamera.GetProjectionMatrix();
}

std::shared_ptr<glm::mat4> LEditorScene::InstanceModel(std::string name, glm::mat4 transform){
	if (mSceneModels.count(name) == 0){
		return mCubeManager.addInstance(transform);
	} else {
		return mSceneModels[name].addInstance(transform);
	}
}

void LEditorScene::RenderSubmit(uint32_t m_width, uint32_t m_height){
	mCamera.SetAspectRatio(m_width, m_height);

	glm::mat4 view = mCamera.GetViewMatrix();
	glm::mat4 proj = mCamera.GetProjectionMatrix();

	bgfx::setViewTransform(0, &view[0][0], &proj[0][0]);
    bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));
    bgfx::touch(0);
    
	mCubeManager.render();
}

void LEditorScene::update(GLFWwindow* window, float dt)
{
	mCamera.Update(window, dt);
}