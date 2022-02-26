#pragma once
#ifndef SKYBOX_H
#define SKYBOX_H

#include"model.h"

// std c++
#include <iostream>
#include <vector>

// ģ��
Model skybox;   // ��Ⱦһ��������������������ͼ������պ�

// ��ɫ���������
Shader skyboxProgram;   // ��պл���

// ����
GLuint skyboxTexture;   // ��պ�1

// skybox VAO
unsigned int skyboxVAO, skyboxVBO;


// ��ʼ��
void initSkybox()
{
    // ������ɫ���������
    skyboxProgram.init("Res/skybox/Shaders/skybox.vs", "Res/skybox/Shaders/skybox.fs");

    // ------------------------------------------------------------------------ // 
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // ����һ������������պе� ��������
    // skybox VAO
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // ������������ͼ
    std::vector<const GLchar*> faces;
    faces.push_back("Res/skybox/Textures/5.png");
    faces.push_back("Res/skybox/Textures/3.png");
    faces.push_back("Res/skybox/Textures/1.png");
    faces.push_back("Res/skybox/Textures/2.png");
    faces.push_back("Res/skybox/Textures/6.png");
    faces.push_back("Res/skybox/Textures/4.png");
    skyboxTexture = loadCubemap(faces);
}


#endif
