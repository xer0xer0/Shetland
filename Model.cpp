#define TINYOBJLOADER_IMPLEMENTATION
#include "Model.h"

#include <map>
#include <iostream>

Model::Model(std::string _path)
{
    if (LoadMesh(_path))
    {
        CreateVertexBuffer(mesh);
    }
    else
    {
        std::cout << "Failed to load mesh at " << _path << std::endl;
    }
}

Model::~Model()
{
    for (int i = 0; i < meshes.size(); i++)
    {
        glDeleteVertexArrays(1, &vertexArray);
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &elementBuffer);
    }
}

bool Model::LoadMesh(std::string _path)
{     
    const aiScene* scene = importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ASSIMP ERROR " << importer.GetErrorString() << std::endl;
        return false;
    }
    std::string directory = _path.substr(0, _path.find_last_of('/'));
    ProcessNode(scene->mRootNode, scene);

    return true;
}

void Model::ProcessNode(aiNode* _node, const aiScene* _scene)
{
    for (unsigned int i = 0; i < _node->mNumMeshes; i++)
    {
        aiMesh* aimesh = _scene->mMeshes[_node->mMeshes[i]];
        ProcessMesh(aimesh, _scene);
    }

    // Process children
    for (unsigned int i = 0; i < _node->mNumChildren; i++)
    {
        ProcessNode(_node->mChildren[i], _scene);
    }
}

Mesh Model::ProcessMesh(aiMesh* _mesh, const aiScene* _scene)
{
    Mesh processedMesh;

    // Get vertices
    for (unsigned int i = 0; i < _mesh->mNumVertices; i++)
    {
        Vertex vert;
        glm::vec3 vector3;
        
        // Convert assimp vector to glm
        vector3.x = _mesh->mVertices[i].x;
        vector3.y = _mesh->mVertices[i].y;
        vector3.z = _mesh->mVertices[i].z;
        vert.position = vector3;

        if (_mesh->HasNormals())
        {
            vector3.x = _mesh->mNormals[i].x;
            vector3.y = _mesh->mNormals[i].y;
            vector3.z = _mesh->mNormals[i].z;
            vert.normal = vector3;

            vector3.x = _mesh->mTangents[i].x;
            vector3.y = _mesh->mTangents[i].y;
            vector3.z = _mesh->mTangents[i].z;
            vert.tangent = vector3;
        }

        if (_mesh->mTextureCoords[0])
        {
            glm::vec2 vector2;
            vector2.x = _mesh->mTextureCoords[0][i].x;
            vector2.y = _mesh->mTextureCoords[0][i].y;
            vert.uv = vector2;
        }
        else
        {
            vert.uv = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vert);
    }

    // Get faces
    for (unsigned int i = 0; i < _mesh->mNumFaces; i++)
    {
        aiFace face = _mesh->mFaces[i];
        
        // Get indices
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // TODO Get material

    return processedMesh;
}

void Model::CreateVertexBuffer(Mesh _mesh)
{
    glGenVertexArrays(1, &vertexArray);
    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &elementBuffer);

    glBindVertexArray(vertexArray);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

    // positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));


    // texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    // tangents
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
}