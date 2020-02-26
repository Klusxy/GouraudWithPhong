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
	//���߾��󣨵����ȱ����ŵ�ʱ�򣬻�Ӱ�취�ߵķ�����Ҫ��һ�����߾���
	//output.normal = mul(output.normal, (float3x3)transpose(inverse(worldMatrix)));

	//��world�ռ��м������
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

	//������(ƽ�й�):��ֱ��ָ����������ϵĹ����������
	ambiStrength = 0.1f;
	ambient = ambientColor * ambiStrength;

	//�������:������������ֱ�Ӵ����ı���(�����ڹ��λ�ã�����)
	//lightDirection = normalize(lightPosition - calcLightPosition);
	//Ϊ�˱Ƚ�gouraud��phong���������Բ���cb�е�lightPosition
	float3 lp = float3(-1.0f, 1.3f, -2.0f);
	lightDirection = normalize(lp - calcLightPosition);
	diffStrength = max(dot(input.normal, lightDirection), 0.0f);
	diffuse = diffuseColor * diffStrength;

	//����߹⣨�����ھ��ӣ��۲�ǶȲ�һ�����õ��Ĺ�������Ҳ��ͬ�����������ڹ��λ�ã����ߣ��۲췽��
	float3 ref = reflect(-lightDirection, input.normal);//������ߺͷ��ߣ���������lightDirection��Ҫȡ��
	specStrength = pow(saturate(dot(ref, viewDirection)), 16);//shininess�����(һ������ķ����Խ�ߣ�����������Խǿ��ɢ���Խ�٣��߹��ͻ�ԽС//���ڴ�Ļ�����specStrength�Ľ����С������û�о����)
	specular = specularColor * specStrength * 1.5;

	output.color = (diffuse + ambient) * input.color + specular;
	//output.color = specularColor;

	return output;
}