struct OutputVertex
{
	float3 color : COLOR;
	float4 pos : SV_POSITION;
};

OutputVertex main(float3 pos : POSITION, float3 color : COLOR)
{
	OutputVertex output;
	output.pos = float4(pos, 1.0f);
	output.color = color;
	return output;
}