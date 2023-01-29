float4 main(float3 normal : NORMAL) : SV_TARGET
{
	return float4(normal, 1.0f);
}