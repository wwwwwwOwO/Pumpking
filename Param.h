#pragma once
#define NDEBUG 
#include"model.h"
#include"snow.h"
#include<Glm/glm.hpp> //OpenGL Mathematics 基于OpenGL着色语言（GLSL）规范的图形软件的头文件C ++数学库。
#include<Glm/ext.hpp>


/* 定义全局变量 */

// 定义相机相关矩阵
glm::mat4 modelMatrix;
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;


// 光照相关
glm::mat4 lightProjection, lightView, ligntModel;
glm::mat4 lightSpaceMatrix;
glm::vec3 lightPos;

float La;	// 环境光系数
int LightMode;	// 光照模式

//定义摄像机相关变量
glm::vec3 cameraPos;//摄像机位置
glm::vec3 cameraCenter;
glm::vec3 cameraForward;
glm::vec3 cameraTarget;//原点位置
glm::vec3 cameraDirection;//摄像机朝向
glm::vec3 cameraRight;//右向量
glm::vec3 cameraUp;//上向量
float cameraSpeed = 1.25f; //移动速度
float fov = 45.0f; //摄像机视角，通过监听鼠标滚轮，从而触发增加减小视角，产生放大缩小的效果。

// 帧缓冲对象
FrameBufferObject* mFBO;

// 着色器
Shader ourShader;
Shader DepthShader;

// 模型
Model ourHouse;
Model ourSanta;
Model ourCloud;

// 雪花粒子系统
Snow::Snow snow;