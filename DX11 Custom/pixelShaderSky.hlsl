
struct VertexToPixel {
	float4 position		: SV_POSITION;	// XYZW position (System Value Position)
	float3 sampleDir	: TEXCOORD;
};

TextureCube cube : register(t0);
SamplerState sampleState : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	return cube.Sample(sampleState, input.sampleDir);
}