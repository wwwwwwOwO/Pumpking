#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// material parameters
// layout (binding = 0) uniform sampler2D albedoMap;
// layout (binding = 1) uniform sampler2D normalMap;
// layout (binding = 2) uniform sampler2D metallicMap;
// layout (binding = 3) uniform sampler2D roughnessMap;
// layout (binding = 4) uniform sampler2D aoMap;
// �����ʣ�������(Albedo)����Ϊÿһ������������(Texel)���������أ�ָ��������ɫ���߻��������ʡ��������֮ǰʹ�ù��������������൱���ƣ���ͬ�������й�����Ϣ������һ����������ȡ�ġ������������ͼ���г�������һЩϸС����Ӱ������ɫ�����ƣ����������������ǲ�������Щ�����ġ���Ӧ��ֻ�����������ɫ��������������ϵ������
uniform sampler2D albedoMap; 
// ���ߣ�������ͼ���������֮ǰ�ڷ�����ͼ�̳�����ʹ�õ���ͼ����ȫһ���ġ�������ͼʹ���ǿ�����Ƭ�ε�ָ�����صķ��ߣ���Ϊ��������������ƽ�ļ���
uniform sampler2D normalMap;
// �����ȣ�����(Metallic)��ͼ������ص�ָ���������ǲ��ǽ����ʵصġ�����PBR�������õĲ�ͬ������ʦ�Ǽȿ��Խ������ȱ�дΪ�Ҷ�ֵ�ֿ��Ա�дΪ1��0�����Ķ�Ԫֵ��
uniform sampler2D metallicMap;
// �ֲڶȣ��ֲڶ�(Roughness)��ͼ����������Ϊ��λָ��ĳ�������ж�ֲڡ����������Ĵֲڶ���ֵ��Ӱ��һ�������΢ƽ��ͳ��ѧ�ϵ�ȡ��ȡ�һ���Ƚϴֲڵı����õ���������ģ���ľ��淴�䣨�߹⣩����һ���ȽϹ⻬�ı������õ����ж������ľ��淴�䡣ĳЩPBR����Ԥ����õ��Ƕ�ĳЩ����ʦ��˵����ֱ�۵Ĺ⻬��(Smoothness)��ͼ���Ǵֲڶ���ͼ��������Щ��ֵ�ڲ���֮ʱ�������ã�1.0 �C �⻬�ȣ�ת�����˴ֲڶȡ�
uniform sampler2D roughnessMap;
// AO���������ڱ�(Ambient Occlusion)��ͼ����˵AO��ͼΪ�������ΧǱ�ڵļ���ͼ��ָ����һ���������Ӱ���ӡ��������������һ��ש����棬�����������ϵ�ש���ѷ첿��Ӧ��û���κ���Ӱ��Ϣ��Ȼ��AO��ͼ������Щ���߽������ݳ����İ�ɫ��Եָ���������ڹ��յĽ�β�׶����뻷���ڱο������Ե������㳡�����Ӿ�Ч��������/����Ļ����ڱ���ͼҪôͨ���ֶ����ɣ�Ҫô��3D��ģ����Զ����ɡ�
uniform sampler2D aoMap;

// lights
uniform vec3 lightPos;

uniform vec3 camPos;

const float PI = 3.14159265359;

uniform int LightMode;
uniform float La;


// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
void main()
{
    if(LightMode == 0)
    {
        vec3 albedo     = pow(texture(albedoMap, TexCoords).rgb, vec3(1.6));
        float metallic  = texture(metallicMap, TexCoords).r * 0.5;
        float roughness = texture(roughnessMap, TexCoords).r * 2;
        // float ao        = texture(aoMap, TexCoords).r;

        vec3 N = getNormalFromMap();
        vec3 V = normalize(camPos - WorldPos);

        // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
        // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
        vec3 F0 = vec3(0.04); 
        F0 = mix(F0, albedo, metallic);

        // reflectance equation
        vec3 Lo = vec3(0.0);
        // calculate per-light radiance
        vec3 L = normalize(lightPos - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPos - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = vec3(2000000.0) * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    
        // ambient lighting (note that the next IBL tutorial will replace 
        // this ambient lighting with environment lighting).
        // vec3 ambient = vec3(0.12) * albedo * ao;
        vec3 ambient = (kD * albedo + specular) * 1.0;

        vec3 color = ambient + Lo;

        // HDR tonemapping
        // color = color / (color + vec3(1.0));
        // gamma correct
        // color = pow(color, vec3(1.0/2.2)); 

        FragColor = vec4(color, 1.0);
    }
    else
    {
                    
        // vec3 color = texture(albedoMap, TexCoords).rgb;
        vec3 color = pow(texture(albedoMap, TexCoords).rgb, vec3(1.6));
        // ambient
        vec3 ambient = La * color;
        // diffuse
        vec3 lightDir = normalize(lightPos - WorldPos);
        vec3 normal = normalize(Normal);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = diff * color;
        // specular
        vec3 viewDir = normalize(camPos - WorldPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = 0.0;
        if(LightMode == 1)  // blinn phong
        {
            vec3 halfwayDir = normalize(lightDir + viewDir);  
            spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
        }
        else
        {
            vec3 reflectDir = reflect(-lightDir, normal);
            spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
        }
        vec3 specular = vec3(0.3) * spec; // assuming bright white light color
        FragColor = vec4(ambient + diffuse + (0.06*specular), 1.0);
    }

}