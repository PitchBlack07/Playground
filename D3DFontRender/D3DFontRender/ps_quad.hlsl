Texture2D<float> glyphs : register(t0);
SamplerState glyphSampler : register(s0);

struct PS_IN
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

struct PS_OUT
{
	float4 color0 : SV_TARGET0;
};

PS_OUT main(PS_IN psIn_)
{
	PS_OUT psOut;

	psOut.color0 = glyphs.SampleLevel(glyphSampler, psIn_.tex, 0.f); // float4(psIn_.tex, 0.f, 1.f);
	return psOut;
}