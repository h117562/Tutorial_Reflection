Texture2D diffuseTexture;
SamplerState SampleType;

struct Pin
{
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 reflectionPos : TEXCOORD;
};

float4 main(Pin input) : SV_TARGET
{
    float4 textureColor = diffuseTexture.Sample(SampleType, input.tex);
    float4 reflectionColor;
	float2 reflectTexCoord;
	float4 output;

    reflectTexCoord.x = input.reflectionPos.x / input.reflectionPos.w / 2.0f + 0.5f;
    reflectTexCoord.y = -input.reflectionPos.y / input.reflectionPos.w / 2.0f + 0.5f;

	reflectionColor = diffuseTexture.Sample(SampleType, reflectTexCoord);

	output = lerp(textureColor, reflectionColor, 0.15f);

	return output;
}