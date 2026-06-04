#include "graphics/Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

Model::Model()
{
}

bool Model::loadFromFile(const char* path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace
    );

    if (scene == nullptr || scene->mNumMeshes == 0)
    {
        std::cout << "Failed to load model: " << path << std::endl;
        std::cout << importer.GetErrorString() << std::endl;
        return false;
    }


    renderContext.initFromAssimpMesh(scene->mMeshes[0]);

    return true;
}

void Model::draw()
{
    Core::DrawContext(renderContext);
}