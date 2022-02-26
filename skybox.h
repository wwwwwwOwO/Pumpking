#pragma once
#ifndef SKYBOX_H
#define SKYBOX_H

#include"model.h"

// std c++
#include <iostream>
#include <vector>

// 模型
Model skybox;   // 渲染一个立方体用于立方体贴图绘制天空盒

// 着色器程序对象
Shader skyboxProgram;   // 天空盒绘制

// 纹理
GLuint skyboxTexture;   // 天空盒1

// skybox VAO
unsigned int skyboxVAO, skyboxVBO;


// 初始化
void initSkybox()
{
    // 生成着色器程序对象
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

    // 生成一个立方体做天空盒的 “画布”
    // skybox VAO
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // 加载立方体贴图
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
