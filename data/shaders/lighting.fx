#include "globals.inl"
#include "gbuffer.inl"

Texture2D diffuse_tu : register(t0);
Texture2D normal_tv : register(t1);
Texture2D aux : register(t2);

struct Vertex
{
	float2 position : POSITION;
};

struct VertexOut
{
	float4 positionH : SV_POSITION;
	float2 tcoord : TEXCOORD;
};

VertexOut VS(Vertex inVertex)
{
	VertexOut v;
	v.positionH = float4(inVertex.position, 0.0, 1.0);
	v.tcoord = (inVertex.position.xy + float2(1.0, 1.0)) / 2.0;
	v.tcoord.y = 1.0 - v.tcoord.y;
	return v;
}

float4 PS(VertexOut inVertex) : SV_Target
{
	return float4(diffuse_tu.Sample(TextureSampler,inVertex.tcoord).xyz, 1.0);
}

technique11 Lighting
{
	pass P0
	{
		SetVertexShader( CompileShader(vs_5_0, VS()) );
		SetGeometryShader(0);
		SetPixelShader(CompileShader(ps_5_0, PS()) );
	}
}