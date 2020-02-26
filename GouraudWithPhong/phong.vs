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

struct VertexInputType
{
	float4 position : POSITION;
	float4 color : COLOR;
	float2 tex		: TEXCOORD;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 calcLightPosition : POSITION0;
	float4 color : COLOR;
	float2 tex		: TEXCOORD;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
};

PixelInputType TextureVertexShader(VertexInputType input)
{
	float4 worldPosition;

	input.position.w = 1.0f;
	PixelInputType output;
	output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
	//output.position = input.position;
	output.color = input.color;
	output.tex = input.tex;
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);
	//法线矩阵（当不等比缩放的时候，会影响法线的方向，需要乘一个法线矩阵）
	//output.normal = mul(output.normal, (float3x3)transpose(inverse(worldMatrix)));

	//在world空间中计算光照
	output.calcLightPosition = mul(input.position, worldMatrix).xyz;
	float3 eyePosition = float3(1.0f, 1.3f, -2.0f);
	output.viewDirection = normalize(eyePosition - output.calcLightPosition);

	return output;
}