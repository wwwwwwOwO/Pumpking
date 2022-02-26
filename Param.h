#pragma once
#define NDEBUG 
#include"model.h"
#include"snow.h"
#include<Glm/glm.hpp> //OpenGL Mathematics ����OpenGL��ɫ���ԣ�GLSL���淶��ͼ�������ͷ�ļ�C ++��ѧ�⡣
#include<Glm/ext.hpp>


/* ����ȫ�ֱ��� */

// ���������ؾ���
glm::mat4 modelMatrix;
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;


// �������
glm::mat4 lightProjection, lightView, ligntModel;
glm::mat4 lightSpaceMatrix;
glm::vec3 lightPos;

float La;	// ������ϵ��
int LightMode;	// ����ģʽ

//�����������ر���
glm::vec3 cameraPos;//�����λ��
glm::vec3 cameraCenter;
glm::vec3 cameraForward;
glm::vec3 cameraTarget;//ԭ��λ��
glm::vec3 cameraDirection;//���������
glm::vec3 cameraRight;//������
glm::vec3 cameraUp;//������
float cameraSpeed = 1.25f; //�ƶ��ٶ�
float fov = 45.0f; //������ӽǣ�ͨ�����������֣��Ӷ��������Ӽ�С�ӽǣ������Ŵ���С��Ч����

// ֡�������
FrameBufferObject* mFBO;

// ��ɫ��
Shader ourShader;
Shader DepthShader;

// ģ��
Model ourHouse;
Model ourSanta;
Model ourCloud;

// ѩ������ϵͳ
Snow::Snow snow;