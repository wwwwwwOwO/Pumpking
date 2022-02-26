#include "snow.h"


namespace Snow {
	Snow::Snow()
	{
		// InitSnow();
	}

	Snow::~Snow()
	{
	}

	bool Snow::InitSnow()
	{
		// 初始化Index、着色器
		mCurVBOIndex = 0;
		mCurTransformFeedbackIndex = 1;
		mFirst = true;
		mTimer = 0;
		const GLchar* varyings[5] = { "Type1", "Position1", "Velocity1", "Age1", "Size1" };//设置TransformFeedback要捕获的输出变量
		mUpdateShader = new Shader("Res/snow/Shaders/Update.vs", "Res/snow/Shaders/Update.fs", "Res/snow/Shaders/Update.gs", varyings, 5);	//设置TransformFeedback缓存能够记录的顶点的数据类型
		mRenderShader = new Shader("Res/snow/Shaders/Render.vs", "Res/snow/Shaders/Render.fs");
		
		InitRandomTexture(512);	// 设置随机纹理
		mSparkTexture.id = TextureFromFile("snowstorm.bmp", "Res/snow/Textures");	// 加载雪花纹理
		mSparkTexture.type = "Alpha";
		mSparkTexture.path = "Res/snow/Textures";

		mRenderShader->use();	// 为雪花的渲染着色器指定纹理
		glActiveTexture(GL_TEXTURE0);
		mRenderShader->setInt("snowflower", 0);
		glBindTexture(GL_TEXTURE_2D, mSparkTexture.id);

		SnowParticle particles[MAX_PARTICLES];	// 初始化粒子数组
		memset(particles, 0, sizeof(particles));
		GenInitLocation(particles, INIT_PARTICLES);	// 为这些粒子赋予初始属性
		glGenTransformFeedbacks(2, mTransformFeedbacks);	
		glGenBuffers(2, mParticleBuffers);
		glGenVertexArrays(2, mParticleArrays);
		for (int i = 0; i < 2; i++)
		{
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mTransformFeedbacks[i]);
			glBindBuffer(GL_ARRAY_BUFFER, mParticleBuffers[i]);
			glBindVertexArray(mParticleArrays[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(particles), particles, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mParticleBuffers[i]);
			//glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(particles), temp_particles);
		}

		// 为几何着色器绑定Uniform数据
		mUpdateShader->use();
		mUpdateShader->setInt("gRandomTexture", 0);
		mUpdateShader->setVec3("MAX_VELOC", MAX_VELOC);
		mUpdateShader->setVec3("MIN_VELOC", MIN_VELOC);
		mUpdateShader->setFloat("MAX_SIZE", MAX_SIZE);
		mUpdateShader->setFloat("MIN_SIZE", MIN_SIZE);
		mUpdateShader->setFloat("MAX_LAUNCH", MAX_LAUNCH);
		mUpdateShader->setFloat("MIN_LAUNCH", MIN_LAUNCH);
		mUpdateShader->setFloat("areaLength", areaLength);
		mUpdateShader->setFloat("fallHeight", fallHeight);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
		glBindVertexArray(0);
		glUseProgram(0);
		return true;
	}

	void Snow::Draw(float frametime, glm::mat4& worldMatrix,
		glm::mat4 viewMatrix, glm::mat4& projectMatrix)
	{
		UpdateParticles(frametime);
		RenderParticles(worldMatrix, viewMatrix, projectMatrix);
		mCurVBOIndex = mCurTransformFeedbackIndex;
		mCurTransformFeedbackIndex = (mCurTransformFeedbackIndex + 1) & 0x1;
		glUseProgram(0);
	}

	void Snow::UpdateParticles(float frametime)
	{
		mTimer += frametime;
		mUpdateShader->use();
		mUpdateShader->setFloat("gDeltaTime", frametime);
		mUpdateShader->setFloat("gTimer", mTimer);
		//绑定纹理
		glActiveTexture(GL_TEXTURE0);
		mUpdateShader->setInt("gRandomTexture", 0);
		glBindTexture(GL_TEXTURE_1D, mRandomTexture);

		glEnable(GL_RASTERIZER_DISCARD);//我们渲染到TransformFeedback缓存中去，并不需要光栅化
		glBindVertexArray(mParticleArrays[mCurVBOIndex]);
		glBindBuffer(GL_ARRAY_BUFFER, mParticleBuffers[mCurVBOIndex]);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mTransformFeedbacks[mCurTransformFeedbackIndex]);

