cbuffer MatrixBuffer
{
	matrix World;
	matrix View;
	matrix Projection;
	matrix Reflection;
}

struct Vin
{
    float4 pos : POSITION;
    float2 texcoord : TEXCOORD;
};

struct VOut
{
    float4 pos : SV_POSITION;
    float2 texcoord : TEXCOORD0;
	float4 reflectionPos : TEXCOORD1;
};

VOut main(Vin input)
{
	VOut output;
	matrix reflectProjectWorld;

	output.pos = mul(input.pos, World);
	output.pos = mul(output.pos, View);
	output.pos = mul(output.pos, Projection);
	output.texcoord = input.texcoord;

	reflectProjectWorld = mul(Reflection, Projection);
    reflectProjectWorld = mul(World, reflectProjectWorld);

	output.reflectionPos = mul(input.pos, reflectProjectWorld);

	return output;
}