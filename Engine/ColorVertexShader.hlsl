// GLOBALS
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

// TYPEDEFS
struct VertexInputType
{
    float4 position : POSITION;
	float3 normal : NORMAL;
    float4 color : COLOR;
	float2 text : TEXCOORD;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
	float4 worldPosition : POSITION;
	float3 normal : NORMAL;
    float4 color : COLOR;
	float2 text : TEXCOORD;
};

// Vertex Shader
PixelInputType main(VertexInputType input)
{
    PixelInputType output;

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.worldPosition = mul(input.position, worldMatrix);
    output.position = mul(output.worldPosition, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the input color for the pixel shader to use.
    output.color = input.color;

	// Store the texture coordinates for the pixel shader.
	output.text = input.text;

	// Calculate the normal vector against the world matrix only and normalize the normal vector
    output.normal = normalize(mul(input.normal, (float3x3)worldMatrix));
    
    return output;
}