cbuffer LightCBuf
{
	float3 lightPos;
	float3 diffuseColor;
	float diffuseIntensity;
	float attConst;
	float attLin;
	float attQuad;
};

float4 main(float3 worldPos : POSITION, float3 n : NORMAL) : SV_TARGET
{
	float3 vToL = lightPos - worldPos;
	float distToL = length(vToL);

	vToL = normalize(vToL);

	float diffuseFactor = max(dot(n, vToL), 0.0f);
	float3 diffuseContribution = diffuseFactor * diffuseColor;

	float3 viewVec = normalize(float3(0.0f, 0.0f, 0.0f) - worldPos);
	float3 halfwayVec = normalize(vToL + viewVec);
	float3 specularFactor = pow(max(dot(n, halfwayVec), 0.0f), diffuseIntensity);
	float3 specularContribution = specularFactor * diffuseColor;

	float attenuation = 1.0f / (attConst + attLin * distToL + attQuad * distToL * distToL);

	return float4((diffuseContribution + specularContribution) * attenuation, 1.0f);
}