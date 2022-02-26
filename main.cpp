#define STB_IMAGE_IMPLEMENTATION
#include "FBO.h"
#include "Param.h"
#include "skybox.h"
#include "snow.h"
#include <vector>
#define W 800.0f
#define H 600.0f
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "assimp-vc142-mt.lib")


bool bCameraRotate = false;			//  摄像机是否允许旋转
float cameraXZRotateAngle = 0;		// 摄像机xz平面内的旋转角度 即左右旋转
float cameraYRotateAngle = 0;		//  摄像机垂直平面内的旋转角度   上下旋转
int lastMousePosX, lastMousePosY;	//  上次鼠标的位置  

const glm::vec3 cameraPosInit = glm::vec3(-437.0f, 408.0f, 977.5f);		// 初始摄像机位置
const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);	//定义上向量 用于叉乘
const glm::vec3 cameraTargetInit = glm::vec3(-249.570038f, 299.751007f, 599.132141f);	// 初始相机朝向
const glm::vec3 lightPosInit = glm::vec3(0.0f, 430.0f, 450.0f);	// 太阳位置初始值
const glm::vec3 lightAxis = glm::vec3(0.0f, 0.0f, 1.0f);	// 太阳旋转轴
const float LaInit = 0.18f;	// 环境光系数
const float skyScale = 2000.0f;	// 天空盒大小

void DrawHouse();	// 绘制南瓜小屋
void DrawShadow();	// 用于生成阴影
void DrawSnow();	// 绘制雪花
void DrawSanta();	// 绘制秃头宝贝
void DrawCloud();	// 绘制云朵
void DrawSkybox();	// 绘制天空盒


