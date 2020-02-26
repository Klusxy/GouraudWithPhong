cbuffer MatrixBuffer : register( b0 )
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraBuffer : register( b1 )
{
	float3 cameraPosition;
	float padding;
};

cbuffer LightBuffer : register( b2 )
{
	float4 specularColor;
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightPosition;
	float padding_light;
};

struct VertexInputType
{
	float4 position : POSITION;
	float4 color : COLOR;
	float2 tex		: TEXCOORD;
	float3 normal : NORMAL;
};

struct PixelOutputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PixelOutputType TextureVertexShader(VertexInputType input)
{
	float4 worldPosition;

	input.position.w = 1.0f;
	PixelOutputType output;
	output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	input.normal = mul(input.normal, (float3x3)worldMatrix);
	input.normal = normalize(input.normal);
	//法线矩阵（当不等比缩放的时候，会影响法线的方向，需要乘一个法线矩阵）
	//output.normal = mul(output.normal, (float3x3)transpose(inverse(worldMatrix)));

	//在world空间中计算光照
	float3 calcLightPosition = mul(input.position, worldMatrix).xyz;
	float3 eyePosition = float3(-1.0f, 1.3f, -2.0f);
	float3 viewDirection = normalize(eyePosition - calcLightPosition);

	float4 ambient;
	float4 diffuse;
	float4 specular;

	float ambiStrength;
	float diffStrength;
	float specStrength;
	
	float3 lightDirection;

	//环境光(平行光):不直接指向物体表面上的光叫做环境光
	ambiStrength = 0.1f;
	ambient = ambientColor * ambiStrength;

	//漫反射光:不会照亮它不直接触摸的表面(依赖于光的位置，法线)
	//lightDirection = normalize(lightPosition - calcLightPosition);
	//为了比较gouraud和phong的区别，所以不用cb中的lightPosition
	float3 lp = float3(-1.0f, 1.3f, -2.0f);
	lightDirection = normalize(lp - calcLightPosition);
	diffStrength = max(dot(input.normal, lightDirection), 0.0f);
	diffuse = diffuseColor * diffStrength;

	//镜面高光（类似于镜子，观察角度不一样，得到的光照亮度也不同，所以依赖于光的位置，法线，观察方向）
	float3 ref = reflect(-lightDirection, input.normal);//入射光线和法线，所以这里lightDirection需要取反
	specStrength = pow(saturate(dot(ref, viewDirection)), 16);//shininess反光度(一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小//过于大的话可能specStrength的结果很小，导致没有镜面光)
	specular = specularColor * specStrength * 1.5;

	output.color = (diffuse + ambient) * input.color + specular;
	//output.color = specularColor;

	return output;
}