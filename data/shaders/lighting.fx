#include "globals.inl"
#include "gbuffer.inl"

Texture2DMS<float4,4> diffuse_tu : register(t0);
Texture2DMS<float4,4> normal_tv : register(t1);
Texture2DMS<float4,4> aux : register(t2);

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
	//inVertex.tcoord * int2(1200, 600)
	int2 n2TexCoord;
	n2TexCoord.x = (int)(inVertex.tcoord.x * 1200);
	n2TexCoord.y = (int)(inVertex.tcoord.y * 600);
	return (float4(diffuse_tu.Load(int3(n2TexCoord, 0), 0).xyz, 1.0) + float4(diffuse_tu.Load(int3(n2TexCoord, 0), 1).xyz, 1.0) + float4(diffuse_tu.Load(int3(n2TexCoord, 0), 2).xyz, 1.0) + float4(diffuse_tu.Load(int3(n2TexCoord, 0), 3).xyz, 1.0)) * 0.25;
}
RasterizerState EnableMultisampling
{
	MultisampleEnable = TRUE;
};
technique11 Lighting
{
	pass P0
	{
		SetVertexShader( CompileShader(vs_5_0, VS()) );
		SetGeometryShader(0);
		SetPixelShader(CompileShader(ps_5_0, PS()) );
	}
}