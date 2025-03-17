#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.hpp"

#ifndef MESH_HPP
#define MESH_HPP

#define MAX_BONE_INFLUENCE 4

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    int boneIDs[MAX_BONE_INFLUENCE];
    float weights[MAX_BONE_INFLUENCE];
};

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh
{
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;
    std::vector<Texture> textures_;
    unsigned int VAO_, VBO_, EBO_;

public:
    Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, const std::vector<Texture> &textures)
        : vertices_(vertices), indices_(indices), textures_(textures)
    {
        glGenVertexArrays(1, &VAO_);
        glGenBuffers(1, &VBO_);
        glGenBuffers(1, &EBO_);
        glBindVertexArray(VAO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));
        glEnableVertexAttribArray(4);
        glVertexAttribIPointer(5, MAX_BONE_INFLUENCE, GL_INT, sizeof(Vertex), (void *)offsetof(Vertex, boneIDs));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(6, MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, weights));
        glEnableVertexAttribArray(6);
        glBindVertexArray(0);
    }
    ~Mesh()
    {
        // glDeleteBuffers(1, &VBO_);
        // glDeleteBuffers(1, &EBO_);
        // glDeleteVertexArrays(1, &VAO_);
    }
    void draw(Shader &shader) const
    {
        for (unsigned int i = 0; i < textures_.size(); ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glUniform1i(glGetUniformLocation(shader.getID(), textures_[i].type.c_str()), i);
            glBindTexture(GL_TEXTURE_2D, textures_[i].id);
        }
        glBindVertexArray(VAO_);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices_.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
        GLenum error;
        while ((error = glGetError()) != GL_NO_ERROR)
        {
            std::string errorMsg;
            switch (error)
            {
            case GL_INVALID_ENUM:
                errorMsg = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                errorMsg = "GL_INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                errorMsg = "GL_INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                errorMsg = "GL_STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                errorMsg = "GL_STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                errorMsg = "GL_OUT_OF_MEMORY";
                break;
            default:
                errorMsg = "UNKNOWN";
                break;
            }
            std::cerr << errorMsg << std::endl;
        }
    }
};

#endif