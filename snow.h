#pragma once 

#include <GL/glew.h>
#include <iostream>
#include <ctime>
#include <math.h>
#include <Glm/glm.hpp>
#include "model.h"

namespace Snow {

#define PARTICLE_TYPE_LAUNCHER 0.0f
#define PARTICLE_TYPE_SHELL 1.0f

	//发射粒子最大周期
#define MAX_LAUNCH (4.0f*10.0f)
	//发射粒子最小周期
#define MIN_LAUNCH (0.4f*10.0f)
	//点精灵大小
#define MAX_SIZE 10.0f
#define MIN_SIZE 2.0f
	//最大速度
#define MAX_VELOC glm::vec3(50.0,0.0,50.0)
	//最小速度
#define MIN_VELOC glm::vec3(-50.0,-50.0,-50.0)
	//定义粒子发射系统最大的粒子数
#define MAX_PARTICLES 5000
	//初始发射器粒子数量
#define INIT_PARTICLES 2000
	//落雪半径
	const float areaLength = 100.0f;
	const float fallHeight = 400.0f;

	struct SnowParticle
	{
		float type;
		glm::vec3 position;
		glm::vec3 velocity;
		float lifetime;//年龄
		float size;//粒子点精灵大小
	};

	class Snow
	{
	public:
		Snow();
		~Snow();
		void Draw(float frametime, glm::mat4& worldMatrix, glm::mat4 viewMatrix, glm::mat4& projectMatrix);
		bool InitSnow();
	private:
		void UpdateParticles(float frametime);		// 更新粒子的位置等
		void InitRandomTexture(unsigned int size);	// 生成一维随机纹理
		void RenderParticles(glm::mat4& worldMatrix, glm::mat4& viewMatrix, glm::mat4& projectMatrix);	// 渲染粒子
		void GenInitLocation(SnowParticle partciles[], int nums);	// 生成初始粒子

		unsigned int mCurVBOIndex, mCurTransformFeedbackIndex;	// 用于记录两个TransformFeedback的更替
		GLuint mParticleBuffers[2]; // 粒子发射系统的两个顶点缓存区
		GLuint mParticleArrays[2];	
		GLuint mTransformFeedbacks[2];	// 粒子发射系统对应的TransformFeedback
		GLuint mRandomTexture;	// 随机一维纹理
		Texture mSparkTexture;	// 雪花粒子纹理
		float mTimer;	// 粒子发射器已经发射的时间
		bool mFirst;
		Shader* mUpdateShader;//更新粒子的GPUProgram
		Shader* mRenderShader;//渲染粒子的GPUProgram
	};
}
