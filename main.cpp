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


bool bCameraRotate = false;			//  ������Ƿ�������ת
float cameraXZRotateAngle = 0;		// �����xzƽ���ڵ���ת�Ƕ� ��������ת
float cameraYRotateAngle = 0;		//  �������ֱƽ���ڵ���ת�Ƕ�   ������ת
int lastMousePosX, lastMousePosY;	//  �ϴ�����λ��  

const glm::vec3 cameraPosInit = glm::vec3(-437.0f, 408.0f, 977.5f);		// ��ʼ�����λ��
const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);	//���������� ���ڲ��
const glm::vec3 cameraTargetInit = glm::vec3(-249.570038f, 299.751007f, 599.132141f);	// ��ʼ�������
const glm::vec3 lightPosInit = glm::vec3(0.0f, 430.0f, 450.0f);	// ̫��λ�ó�ʼֵ
const glm::vec3 lightAxis = glm::vec3(0.0f, 0.0f, 1.0f);	// ̫����ת��
const float LaInit = 0.18f;	// ������ϵ��
const float skyScale = 2000.0f;	// ��պд�С

void DrawHouse();	// �����Ϲ�С��
void DrawShadow();	// ����������Ӱ
void DrawSnow();	// ����ѩ��
void DrawSanta();	// ����ͺͷ����
void DrawCloud();	// �����ƶ�
void DrawSkybox();	// ������պ�


