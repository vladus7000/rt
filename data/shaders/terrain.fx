#include "globals.inl"
#include "gbuffer.inl"

struct Vertex
{
	float3 position : POSITION;
};

struct VertexOut
{
	float4 positionH : SV_POSITION;
};

VertexOut VS(Vertex inVertex)
{
	VertexOut v;
	v.positionH = mul(float4(inVertex.position, 1.0), viwewProjection);
	
	return v;
}

gbufferOut PS(VertexOut inVertex)
{
	gbufferOut pixelout;

	pixelout.diffuse_tu = float4(0.5, 0.5, 1.0, 0.0);
	pixelout.normal_tv = float4(0.0, 0.0, 0.0, 0.0);
	pixelout.aux = float4(0.0, 0.0, 0.0, 0.0);

	return pixelout;
}

technique11 Terrain
{
	pass P0
	{
		SetVertexShader( CompileShader(vs_5_0, VS()) );
		SetGeometryShader(0);
		SetPixelShader(CompileShader(ps_5_0, PS()) );
	}
}