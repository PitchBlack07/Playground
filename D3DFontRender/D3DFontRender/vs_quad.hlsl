struct VS_IN
{
	float2 pos : POSITION0;
	uint   tex : TEXCOORD0;
	uint   vid : SV_VERTEXID;
};

cbuffer cbGlyphAddressTable : register(b0)
{
	float4 values[94];
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

VS_OUT main(VS_IN vsIn_)
{
	VS_OUT vsOut = (VS_OUT)0;
	vsOut.pos    = float4(vsIn_.pos, 0.f, 1.f);
	float4 tmp   = values[vsIn_.tex];

	switch (vsIn_.vid)
	{
	case 0: vsOut.tex = float2(tmp.x, tmp.w); break;
	case 1: vsOut.tex = float2(tmp.x, tmp.y); break;
	case 2: vsOut.tex = float2(tmp.z, tmp.w); break;
	default: vsOut.tex = float2(tmp.z, tmp.y); break;
	}

	return vsOut;
}