void init()
{
	//������ɫ����ȡģ��=============================================================================
	ourShader.init("Res/Base.vs", "Res/Base.fs");
	DepthShader.init("Res/Depth.vs", "Res/Depth.fs");
	ourHouse.loadModel("Res/christmas-pumpkin/christmas-pumpkin.obj");
	snow.InitSnow();
	initSkybox();

	//��ʼ��������õ��ӽǾ���===========================================================================
	cameraPos = cameraPosInit;		//��ʼ���������λ��(x,y,z)
	cameraTarget = cameraTargetInit;	//���������
	cameraCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	
	//���������3���᣺�������ᡢ����
	cameraDirection = glm::normalize(cameraTarget - cameraPos);	//���������

	cameraRight = glm::normalize(glm::cross(up, cameraDirection));	//����
	cameraUp = glm::cross(cameraDirection, cameraRight);	//����
	cameraSpeed = 2.5f;	//����������ٶ�
	viewMatrix = glm::lookAt(cameraPos, cameraTarget, up);
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f, 2.0f, 2.0f));	// it's a bit too big for our scene, so scale it down

	//=======================================================================================
	La = LaInit;		// ���û�����
	LightMode = 0;		// ����ģ��
	lightPos = lightPosInit;
	ligntModel = glm::mat4(1.0f);
	lightProjection = glm::ortho(-10000.0f, 10000.0f, -10000.0f, 10000.0f, 1.0f, 10000.0f);
	lightView = glm::lookAt(lightPos, cameraCenter, glm::vec3(1.0f, 0.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;


	glEnable(GL_DEPTH_TEST);// ��Ȳ���
	glEnable(GLUT_MULTISAMPLE); // ���ز�����ʵ�ֿ����Ч��

	// ����һ��֡�������
	mFBO = new FrameBufferObject;												//FBO.h
	mFBO->AttachColorBuffer("color", GL_COLOR_ATTACHMENT0, W, H);			//��һ��colorbuffer��800,600����Ļ���
	mFBO->AttachDepthBuffer("depth", W, H);									//��һ��depthbuffer��800,600����Ļ���
	mFBO->Finish();
}


void display()
{	
	// ���ڵõ����ͼ
	mFBO->Bind();
	DrawShadow();
	mFBO->Unbind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ����
	DrawSkybox();
	DrawHouse();
	DrawSnow();
	DrawSanta();
	DrawCloud();
	
	glutSwapBuffers();
	glutPostRedisplay();
}


void keyFunc(GLubyte key, int x, int y)          // ���̽���������   ws�ƶ������   c�л�������  l���ص�
{
	cameraRight = glm::normalize(glm::cross(up, cameraDirection));//���ݳ���ı�����
	switch (key)
	{
		//������ƶ�------------------------------------------------------
	case 'w': case 'W'://ǰ��
		cameraPos += cameraSpeed * cameraDirection;
		cameraTarget += cameraSpeed * cameraDirection;
		break;
	case 's': case 'S'://����
		cameraPos -= cameraSpeed * cameraDirection;
		cameraTarget -= cameraSpeed * cameraDirection;
		break;
	case 'a': case 'A'://����
		cameraPos += cameraSpeed * cameraRight;
		cameraTarget += cameraSpeed * cameraRight;
		break;
	case 'd': case 'D'://����
		cameraPos -= cameraSpeed * cameraRight;
		cameraTarget -= cameraSpeed * cameraRight;
		break;
	case 'r': case 'R'://����
		cameraPos += cameraSpeed * up;
		cameraTarget += cameraSpeed * up;
		break;
	case 'f': case 'F'://�ݵ�
		cameraPos -= cameraSpeed * up;
		cameraTarget -= cameraSpeed * up;
		break;
	case 't': case 'T':	// ���󻷾���ϵ��
		La += 0.01f;
		if (La > 1.0f)
			La = 1.0f;
		break;
	case 'g': case 'G':	// ��С������ϵ��
		La -= 0.01f;
		if (La < 0.0f)
			La = 0.0f;
		break;
	case 'o': case 'O':	// �ı�̫��λ��
		ligntModel = glm::rotate(ligntModel, glm::radians(-3.0f), lightAxis);
		lightPos = glm::vec3(ligntModel * glm::vec4(lightPos, 1.0));
		break;
	case 'p': case 'P':
		ligntModel = glm::rotate(ligntModel, glm::radians(3.0f), lightAxis);
		lightPos = glm::vec3(ligntModel * glm::vec4(lightPos, 1.0));
		break;
	case 'h':case 'H':	// ����̫��λ��
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

void MouseFunc(int button, int state, int x, int y)      // ��꺯����  �����Ҽ������ƶ�����ͷ�� �ɿ��Ҽ����������ƶ�
{
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		bCameraRotate = true;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		bCameraRotate = false;
	}

	//���ֿ�����Ұ�ķŴ����С
	if (button == 3)
	{
		fov -= 1.5f;
	}
	if (button == 4) {
		fov += 1.5f;
	}
}

void MotionFunc(int x, int y)       // ����ƶ�������  �Ҽ������ƶ����ڶ�����ͷ
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
	glutDisplayFunc(display);  // ������ʾ�ص����� -- ÿִ֡��
	glutKeyboardFunc(keyFunc);
	glutMouseFunc(MouseFunc);
	glutMotionFunc(MotionFunc);
	printf("===============���ι���================\n");
	printf("����W��S��D��Aǰ�������ƶ�...\n");
	printf("����R�����ƶ�...\n");
	printf("����F�����ƶ�...\n");
	printf("ͨ�����ֿ��ƷŴ���С\n");
	printf("================���տ���===============\n");
	printf("����T��������...\n");
	printf("����G��������...\n");
	printf("����P̫����ɽ...\n");
	printf("����O̫������...\n");
	printf("����H��������...\n");

	glutMainLoop();

	return 0;
}


void DrawHouse() {

	ourShader.use();	// ������ɫ��

	// ����projection��view��model����
	projectionMatrix = glm::perspective(fov, W / H, 100.0f, 6000.0f);
	ourShader.setMat4("projection", projectionMatrix);
	ourShader.setMat4("view", viewMatrix);
	ourShader.setMat4("model", modelMatrix);

	// ����light��uniform����
	ourShader.setVec3("camPos", cameraPos);
	ourShader.setVec3("lightPos", lightPos);
	ourShader.setFloat("La", La);
	ourShader.setInt("LightMode", LightMode);
	ourShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	// �����ͼ
	glActiveTexture(mFBO->GetBuffer("depth")); // active proper texture unit before binding
	ourShader.setInt("depthMap", mFBO->GetBuffer("depth"));
	glBindTexture(GL_TEXTURE_2D, mFBO->GetBuffer("depth"));

	// ����
	ourHouse.Draw(ourShader);

	glUseProgram(0);
}

void DrawShadow()	// �ӹ��շ�����Ⱦģ�ͣ��õ����ͼ������������Ӱ
{
	glClear(GL_DEPTH_BUFFER_BIT);
	DepthShader.use();
	ourShader.setMat4("model", modelMatrix);
	DepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);	// ʹ�ù��տռ����lightSpaceMatrix
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
	// ������պ�
	glDepthFunc(GL_LEQUAL);  
	skyboxProgram.use();
	glm::mat4 modelsky = glm::mat4(1.0f);
	//modelsky = glm::translate(modelsky, glm::vec3(0.0f, skyScale, 0.0f));
	//modelsky = glm::rotate(modelsky, glm::radians(60.0f),glm::vec3(0.0, 1.0, 0.0));
	modelsky = glm::scale(modelsky, glm::vec3(skyScale, skyScale, skyScale));
	skyboxProgram.setMat4("view", viewMatrix);
	skyboxProgram.setMat4("projection", projectionMatrix);
	skyboxProgram.setMat4("model", modelsky);

	// ��cubemap����
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	skyboxProgram.setInt("skybox", 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glUseProgram(0);
	glDepthFunc(GL_LESS);
}