cbuffer cbColor : register(b0)
{
	float4 color;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PS_IN psIn) : SV_TARGET0
{
	return float4(psIn.tex.x, psIn.tex.y, 0, 1) * color;
}