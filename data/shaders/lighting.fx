#include "globals.inl"
#include "gbuffer.inl"

Texture2D diffuse_tu : register(t0);
Texture2D normal_tv : register(t1);
Texture2D aux : register(t2);
Texture2D depth : register(t3);

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
	float4 posW = aux.Sample(TextureSampler, inVertex.tcoord);
	float4 posH = mul(posW, lightMatrix);
	
	float2 shadowCoord;
	posH.xyz /= posH.w;
	shadowCoord.x = posH.x / 2.0f + 0.5f;
	shadowCoord.y = -posH.y / 2.0f + 0.5f;

	float depthInShadow = depth.Sample(TextureSampler, shadowCoord).x;
	float bias = 0.001;

	if ((posH.z - bias) > depthInShadow && (saturate(shadowCoord.x) == shadowCoord.x) && (saturate(shadowCoord.y) == shadowCoord.y))
	{
		//in shadow
		return float4(diffuse_tu.Sample(TextureSampler, inVertex.tcoord).xyz/2, 1.0);
	}
	else
	{
		return float4(diffuse_tu.Sample(TextureSampler, inVertex.tcoord).xyz, 1.0);
	}
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