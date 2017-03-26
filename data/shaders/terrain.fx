cbuffer globalMatrixs
{
	float4x4 projection;
	float4x4 viwewProjection;
};

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

float4 PS(VertexOut inVertex) : SV_Target
{
	return float4(0.5, 0.5, 0.5, 1.0);
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