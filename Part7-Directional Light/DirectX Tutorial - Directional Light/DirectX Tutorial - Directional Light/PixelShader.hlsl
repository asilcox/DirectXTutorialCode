cbuffer LightCBuf
{
	float3 lightDir;
	float3 lightPos;
	float3 ambient;
	float3 diffuseColor;
	float diffuseIntensity;
	float attConst;
	float attLin;
	float attQuad;
};

float4 main(float3 worldPos : POSITION, float3 n : NORMAL) : SV_TARGET
{
	const float3 vToL = lightDir * (lightPos - worldPos);
	const float distToL = length(vToL);
	const float3 dirToL = vToL / distToL;

	const float fAngularAtt = saturate(dot(n, vToL));
	const float3 diffuse = diffuseColor * diffuseIntensity * fAngularAtt * max(0.0f, dot(dirToL, n));

	return float4(diffuse, 1.0f);
}