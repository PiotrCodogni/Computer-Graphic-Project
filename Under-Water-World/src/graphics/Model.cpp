#include "graphics/Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <string>

Model::Model()
{
}

bool Model::loadFromFile(const char* path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals);

    if (!scene || scene->mNumMeshes == 0) return false;

    meshes.clear();

    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        MeshData mesh;
        mesh.context.initFromAssimpMesh(scene->mMeshes[i]);

        aiMaterial* material = scene->mMaterials[scene->mMeshes[i]->mMaterialIndex];
        aiString texturePath;

        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
        {
            std::string pathStr = texturePath.C_Str();

            if (!pathStr.empty() && pathStr[0] != '*')
            {
                size_t lastSlash = pathStr.find_last_of("/\\");
                std::string filename = (lastSlash == std::string::npos) ? pathStr : pathStr.substr(lastSlash + 1);

                std::string fullPath = "assets/texture/" + filename;
                if (!mesh.texture.loadFromFile(fullPath.c_str(), true)) {
                    std::cout << "Warning: Failed to load texture file: " << fullPath << std::endl;
                }
            }
        }
        meshes.push_back(mesh);
    }
    return true;
}

void Model::draw()
{
    for (auto& mesh : meshes)
    {
        if (mesh.texture.getId() != 0)
        {
            mesh.texture.bind(0);
        }
        Core::DrawContext(mesh.context);
    }
}
