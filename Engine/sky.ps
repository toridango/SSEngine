////////////////////////////////////////////////////////////////////////////////
// Filename: light.ps
////////////////////////////////////////////////////////////////////////////////



/////////////
// GLOBALS //
/////////////

cbuffer LightBuffer
{
	float4 ambientColour;
    float4 diffuseColour;
    float3 lightDirection;
	float specularPower;
    float4 specularColour;
};

TextureCube shaderTexture;
SamplerState SampleType;


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float3 tex : TEXCOORD0;
	float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
	float3 lightDir;
	float lightIntensity;
	float4 colour;
    float3 reflection;
    float4 specular;
	float4 textureColour;
	
	colour = ambientColour;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColour = shaderTexture.Sample(SampleType, input.tex);


	// Initialize the specular color.
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.normal, lightDir));

    // Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    if(lightIntensity > 0.0f)
	{

		// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
		colour += (diffuseColour * lightIntensity);

		// Saturate the ambient and diffuse colour.
        colour = saturate(colour);

		// Calculate the reflection vector based on the light intensity, normal vector, and light direction.
        reflection = normalize(2 * lightIntensity * input.normal - lightDir);
	
		// Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
        specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
	}
		
	// Multiply the texture pixel and the final diffuse color to get the final pixel color result.
	colour = colour * textureColour;


	colour = saturate(colour + specular);
    return colour;
}
