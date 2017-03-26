struct gbufferOut
{
	float4 diffuse_tu : SV_Target0;
	float4 normal_tv : SV_Target1;
	float4 aux : SV_Target2;
};

SamplerState TextureSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};