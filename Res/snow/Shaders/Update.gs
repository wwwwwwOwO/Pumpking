#version 330 core
layout (points) in;
layout (points,max_vertices = 10) out;

// 输入粒子属性
in float Type0[];
in vec3 Position0[];
in vec3 Velocity0[];
in float Age0[];
in float Size0[];

// 输出粒子属性
out float Type1;
out vec3 Position1;
out vec3 Velocity1;
out float Age1;
out float Size1;

uniform float gDeltaTime;   // 每帧时间变化量
uniform float gTimer;       // 雪花系统计数器
uniform sampler1D gRandomTexture;   // 用于生成随机数的贴图
uniform float areaLength;   // 落雪区域半径
uniform float fallHeight;   // 落雪高度
uniform float MAX_SIZE;     //最大大小
uniform float MIN_SIZE;     //最小大小
uniform vec3 MAX_VELOC;     //最大速度
uniform vec3 MIN_VELOC;     //最小速度
uniform float MAX_LAUNCH;   //最大发射年龄
uniform float MIN_LAUNCH;   //最小发射年龄

#define PARTICLE_TYPE_LAUNCHER 0.0f //发射器
#define PARTICLE_TYPE_SHELL 1.0f    //运动粒子

// 生成随机方向的向量 （-0.5, 0.5)
vec3 GetRandomDir(float TexCoord);
// 生成随机方向的向量 （0.0, 1.0)
vec3 Rand(float TexCoord);

void main()
{
    float Age = Age0[0] - gDeltaTime;   // 计算更新后的生命周期
    
    if(Age < 0){    // 若生命周期耗尽，则在原平面上产生新的雪花粒子
        // 与Init相同
        Type1 = PARTICLE_TYPE_LAUNCHER; // Type
        Position1.x = (Rand(gTimer + Age0[0]).x - 0.5f) * areaLength * 2.0f;   // Position
        Position1.y = fallHeight;
        Position1.z = (Rand(gTimer + Age0[0]).z - 0.5f) * areaLength * 2.0f;
        vec3 Dir = GetRandomDir(gTimer + Age0[0]);
        Velocity1 = normalize(Dir) * ((MAX_VELOC.x - MIN_VELOC.x) / 2.0f); // Velocity
        Size1 = (MAX_SIZE - MIN_SIZE) * Rand(gTimer + Age0[0]).y + MIN_SIZE; // Size
        Age1 = (MAX_LAUNCH - MIN_LAUNCH) * Rand(gTimer + Age0[0]).z + MIN_LAUNCH;
    }
    else if (Position0[0].y > -50.0f)   // 跟据粒子原属性更新粒子，粒子做类平抛运动
    {
        Type1 = Type0[0];
        vec3 DeltaP = Velocity0[0] * gDeltaTime;    // 根据速度以及时间变化量更新粒子位置
        vec3 DeltaV = vec3(0.0, -9.81, 0.0) * gDeltaTime;   // 根据加速度和时间变化量更新粒子速度，加速度取重力加速度
        Position1 = Position0[0] + DeltaP;
        Velocity1 = Velocity0[0] + DeltaV;
        Size1 = Size0[0];
        Age1 = Age;
    }
    else    // 若粒子处于y=-50平面下方则不显示，设置Size = 0
    {
        Type1 = Type0[0];
        Position1 = Position0[0];
        Velocity1 = Velocity0[0];
        Size1 = 0;
        Age1 = Age;
    }

    EmitVertex();
    EndPrimitive();
}

vec3 GetRandomDir(float TexCoord)
{
    vec3 Dir = texture(gRandomTexture,TexCoord).xyz;
    Dir -= vec3(0.5,0.5,0.5);
    return Dir;
}

vec3 Rand(float TexCoord){//随机0-1
    vec3 ret = texture(gRandomTexture,TexCoord).xyz;
    return ret; 
}