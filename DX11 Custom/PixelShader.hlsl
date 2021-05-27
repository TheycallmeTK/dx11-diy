
struct DirectionalLight
{
	float3 ambientColor;
	float3 diffuseColor;
	float3 direction;
};

struct PointLight 
{
	float3 ambientColor;
	float3 diffuseColor;
	float3 position;
};

static const float F0_NON_METAL = 0.04f;// Minimum roughness for when spec distribution function denominator goes to zero
static const float MIN_ROUGHNESS = 0.0000001f; // 6 zeros after decimal
// Handy to have this as a constant
static const float PI = 3.14159265359f;

//Texture2D diffuseTexture	: register(t0);
SamplerState samplerOptions : register(s0);
Texture2D Albedo			: register(t0); 
Texture2D NormalMap			: register(t1); 
Texture2D RoughnessMap		: register(t2); 
Texture2D MetalnessMap		: register(t3);

// Lambert diffuse BRDF - Same as the basic lighting diffuse calculation!

// - NOTE: this function assumes the vectors are already NORMALIZED!

float DiffusePBR(float3 normal, float3 dirToLight)

{

	return saturate(dot(normal, dirToLight));

}



// Calculates diffuse amount based on energy conservation

//

// diffuse - Diffuse amount

// specular - Specular color (including light color)

// metalness - surface metalness amount

//

// Metals should have an albedo of (0,0,0)...mostly

// See slide 65: http://blog.selfshadow.com/publications/s2014-shading-course/hoffman/s2014_pbs_physics_math_slides.pdf

float3 DiffuseEnergyConserve(float diffuse, float3 specular, float metalness)

{

	return diffuse * ((1 - saturate(specular)) * (1 - metalness));

}



// GGX (Trowbridge-Reitz)

//

// a - Roughness

// h - Half vector

// n - Normal

//

// D(h, n) = a^2 / pi * ((n dot h)^2 * (a^2 - 1) + 1)^2

float SpecDistribution(float3 n, float3 h, float roughness)

{

	// Pre-calculations

	float NdotH = saturate(dot(n, h));

	float NdotH2 = NdotH * NdotH;

	float a = roughness * roughness;

	float a2 = max(a * a, MIN_ROUGHNESS); // Applied after remap!


	// ((n dot h)^2 * (a^2 - 1) + 1)

	float denomToSquare = NdotH2 * (a2 - 1) + 1;

	// Can go to zero if roughness is 0 and NdotH is 1


	// Final value

	return a2 / (PI * denomToSquare * denomToSquare);

}




// Fresnel term - Schlick approx.

//

// v - View vector

// h - Half vector

// f0 - Value when l = n (full specular color)

//

// F(v,h,f0) = f0 + (1-f0)(1 - (v dot h))^5

float3 Fresnel(float3 v, float3 h, float3 f0)

{

	// Pre-calculations

	float VdotH = saturate(dot(v, h));


	// Final value

	return f0 + (1 - f0) * pow(1 - VdotH, 5);

}



// Geometric Shadowing - Schlick-GGX (based on Schlick-Beckmann)

// - k is remapped to a / 2, roughness remapped to (r+1)/2

//

// n - Normal

// v - View vector

//

// G(l,v,h)

float GeometricShadowing(float3 n, float3 v, float3 h, float roughness)

{

	// End result of remapping:

	float k = pow(roughness + 1, 2) / 8.0f;

	float NdotV = saturate(dot(n, v));


	// Final value

	return NdotV / (NdotV * (1 - k) + k);

}





// Microfacet BRDF (Specular)

//

// f(l,v) = D(h)F(v,h)G(l,v,h) / 4(n dot l)(n dot v)

// - part of the denominator are canceled out by numerator (see below)

//

// D() - Spec Dist - Trowbridge-Reitz (GGX)

// F() - Fresnel - Schlick approx

// G() - Geometric Shadowing - Schlick-GGX

float3 MicrofacetBRDF(float3 n, float3 l, float3 v, float roughness, float metalness, float3 specColor)

{

	// Other vectors

	float3 h = normalize(v + l);


	// Grab various functions

	float D = SpecDistribution(n, h, roughness);

	float3 F = Fresnel(v, h, specColor);

	float G = GeometricShadowing(n, v, h, roughness) * GeometricShadowing(n, l, h, roughness);


	// Final formula

	// Denominator dot products partially canceled by G()!

	// See page 16: http://blog.selfshadow.com/publications/s2012-shading-course/hoffman/s2012_pbs_physics_math_notes.pdf

	return (D * F * G) / (4 * max(dot(n, v), dot(n, l)));

}

cbuffer ExternalData : register(b0)
{
	DirectionalLight light;
	DirectionalLight light2;
	DirectionalLight light3;
	PointLight point1;
	float3 cameraPos;
	float specExponent;
	
}
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;
	float4 color		: COLOR;
	float3 normal		: NORMAL;
	float3 worldPos		: POSITION;
	float2 uv			: TEXCOORD;
};
float3 calcSpec(VertexToPixel input, float3 lightDir)
{
	float3 V = normalize(cameraPos - input.worldPos);
	float3 R = reflect(normalize(lightDir), input.normal);
	float spec = pow(saturate(dot(R, V)), specExponent);
	return spec;
}

//helper function to calculate the final color for each light to add together in main
float3 calcColor(DirectionalLight l, VertexToPixel input, float3 surface, float3 roughness, float3 metal, float3 spec)
{
	//normalize the normal
	input.normal = normalize(input.normal);
	
	//negate and normalize the direction to get the vector to the light
	float3 negatedDirection = normalize(-l.direction);
	float3 toCam = normalize(cameraPos - input.worldPos);



	//calculate final color for this light

	float diff = DiffusePBR(input.normal, negatedDirection);
	float3 specularity = MicrofacetBRDF(input.normal, negatedDirection, toCam, roughness, metal, spec);
	float3 balanced = DiffuseEnergyConserve(diff, specularity, metal);

	float3 finalColor = (balanced * surface + specularity) * l.ambientColor;

	return finalColor;

}

float3 calcPointColor(PointLight l, VertexToPixel input, float3 surface, float3 roughness, float3 metal, float3 spec)
{

	float3 dirLight = normalize(l.position - input.worldPos);
	float3 toCam = normalize(cameraPos - input.worldPos);

	float3 diff = DiffusePBR(input.normal, dirLight);
	float3 specularity = MicrofacetBRDF(input.normal, dirLight, toCam, roughness, metal, spec);
	float3 balanced = DiffuseEnergyConserve(diff, specularity, metal);
	float3 finalColor = (balanced * surface + specularity) * l.ambientColor;
	return finalColor;

}



// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	input.uv.x+=1;
	//float3 surfaceColor = pow(diffuseTexture.Sample(samplerOptions, input.uv).rgb,2.2f);
	float3 surfaceColor = pow(Albedo.Sample(samplerOptions, input.uv).rgb, 2.2f);

	float roughness = RoughnessMap.Sample(samplerOptions, input.uv).r;

	float metal = MetalnessMap.Sample(samplerOptions, input.uv).r;

	float3 specularColor = lerp(F0_NON_METAL.rrr, surfaceColor.rgb, metal);

	float3 n = input.normal;



	//calculate final color with helper function
	float3 finalColor = calcColor(light, input, surfaceColor, roughness, metal, specularColor) + calcColor(light2, input, surfaceColor, roughness, metal, specularColor) + calcColor(light3, input, surfaceColor, roughness, metal, specularColor) + calcPointColor(point1, input, surfaceColor, roughness, metal, specularColor);
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	return float4(pow(finalColor, 1.0f/2.2f), 1);
}

