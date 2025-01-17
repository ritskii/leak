#if !defined(ASSETS_H)

#undef global
#undef local_global

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#define global static
#define local_global static

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#pragma pack(push, 1)
struct vertex
{
    dxm::XMFLOAT3 Pos;
    dxm::XMFLOAT2 Uv;
    dxm::XMFLOAT3 Normal;
    dxm::XMFLOAT3 Tangent;
    dxm::XMFLOAT3 BiTangent;
};

struct texel_rgba8
{
    u8 Red;
    u8 Green;
    u8 Blue;
    u8 Alpha;
};
#pragma pack(pop)

struct texture
{
    i32 Width;
    i32 Height;
    u32* Texels;

    ID3D12Resource* GpuTexture;
    D3D12_GPU_DESCRIPTOR_HANDLE GpuDescriptor;
};

struct mesh
{
    u32 IndexOffset;
    u32 IndexCount;
    u32 VertexOffset;
    u32 VertexCount;
    u32 TextureId;
};

struct model
{
    u32 NumMeshes;
    mesh* MeshArray;

    u32 NumTextures;
    texture* ColorTextureArray;
    texture* NormalTextureArray;
    texture* MRTextureArray;

    u32 VertexCount;
    vertex* VertexArray;
    u32 IndexCount;
    u32* IndexArray;

    ID3D12Resource* GpuVertexBuffer;
    ID3D12Resource* GpuIndexBuffer;

    dxm::XMMATRIX transform;
    bool is_pbr;
};

#define ASSETS_H
#endif
