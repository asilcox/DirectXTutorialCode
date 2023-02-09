cbuffer cBuf
{
	matrix wMatrix;
	matrix vMatrix;
	matrix pMatrix;
};

struct OutputVertex
{
	float3 worldPos : POSITION;
	float3 normal : NORMAL;
	float4 pos : SV_POSITION;
};

OutputVertex main(float3 pos : POSITION, float3 n : NORMAL)
{
	OutputVertex output;
	output.worldPos = mul(wMatrix, float4(pos, 1.0f));
	output.normal = mul(wMatrix, n);
	output.normal = mul(vMatrix, output.normal);
	output.normal = mul(pMatrix, output.normal);
	output.pos = mul(wMatrix, float4(pos, 1.0f));
	output.pos = mul(vMatrix, output.pos);
	output.pos = mul(pMatrix, output.pos);
	return output;
}