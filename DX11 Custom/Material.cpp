#include "Material.h"

Material::Material(DirectX::XMFLOAT4 tint, SimplePixelShader* pixShader, SimpleVertexShader* vertShader, float spec, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv, Microsoft::WRL::ComPtr<ID3D11SamplerState> sample, bool normal, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> norm, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metal, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughness)
{
    colorTint = tint;
    pixelShader = pixShader;
    vertexShader = vertShader;
    specExponent = spec;
    sampler = sample;
    SRV = srv;
    hasNormal = normal;
    if (hasNormal) { normalMap = norm; }
    metalMap = metal;
    roughnessMap = roughness;
}

DirectX::XMFLOAT4 Material::getTint()
{
    return colorTint;
}

void Material::setTint(DirectX::XMFLOAT4 tint)
{
    colorTint = tint;
}

SimplePixelShader* Material::getPixel()
{
    return pixelShader;
}

SimpleVertexShader* Material::getVertex()
{
    return vertexShader;
}

Microsoft::WRL::ComPtr<ID3D11SamplerState> Material::getSampler()
{
    return sampler;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::getSRV()
{
    return SRV;
}
