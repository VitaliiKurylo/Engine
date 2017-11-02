// TYPEDEFS
struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 worldPosition : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float2 text : TEXCOORD;
};

struct DirectionalLight
{
	float4 color;
	float4 direction;
};

struct Material
{
	float ambient, diffuse, specular, shininess;
};

// GLOBALS
Texture2D shaderTexture : register(t0);
SamplerState samplerState : register(s0);
cbuffer LightBuffer : register(b0)
{
	DirectionalLight light;
	Material material;
	float4 ambientLightColor;
	float4 cameraPosition;
};

// Phong Lighting Reflection Model
float4 calcPhongLighting(Material material, float4 lightColor, float3 normal, float3 lightDirection, float3 V, float3 R)
{
	float4 Ia = material.ambient * ambientLightColor;
	float4 Id = material.diffuse * saturate(dot(normal, lightDirection));
	float4 Is = material.specular * pow(saturate(dot(R, V)), material.shininess);

	return Ia + (Id + Is) * lightColor;
}

// Blinn-Phong Lighting Reflection Model
float4 calcBlinnPhongLighting(Material material, float4 lightColor, float3 normal, float3 lightDirection, float3 H)
{
	float4 Ia = material.ambient * ambientLightColor;
	float4 Id = material.diffuse * saturate(dot(normal, lightDirection));
	float4 Is = material.specular * pow(saturate(dot(normal, H)), material.shininess);

	return Ia + (Id + Is) * lightColor;
}

// Pixel Shader
float4 main(PixelInputType input) : SV_TARGET
{
	//calculate lighting vectors - renormalize vectors
	input.normal = normalize(input.normal);
	float3 V = (float3)normalize(cameraPosition - input.worldPosition);
	//Do not use -light.dir since the reflection returns a ray from the surface
	float3 R = reflect((float3)light.direction, input.normal);

	//calculate lighting
	float4 I = calcPhongLighting(material, light.color, input.normal, (float3)-light.direction, V, R);

	//sample the pixel color from the texture using the sampler at this texture coordinate location
	float4 textureColor = shaderTexture.Sample(samplerState, input.text);

	//return I * input.color;
	return I * input.color;
}