
char* CombineStrings(const char* Str1, const char* Str2)
{
    size_t Length1 = strlen(Str1);
    size_t Length2 = strlen(Str2);
    char* Result = (char*)malloc(sizeof(char) * (Length1 + Length2 + 1));
    memcpy(Result, Str1, Length1 * sizeof(char));
    memcpy(Result + Length1, Str2, Length2 * sizeof(char));
    Result[Length1 + Length2] = 0;
    return Result;
}

static model AssetCreateCube(Renderer* renderer)
{
    model Result = {};
    {
        static int Texel = 0xFFFFFFFF;
        Result.NumTextures = 1;
        Result.TextureArray = (texture*)malloc(sizeof(texture));
        *Result.TextureArray = {};
        Result.TextureArray[0].Width = 1;
        Result.TextureArray[0].Height = 1;
        Result.TextureArray[0].Texels = &Texel;
        renderer->Dx12CreateTexture(Result.TextureArray[0].Width, Result.TextureArray[0].Height, (uint8_t*)Result.TextureArray[0].Texels,
            &Result.TextureArray[0].GpuTexture, &Result.TextureArray[0].GpuDescriptor);
    }
    {
        static vertex ModelVertices[] =
        {
            { V3(-0.5f, -0.5f, -0.5f), V2(0, 0), V3(0, 0, 1) },
            { V3(-0.5f, 0.5f, -0.5f), V2(1, 0), V3(0, 0, 1) },
            { V3(0.5f, 0.5f, -0.5f), V2(1, 1), V3(0, 0, 1) },
            { V3(0.5f, -0.5f, -0.5f), V2(0, 1), V3(0, 0, 1) },
            { V3(-0.5f, -0.5f, 0.5f), V2(0, 0), V3(0, 0, -1) },
            { V3(-0.5f, 0.5f, 0.5f), V2(1, 0), V3(0, 0, -1) },
            { V3(0.5f, 0.5f, 0.5f), V2(1, 1), V3(0, 0, -1) },
            { V3(0.5f, -0.5f, 0.5f), V2(0, 1), V3(0, 0, -1) },
            { V3(-0.5f, 0.5f, -0.5f), V2(0, 0), V3(-1, 0, 0) },
            { V3(-0.5f, -0.5f, -0.5f), V2(1, 0), V3(-1, 0, 0) },
            { V3(-0.5f, -0.5f, 0.5f), V2(1, 1), V3(-1, 0, 0) },
            { V3(-0.5f, 0.5f, 0.5f), V2(0, 1), V3(-1, 0, 0) },
            { V3(0.5f, 0.5f, -0.5f), V2(0, 0), V3(1, 0, 0) },
            { V3(0.5f, 0.5f, 0.5f), V2(1, 0), V3(1, 0, 0) },
            { V3(0.5f, -0.5f, 0.5f), V2(1, 1), V3(1, 0, 0) },
            { V3(0.5f, -0.5f, -0.5f), V2(0, 1), V3(1, 0, 0) },
            { V3(-0.5f, 0.5f, -0.5f), V2(0, 0), V3(0, 1, 0) },
            { V3(-0.5f, 0.5f, 0.5f), V2(1, 0), V3(0, 1, 0) },
            { V3(0.5f, 0.5f, 0.5f), V2(1, 1), V3(0, 1, 0) },
            { V3(0.5f, 0.5f, -0.5f), V2(0, 1), V3(0, 1, 0) },
            { V3(-0.5f, -0.5f, -0.5f), V2(0, 0), V3(0, -1, 0) },
            { V3(0.5f, -0.5f, -0.5f), V2(0, 1), V3(0, -1, 0) },
            { V3(0.5f, -0.5f, 0.5f), V2(1, 1), V3(0, -1, 0) },
            { V3(-0.5f, -0.5f, 0.5f), V2(1, 0), V3(0, -1, 0) }
        };

        static int ModelIndices[] =
        {
            0, 1, 2, 2, 3, 0,
            4, 7, 6, 6, 5, 4,
            8, 9, 10, 10, 11, 8,
            12, 13, 14, 14, 15, 12,
            16, 17, 18, 18, 19, 16,
            20, 21, 22, 22, 23, 20
        };

        Result.VertexCount = sizeof(ModelVertices) / sizeof(vertex);
        Result.IndexCount = sizeof(ModelIndices) / sizeof(int);
        Result.NumMeshes = 1;
        Result.MeshArray = (mesh*)malloc(sizeof(mesh));
        mesh Mesh = {};
        Mesh.IndexOffset = 0;
        Mesh.IndexCount = Result.IndexCount;
        Mesh.VertexOffset = 0;
        Mesh.VertexCount = Result.VertexCount;
        Mesh.TextureId = 0;
        Result.MeshArray[0] = Mesh;

        Result.VertexArray = ModelVertices;
        Result.IndexArray = ModelIndices;
        Result.GpuVertexBuffer = renderer->Dx12CreateBufferAsset(Result.VertexCount * sizeof(vertex), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, ModelVertices);
        Result.GpuIndexBuffer = renderer->Dx12CreateBufferAsset(Result.IndexCount * sizeof(int), D3D12_RESOURCE_STATE_INDEX_BUFFER, ModelIndices);
    }

    return Result;
}