void init()
{
	//创建着色器读取模型=============================================================================
	ourShader.init("Res/Base.vs", "Res/Base.fs");
	DepthShader.init("Res/Depth.vs", "Res/Depth.fs");
	ourHouse.loadModel("Res/christmas-pumpkin/christmas-pumpkin.obj");
	snow.InitSnow();
	initSkybox();

	//初始化相机，得到视角矩阵===========================================================================
	cameraPos = cameraPosInit;		//初始化摄像机的位置(x,y,z)
	cameraTarget = cameraTargetInit;	//摄像机朝向
	cameraCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	
	//定义相机的3个轴：朝向、右轴、上轴
	cameraDirection = glm::normalize(cameraTarget - cameraPos);	//摄像机朝向

	cameraRight = glm::normalize(glm::cross(up, cameraDirection));	//右轴
	cameraUp = glm::cross(cameraDirection, cameraRight);	//上轴
	cameraSpeed = 2.5f;	//设置摄像机速度
	viewMatrix = glm::lookAt(cameraPos, cameraTarget, up);
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f, 2.0f, 2.0f));	// it's a bit too big for our scene, so scale it down

	//=======================================================================================
	La = LaInit;		// 设置环境光
	LightMode = 0;		// 光照模型
	lightPos = lightPosInit;
	ligntModel = glm::mat4(1.0f);
	lightProjection = glm::ortho(-10000.0f, 10000.0f, -10000.0f, 10000.0f, 1.0f, 10000.0f);
	lightView = glm::lookAt(lightPos, cameraCenter, glm::vec3(1.0f, 0.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;


	glEnable(GL_DEPTH_TEST);// 深度测试
	glEnable(GLUT_MULTISAMPLE); // 多重采样，实现抗锯齿效果

	// 创建一个帧缓冲对象
	mFBO = new FrameBufferObject;												//FBO.h
	mFBO->AttachColorBuffer("color", GL_COLOR_ATTACHMENT0, W, H);			//绑定一个colorbuffer，800,600是屏幕宽高
	mFBO->AttachDepthBuffer("depth", W, H);									//绑定一个depthbuffer，800,600是屏幕宽高
	mFBO->Finish();
}


void display()
{	
	// 用于得到深度图
	mFBO->Bind();
	DrawShadow();
	mFBO->Unbind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 绘制
	DrawSkybox();
	DrawHouse();
	DrawSnow();
	DrawSanta();
	DrawCloud();
	
	glutSwapBuffers();
	glutPostRedisplay();
}


void keyFunc(GLubyte key, int x, int y)          // 键盘交互函数，   ws移动摄像机   c切换方案，  l开关灯
{
	cameraRight = glm::normalize(glm::cross(up, cameraDirection));//根据朝向改变右轴
	switch (key)
	{
		//摄像机移动------------------------------------------------------
	case 'w': case 'W'://前移
		cameraPos += cameraSpeed * cameraDirection;
		cameraTarget += cameraSpeed * cameraDirection;
		break;
	case 's': case 'S'://后移
		cameraPos -= cameraSpeed * cameraDirection;
		cameraTarget -= cameraSpeed * cameraDirection;
		break;
	case 'a': case 'A'://左移
		cameraPos += cameraSpeed * cameraRight;
		cameraTarget += cameraSpeed * cameraRight;
		break;
	case 'd': case 'D'://右移
		cameraPos -= cameraSpeed * cameraRight;
		cameraTarget -= cameraSpeed * cameraRight;
		break;
	case 'r': case 'R'://飞天
		cameraPos += cameraSpeed * up;
		cameraTarget += cameraSpeed * up;
		break;
	case 'f': case 'F'://遁地
		cameraPos -= cameraSpeed * up;
		cameraTarget -= cameraSpeed * up;
		break;
	case 't': case 'T':	// 增大环境光系数
		La += 0.01f;
		if (La > 1.0f)
			La = 1.0f;
		break;
	case 'g': case 'G':	// 减小环境光系数
		La -= 0.01f;
		if (La < 0.0f)
			La = 0.0f;
		break;
	case 'o': case 'O':	// 改变太阳位置
		ligntModel = glm::rotate(ligntModel, glm::radians(-3.0f), lightAxis);
		lightPos = glm::vec3(ligntModel * glm::vec4(lightPos, 1.0));
		break;
	case 'p': case 'P':
		ligntModel = glm::rotate(ligntModel, glm::radians(3.0f), lightAxis);
		lightPos = glm::vec3(ligntModel * glm::vec4(lightPos, 1.0));
		break;
	case 'h':case 'H':	// 重置太阳位置
		ligntModel = glm::mat4(1.0f);
		lightPos = lightPosInit;
		break;
	case ' ':
		LightMode = (LightMode + 1) % 3;

		printf("Light Pos: %f %f %f\n", cameraPos.x, cameraPos.y, cameraPos.z);
		// printf("Light Target: %f %f %f\n", cameraTarget.x, cameraTarget.y, cameraTarget.z);
		break;
	}

	viewMatrix = glm::lookAt(cameraPos, cameraTarget, up);
	// lightView = glm::lookAt(LightPos, cameraCenter, up);
	lightView = glm::lookAt(lightPos, cameraCenter, glm::vec3(1.0f, 0.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;
}

void MouseFunc(int button, int state, int x, int y)      // 鼠标函数，  单击右键允许移动摄像头， 松开右键即不允许移动
{
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		bCameraRotate = true;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		bCameraRotate = false;
	}

	//滚轮控制视野的放大和缩小
	if (button == 3)
	{
		fov -= 1.5f;
	}
	if (button == 4) {
		fov += 1.5f;
	}
}

void MotionFunc(int x, int y)       // 鼠标移动函数，  右键摁下移动即摆动摄像头
{
	y = H - y;
	if (bCameraRotate)
	{
		if (x > lastMousePosX)
		{
			cameraXZRotateAngle += 0.02f;
			lastMousePosX = x;
		}
		else if (x < lastMousePosX)
		{
			cameraXZRotateAngle -= 0.02f;
			lastMousePosX = x;
		}
		if (y > lastMousePosY)
		{
			cameraYRotateAngle += 0.02f;
			lastMousePosY = y;
		}
		else if (y < lastMousePosY)
		{
			cameraYRotateAngle -= 0.02f;
			lastMousePosY = y;
		}
	}
	cameraDirection.x = sin(cameraXZRotateAngle);
	cameraDirection.z = -cos(cameraXZRotateAngle);
	cameraDirection.y = sin(cameraYRotateAngle);

	cameraDirection = glm::normalize(cameraDirection);

	cameraTarget = cameraPos + cameraDirection;

	viewMatrix = glm::lookAt(cameraPos, cameraTarget, up);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(W, H);
	glutCreateWindow("CGFinal");

	glewInit();
	init();
	glutDisplayFunc(display);  // 设置显示回调函数 -- 每帧执行
	glutKeyboardFunc(keyFunc);
	glutMouseFunc(MouseFunc);
	glutMotionFunc(MotionFunc);
	printf("===============漫游功能================\n");
	printf("输入W、S、D、A前后左右移动...\n");
	printf("输入R向上移动...\n");
	printf("输入F向下移动...\n");
	printf("通过滚轮控制放大缩小\n");
	printf("================光照控制===============\n");
	printf("输入T调高亮度...\n");
	printf("输入G调低亮度...\n");
	printf("输入P太阳落山...\n");
	printf("输入O太阳升起...\n");
	printf("输入H重置阳光...\n");

	glutMainLoop();

	return 0;
}


void DrawHouse() {

	ourShader.use();	// 启用着色器

	// 设置projection、view、model矩阵
	projectionMatrix = glm::perspective(fov, W / H, 100.0f, 6000.0f);
	ourShader.setMat4("projection", projectionMatrix);
	ourShader.setMat4("view", viewMatrix);
	ourShader.setMat4("model", modelMatrix);

	// 设置light的uniform变量
	ourShader.setVec3("camPos", cameraPos);
	ourShader.setVec3("lightPos", lightPos);
	ourShader.setFloat("La", La);
	ourShader.setInt("LightMode", LightMode);
	ourShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	// 绑定深度图
	glActiveTexture(mFBO->GetBuffer("depth")); // active proper texture unit before binding
	ourShader.setInt("depthMap", mFBO->GetBuffer("depth"));
	glBindTexture(GL_TEXTURE_2D, mFBO->GetBuffer("depth"));

	// 绘制
	ourHouse.Draw(ourShader);

	glUseProgram(0);
}

void DrawShadow()	// 从光照方向渲染模型，得到深度图，用于生成阴影
{
	glClear(GL_DEPTH_BUFFER_BIT);
	DepthShader.use();
	ourShader.setMat4("model", modelMatrix);
	DepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);	// 使用光照空间矩阵lightSpaceMatrix
	ourHouse.Draw(DepthShader);
	glUseProgram(0);
}

void DrawSnow() {
	snow.Draw(0.01f, modelMatrix, viewMatrix, projectionMatrix);
}

void DrawSanta() 
{

}

void DrawCloud()
{

}
void DrawSkybox() 
{
	// 绘制天空盒
	glDepthFunc(GL_LEQUAL);  
	skyboxProgram.use();
	glm::mat4 modelsky = glm::mat4(1.0f);
	//modelsky = glm::translate(modelsky, glm::vec3(0.0f, skyScale, 0.0f));
	//modelsky = glm::rotate(modelsky, glm::radians(60.0f),glm::vec3(0.0, 1.0, 0.0));
	modelsky = glm::scale(modelsky, glm::vec3(skyScale, skyScale, skyScale));
	skyboxProgram.setMat4("view", viewMatrix);
	skyboxProgram.setMat4("projection", projectionMatrix);
	skyboxProgram.setMat4("model", modelsky);

	// 传cubemap纹理
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	skyboxProgram.setInt("skybox", 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glUseProgram(0);
	glDepthFunc(GL_LESS);
}