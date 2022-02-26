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

	//���������������
#define MAX_LAUNCH (4.0f*10.0f)
	//����������С����
#define MIN_LAUNCH (0.4f*10.0f)
	//�㾫���С
#define MAX_SIZE 10.0f
#define MIN_SIZE 2.0f
	//����ٶ�
#define MAX_VELOC glm::vec3(50.0,0.0,50.0)
	//��С�ٶ�
#define MIN_VELOC glm::vec3(-50.0,-50.0,-50.0)
	//�������ӷ���ϵͳ����������
#define MAX_PARTICLES 5000
	//��ʼ��������������
#define INIT_PARTICLES 2000
	//��ѩ�뾶
	const float areaLength = 100.0f;
	const float fallHeight = 400.0f;

	struct SnowParticle
	{
		float type;
		glm::vec3 position;
		glm::vec3 velocity;
		float lifetime;//����
		float size;//���ӵ㾫���С
	};

	class Snow
	{
	public:
		Snow();
		~Snow();
		void Draw(float frametime, glm::mat4& worldMatrix, glm::mat4 viewMatrix, glm::mat4& projectMatrix);
		bool InitSnow();
	private:
		void UpdateParticles(float frametime);		// �������ӵ�λ�õ�
		void InitRandomTexture(unsigned int size);	// ����һά�������
		void RenderParticles(glm::mat4& worldMatrix, glm::mat4& viewMatrix, glm::mat4& projectMatrix);	// ��Ⱦ����
		void GenInitLocation(SnowParticle partciles[], int nums);	// ���ɳ�ʼ����

		unsigned int mCurVBOIndex, mCurTransformFeedbackIndex;	// ���ڼ�¼����TransformFeedback�ĸ���
		GLuint mParticleBuffers[2]; // ���ӷ���ϵͳ���������㻺����
		GLuint mParticleArrays[2];	
		GLuint mTransformFeedbacks[2];	// ���ӷ���ϵͳ��Ӧ��TransformFeedback
		GLuint mRandomTexture;	// ���һά����
		Texture mSparkTexture;	// ѩ����������
		float mTimer;	// ���ӷ������Ѿ������ʱ��
		bool mFirst;
		Shader* mUpdateShader;//�������ӵ�GPUProgram
		Shader* mRenderShader;//��Ⱦ���ӵ�GPUProgram
	};
}