model AssetLoadModel(Renderer* renderer, const char* FolderPath, const char* FileName)
{
    model Result = {};

    std::string FilePath = "data/sponza/Sponza.gltf";

    Assimp::Importer Importer;
    const aiScene* Scene = Importer.ReadFile(FilePath, aiProcess_Triangulate | aiProcess_OptimizeMeshes);
    if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
    {
        const char* Error = Importer.GetErrorString();
        InvalidCodePath;
    }

    u32* TextureMappingTable = (u32*)malloc(sizeof(u32) * Scene->mNumMaterials);
    for (u32 MaterialId = 0; MaterialId < Scene->mNumMaterials; ++MaterialId)
    {
        aiMaterial* CurrMaterial = Scene->mMaterials[MaterialId];

        if (CurrMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            TextureMappingTable[MaterialId] = Result.NumTextures;
            Result.NumTextures += 1;
        }
        else
        {
            TextureMappingTable[MaterialId] = 0xFFFFFFFF;
        }
    }

    Result.TextureArray = (texture*)malloc(sizeof(texture) * Result.NumTextures);

    u32 CurrTextureId = 0;
    for (u32 MaterialId = 0; MaterialId < Scene->mNumMaterials; ++MaterialId)
    {
        aiMaterial* CurrMaterial = Scene->mMaterials[MaterialId];

        if (CurrMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString TextureName = {};
            CurrMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &TextureName);

            texture* CurrTexture = Result.TextureArray + CurrTextureId;

            char* TexturePath = CombineStrings("data/sponza/", TextureName.C_Str());

            i32 NumChannels = 0;
            u32* UnFlippedTexels = (u32*)stbi_load(TexturePath, &CurrTexture->Width, &CurrTexture->Height, &NumChannels, 4);

            CurrTexture->Texels = (int*)malloc(sizeof(u32) * CurrTexture->Width * CurrTexture->Height);

            for (u32 Y = 0; Y < CurrTexture->Height; ++Y)
            {
                for (u32 X = 0; X < CurrTexture->Width; ++X)
                {
                    u32 PixelId = Y * CurrTexture->Width + X;
                    CurrTexture->Texels[PixelId] = UnFlippedTexels[(CurrTexture->Height - Y - 1) * CurrTexture->Width + X];
                }
            }

            // NOTE: ������� ���� �� Upload Heap
            renderer->Dx12CreateTexture(CurrTexture->Width, CurrTexture->Height, (u8*)CurrTexture->Texels,
                &CurrTexture->GpuTexture, &CurrTexture->GpuDescriptor);

            stbi_image_free(UnFlippedTexels);

            CurrTextureId += 1;
        }
    }

    Result.NumMeshes = Scene->mNumMeshes;
    Result.MeshArray = (mesh*)malloc(sizeof(mesh) * Result.NumMeshes);
    for (u32 MeshId = 0; MeshId < Result.NumMeshes; ++MeshId)
    {
        aiMesh* SrcMesh = Scene->mMeshes[MeshId];
        mesh* DstMesh = Result.MeshArray + MeshId;

        DstMesh->TextureId = TextureMappingTable[SrcMesh->mMaterialIndex];
        DstMesh->IndexOffset = Result.IndexCount;
        DstMesh->VertexOffset = Result.VertexCount;
        DstMesh->IndexCount = SrcMesh->mNumFaces * 3;
        DstMesh->VertexCount = SrcMesh->mNumVertices;

        Result.IndexCount += DstMesh->IndexCount;
        Result.VertexCount += DstMesh->VertexCount;
    }

    Result.VertexArray = (vertex*)malloc(sizeof(vertex) * Result.VertexCount);
    Result.IndexArray = (int*)malloc(sizeof(u32) * Result.IndexCount);

    f32 MinDistAxis = FLT_MAX;
    f32 MaxDistAxis = FLT_MIN;
    for (u32 MeshId = 0; MeshId < Result.NumMeshes; ++MeshId)
    {
        aiMesh* SrcMesh = Scene->mMeshes[MeshId];
        mesh* DstMesh = Result.MeshArray + MeshId;

        for (u32 VertexId = 0; VertexId < DstMesh->VertexCount; ++VertexId)
        {
            vertex* CurrVertex = Result.VertexArray + DstMesh->VertexOffset + VertexId;
            CurrVertex->Pos = V3(SrcMesh->mVertices[VertexId].x,
                SrcMesh->mVertices[VertexId].y,
                SrcMesh->mVertices[VertexId].z);

            MinDistAxis = std::min(MinDistAxis, std::min(CurrVertex->Pos.x, std::min(CurrVertex->Pos.y, CurrVertex->Pos.z)));
            MaxDistAxis = max(MaxDistAxis, max(CurrVertex->Pos.x, max(CurrVertex->Pos.y, CurrVertex->Pos.z)));

            CurrVertex->Normal = V3(SrcMesh->mNormals[VertexId].x,
                SrcMesh->mNormals[VertexId].y,
                SrcMesh->mNormals[VertexId].z);

            if (SrcMesh->mTextureCoords[0])
            {
                CurrVertex->Uv = V2(SrcMesh->mTextureCoords[0][VertexId].x,
                    SrcMesh->mTextureCoords[0][VertexId].y);
            }
            else
            {
                CurrVertex->Uv = V2(0, 0);
            }
        }

        for (u32 FaceId = 0; FaceId < SrcMesh->mNumFaces; ++FaceId)
        {
            int* CurrIndices = Result.IndexArray + DstMesh->IndexOffset + FaceId * 3;

            CurrIndices[0] = SrcMesh->mFaces[FaceId].mIndices[0];
            CurrIndices[1] = SrcMesh->mFaces[FaceId].mIndices[1];
            CurrIndices[2] = SrcMesh->mFaces[FaceId].mIndices[2];
        }
    }

    for (u32 VertexId = 0; VertexId < Result.VertexCount; ++VertexId)
    {
        vertex* CurrVertex = Result.VertexArray + VertexId;

        // NOTE: ��������� � �� [0, 1]
        CurrVertex->Pos = (CurrVertex->Pos - V3(MinDistAxis)) / (MaxDistAxis - MinDistAxis);
        CurrVertex->Pos = CurrVertex->Pos - V3(0.5f);
    }

    Result.GpuVertexBuffer = renderer->Dx12CreateBufferAsset(Result.VertexCount * sizeof(vertex), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, Result.VertexArray);
    Result.GpuIndexBuffer = renderer->Dx12CreateBufferAsset(Result.IndexCount * sizeof(u32), D3D12_RESOURCE_STATE_INDEX_BUFFER, Result.IndexArray);

    return Result;
}