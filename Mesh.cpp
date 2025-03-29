#define TINYOBJLOADER_IMPLEMENTATION
#include "Mesh.h"

#include <map>
#include <iostream>

Mesh::Mesh(std::string _path)
{
    if (LoadMesh(_path))
    {
        CreateVertexBuffer();
    }
    else
    {
        std::cout << "Failed to load mesh at " << _path << std::endl;
    }
}

Mesh::~Mesh()
{
    
}

bool Mesh::LoadMesh(std::string _path)
{     
    FILE* file = fopen_s(_path.c_str(), "r");
    if (file == NULL)
    {
        std::cerr << "Error opening model at " << _path << std::endl;
        return false;
    }

    while (1)
    {
        // Assume first word in file line is less than 128 chars
        char header[128];
        int result = fscanf_s(file, "%s", header);
        if (result == EOF)
        {
            break;
        }

        if (strcmp(header, "v") == 0)
        {
            glm::vec3 vertex;
            fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            tempVertices.push_back(vertex);
        }
        else if (strcmp(header, "vt") == 0)
        {
            glm::vec2 uv;
            fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
            tempUvs.push_back(uv);
        }
        else if (strcmp(header, "vn") == 0)
        {
            glm::vec3 normal;
            fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            tempNormals.push_back(normal);
        }
        else if (strcmp(header, "f") == 0)
        {
            std::string vert1, vert2, vert3;
            unsigned int vertIndex[3], uvIndex[3], normalIndex[3];
            int format = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertIndex[0], &uvIndex[0], &normalIndex[0], &vertIndex[1], &uvIndex[1], &normalIndex[1], &vertIndex[2], &uvIndex[2], &normalIndex[2]);
            if (format != 9)
            {
                std::cerr << "Cannot read file. Try another file format." << std::endl;
                return false;
            }

            vertIndices.push_back(vertIndex[0]);
            vertIndices.push_back(vertIndex[1]);
            vertIndices.push_back(vertIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
    }

    return true;
}

void Mesh::CreateVertexBuffer()
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}