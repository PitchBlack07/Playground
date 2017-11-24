struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

VS_OUT main(uint vertexId : SV_VERTEXID)
{
	VS_OUT vsout;

	switch (vertexId)
	{
	case 0:
		vsout.pos = float4(-1, -1, 0, 1);
		vsout.tex = float2(0, 1);
		return vsout;

	case 1:
		vsout.pos = float4(-1, 1, 0, 1);
		vsout.tex = float2(0, 0);
		return vsout;

	case 2:
		vsout.pos = float4(1, -1, 0, 1);
		vsout.tex = float2(1, 1);
		return vsout;

	case 3:
	default:
		vsout.pos = float4(1, 1, 0, 1);
		vsout.tex = float2(1, 0);
		return vsout;
	}
}