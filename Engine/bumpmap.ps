////////////////////////////////////////////////////////////////////////////////
// Filename: bumpmap.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////

// The bump map shader requires two textures // but I'm pushing five for all presaved colours
// The first texture in the array is the color texture
// The second texture is the normal map.
// third is diffuse, fourth is ambient occlusion fifth is specular

Texture2D shaderTextures[5];
SamplerState SampleType;

// Just like most light shaders the direction and color 
// of the light is required for lighting calculations.

cbuffer LightBuffer
{
	// float4 diffuseColor;
    float3 lightDirection;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

// First the pixel is sampled from the color texture and the normal map. 

// The normal map value is then multiplied by two and is subtracted one to move it into the -1.0 to +1.0 float range. 

// This has to be done because the sampled value that is presented is in the 0.0 to +1.0 texture range,
// which only covers half the range needed for bump map normal calculations

// After that the bump normal is calculated using the equation: 
//    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);

// This bump normal is normalized and then used to determine the light intensity at this pixel 
// by doing a dot product with the light direction. 

// Once the light intensity at this pixel has been obtained the bump mapping is done
// The light intensity is used with the light color and texture color to get the final pixel color.

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 BumpMapPixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColour;
    float4 diffuseColour;
    float4 aoColour;
    float4 specularColour;
    float4 bumpMap;
    float3 bumpNormal;
    float3 lightDir;
    float lightIntensity;
    float4 colour;


    // Sample the texture pixels at this location.
    textureColour = shaderTextures[0].Sample(SampleType, input.tex);
    bumpMap = shaderTextures[1].Sample(SampleType, input.tex);
	diffuseColour = shaderTextures[2].Sample(SampleType, input.tex);
	aoColour = shaderTextures[3].Sample(SampleType, input.tex);
	specularColour = shaderTextures[4].Sample(SampleType, input.tex);
	
	//colour = textureColour;
	

    // Expand the range of the normal value from (0, +1) to (-1, +1).
    bumpMap = (bumpMap * 2.0f) - 1.0f;

    // Calculate the normal from the data in the bump map.
    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);
	
    // Normalize the resulting bump normal.
    bumpNormal = normalize(bumpNormal);

    // Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Calculate the amount of light on this pixel based on the bump map normal value.
    lightIntensity = saturate(dot(bumpNormal, lightDir));
	
	
    // Determine the final diffuse color based on the diffuse color and the amount of light intensity.
    colour = saturate(specularColour * lightIntensity); 

	colour = saturate(colour + aoColour); 
	
    // Combine the final bump light color with the texture color.
    colour = colour * textureColour;
	
	
    return colour;
}