		glEnableVertexAttribArray(0);//type
		glEnableVertexAttribArray(1);//position
		glEnableVertexAttribArray(2);//velocity
		glEnableVertexAttribArray(3);//lifetime
		glEnableVertexAttribArray(4);//size
		glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(SnowParticle), (void*)offsetof(SnowParticle, type));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SnowParticle), (void*)offsetof(SnowParticle, position));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(SnowParticle), (void*)offsetof(SnowParticle, velocity));
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(SnowParticle), (void*)offsetof(SnowParticle, lifetime));
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(SnowParticle), (void*)offsetof(SnowParticle, size));

		glBeginTransformFeedback(GL_POINTS);

		if (mFirst)
		{

			glDrawArrays(GL_POINTS, 0, INIT_PARTICLES);
			mFirst = false;
		}
		else {
			glDrawTransformFeedback(GL_POINTS, mTransformFeedbacks[mCurVBOIndex]);
		}
		glEndTransformFeedback();
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);
		glDisable(GL_RASTERIZER_DISCARD);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}

	void Snow::RenderParticles(glm::mat4& worldMatrix,
		glm::mat4& viewMatrix, glm::mat4& projectMatrix)
	{
		glEnable(GL_POINT_SPRITE);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		mRenderShader->use();
		mRenderShader->setMat4("model", worldMatrix);
		mRenderShader->setMat4("view", viewMatrix);
		mRenderShader->setMat4("projection", projectMatrix);
		glBindVertexArray(mParticleArrays[mCurTransformFeedbackIndex]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mParticleBuffers[mCurTransformFeedbackIndex]);
		glBindBuffer(GL_ARRAY_BUFFER, mParticleBuffers[mCurTransformFeedbackIndex]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SnowParticle), (void*)offsetof(SnowParticle, position));
		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(SnowParticle), (void*)offsetof(SnowParticle, size));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glActiveTexture(GL_TEXTURE0);
		mRenderShader->setInt("snowflower", 0);
		glBindTexture(GL_TEXTURE_2D, mSparkTexture.id);

		//glDrawArrays(GL_POINTS, 0, INIT_PARTICLES);
		glDrawTransformFeedback(GL_POINTS, mTransformFeedbacks[mCurTransformFeedbackIndex]);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisable(GL_POINT_SPRITE);
		glDisable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

	}

	void Snow::InitRandomTexture(unsigned int size)
	{
		srand(time(NULL));
		glm::vec3* pRandomData = new glm::vec3[size];
		for (int i = 0; i < size; i++)
		{
			pRandomData[i].x = float(rand()) / float(RAND_MAX);
			pRandomData[i].y = float(rand()) / float(RAND_MAX);
			pRandomData[i].z = float(rand()) / float(RAND_MAX);
		}
		glGenTextures(1, &mRandomTexture);
		glBindTexture(GL_TEXTURE_1D, mRandomTexture);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, size, 0, GL_RGB, GL_FLOAT, pRandomData);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		delete[] pRandomData;
		pRandomData = nullptr;
	}

	void Snow::GenInitLocation(SnowParticle particles[], int nums)
	{
		srand(time(NULL));
		for (int x = 0; x < nums; x++) {
			glm::vec3 record(0.0f);
			record.x = (2.0f * float(rand()) / float(RAND_MAX) - 1.0f) * areaLength;
			record.z = (2.0f * float(rand()) / float(RAND_MAX) - 1.0f) * areaLength;
			record.y = fallHeight;
			particles[x].type = PARTICLE_TYPE_LAUNCHER;
			particles[x].position = record;
			record.x = (MAX_VELOC.x - MIN_VELOC.x) * (float(rand()) / float(RAND_MAX)) + MIN_VELOC.x;
			record.y = (MAX_VELOC.y - MIN_VELOC.y) * (float(rand()) / float(RAND_MAX)) + MIN_VELOC.y;
			record.z = (MAX_VELOC.z - MIN_VELOC.z) * (float(rand()) / float(RAND_MAX)) + MIN_VELOC.z;
			particles[x].velocity = record;//在最大最小速度之间随机选择
			particles[x].size = (MAX_SIZE - MIN_SIZE) * (float(rand()) / float(RAND_MAX)) + MIN_SIZE;//发射器粒子大小
			particles[x].lifetime = (MAX_LAUNCH - MIN_LAUNCH) * (float(rand()) / float(RAND_MAX)) + MIN_LAUNCH;
			//printf("%d record %f %f %f\n", x, record.x, record.y, record.z);
		}


	}
}
