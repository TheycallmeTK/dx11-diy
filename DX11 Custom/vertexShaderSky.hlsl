
struct VertexShaderInput {
	float3 position		: POSITION;     // XYZ position
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float3 tangent		: TANGENT;
};

struct VertexToPixel {
	float4 position		: SV_POSITION;
	float3 sampleDir	: DIRECTION;
};

cbuffer ExternalData : register(b0)
{
	float4x4 view; float4x4 proj;
}



VertexToPixel main( VertexShaderInput input )
{
	VertexToPixel output;

	matrix viewNoTranslation = view;

	//setting translation to 0
	viewNoTranslation._14 = 0; 
	viewNoTranslation._24 = 0; 
	viewNoTranslation._34 = 0; 

	//apply projection and updated view to input position
	matrix combined = mul(proj, viewNoTranslation);
	output.position = mul(combined, float4(input.position, 1));

	//normalize output depth
	output.position.z = output.position.w;

	//sample direction for vertex
	output.sampleDir = input.position;

	return output;
}