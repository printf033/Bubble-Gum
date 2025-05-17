#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <stb/stb_image.h>
#include "mesh.hpp"
#include "converter.hpp"

#ifndef MODEL_HPP
#define MODEL_HPP

struct Hierarchy
{
    int id = -1;
    glm::mat4 offset = glm::mat4(1.0f);
    std::string name;
    std::vector<Hierarchy *> children;
};

class Model
{
    std::filesystem::path path_;
    Hierarchy *root_;
    std::vector<Mesh> meshes_;
    std::vector<Texture> texturesLoaded_;
    std::unordered_map<std::string, Hierarchy> bonesLoaded_;

public:
    Model(const std::filesystem::path &path)
        : path_(path),
          root_(nullptr)
    {
        Assimp::Importer importer;
        const aiScene *paiScene = importer.ReadFile(path_,
                                                    aiProcess_CalcTangentSpace |
                                                        aiProcess_Triangulate |
                                                        aiProcess_GenSmoothNormals |
                                                        aiProcess_LimitBoneWeights |
                                                        aiProcess_JoinIdenticalVertices |
                                                        aiProcess_ConvertToLeftHanded);
        assert(paiScene != nullptr &&
               !(paiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) &&
               paiScene->mRootNode != nullptr);
        processNode(root_, paiScene->mRootNode, paiScene);
    }
    ~Model()
    {
        root_ = nullptr;
        for (auto &texture : texturesLoaded_)
            glDeleteTextures(1, &texture.id);
    }
    Model(const Model &) = delete;
    Model &operator=(const Model &) = delete;
    Model(Model &&other)
        : path_(std::move(other.path_)),
          root_(other.root_),
          meshes_(std::move(other.meshes_)),
          texturesLoaded_(std::move(other.texturesLoaded_)),
          bonesLoaded_(std::move(other.bonesLoaded_))
    {
        other.root_ = nullptr;
    }
    Model &operator=(Model &&other)
    {
        if (this != &other)
            Model(std::move(other)).swap(*this);
        return *this;
    }
    inline std::filesystem::path getPath() const { return path_; }
    inline std::unordered_map<std::string, Hierarchy> &getBonesLoaded() { return bonesLoaded_; }
    inline Hierarchy *getRootHierarchy() const { return root_; }
    void draw(Shader &shader) const
    {
        for (auto &mesh : meshes_)
            mesh.draw(shader);
    }

private:
    void swap(Model &other)
    {
        std::swap(path_, other.path_);
        std::swap(root_, other.root_);
        std::swap(meshes_, other.meshes_);
        std::swap(texturesLoaded_, other.texturesLoaded_);
        std::swap(bonesLoaded_, other.bonesLoaded_);
    }
    void processNode(Hierarchy *&node, aiNode *paiNode, const aiScene *paiScene)
    {
        assert(paiNode != nullptr);
        assert(paiScene != nullptr);
        std::string nodeName(paiNode->mName.C_Str());
        if (!bonesLoaded_.contains(nodeName))
            bonesLoaded_.emplace(nodeName,
                                 Hierarchy{static_cast<int>(bonesLoaded_.size()),
                                           Converter::convertMatrix2GLMFormat(paiNode->mTransformation),
                                           nodeName});
        node = &bonesLoaded_.at(nodeName);
        for (unsigned int i = 0; i < paiNode->mNumMeshes; ++i)
        {
            aiMesh *paiMesh = paiScene->mMeshes[paiNode->mMeshes[i]];
            meshes_.push_back(processMesh(paiMesh, paiScene));
        }
        bonesLoaded_.at(nodeName).children.reserve(paiNode->mNumChildren);
        for (unsigned int i = 0; i < paiNode->mNumChildren; ++i)
        {
            Hierarchy *child = nullptr;
            processNode(child, paiNode->mChildren[i], paiScene);
            bonesLoaded_.at(nodeName).children.push_back(child);
        }
    }
    Mesh processMesh(aiMesh *paiMesh, const aiScene *paiScene)
    {
        assert(paiMesh != nullptr);
        assert(paiScene != nullptr);
        std::vector<Vertex> vertices = processVertices(paiMesh);
        std::vector<unsigned int> indices = processIndices(paiMesh);
        std::vector<Texture> textures = processTextures(paiMesh, paiScene);
        return Mesh(vertices, indices, textures);
    }
    std::vector<Vertex> processVertices(aiMesh *paiMesh)
    {
        assert(paiMesh != nullptr);
        std::vector<Vertex> vertices;
        vertices.resize(paiMesh->mNumVertices);
        for (unsigned int i = 0; i < paiMesh->mNumVertices; ++i)
        {
            vertices[i].position = Converter::getGLMVec(paiMesh->mVertices[i]);
            if (paiMesh->HasNormals())
                vertices[i].normal = Converter::getGLMVec(paiMesh->mNormals[i]);
            else
                vertices[i].normal = glm::vec3(0.0f, 0.0f, 0.0f);
            if (paiMesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                vec.x = paiMesh->mTextureCoords[0][i].x;
                vec.y = paiMesh->mTextureCoords[0][i].y;
                vertices[i].texCoords = vec;
            }
            else
                vertices[i].texCoords = glm::vec2(0.0f, 0.0f);
            if (paiMesh->mTangents)
                vertices[i].tangent = Converter::getGLMVec(paiMesh->mTangents[i]);
            else
                vertices[i].tangent = glm::vec3(0.0f, 0.0f, 0.0f);
            if (paiMesh->mBitangents)
                vertices[i].bitangent = Converter::getGLMVec(paiMesh->mBitangents[i]);
            else
                vertices[i].bitangent = glm::vec3(0.0f, 0.0f, 0.0f);
            for (int j = 0; j < MAX_BONE_INFLUENCE; ++j)
                vertices[i].boneIDs[j] = -1;
        }
        for (unsigned int i = 0; i < paiMesh->mNumBones; ++i)
        {
            auto curBone = paiMesh->mBones[i];
            std::string boneName(curBone->mName.C_Str());
            if (!bonesLoaded_.contains(boneName))
                bonesLoaded_.emplace(boneName,
                                     Hierarchy{static_cast<int>(bonesLoaded_.size()),
                                               Converter::convertMatrix2GLMFormat(curBone->mOffsetMatrix),
                                               boneName});
            for (unsigned int j = 0; j < curBone->mNumWeights; ++j)
            {
                auto curWeight = curBone->mWeights[j];
                auto vertexId = curWeight.mVertexId;
                for (int k = 0; k < MAX_BONE_INFLUENCE; ++k)
                    if (vertices[vertexId].boneIDs[k] == -1)
                    {
                        vertices[vertexId].boneIDs[k] = bonesLoaded_[boneName].id;
                        vertices[vertexId].weights[k] = curWeight.mWeight;
                        break;
                    }
            }
        }
        for (auto &vertex : vertices) // 归一化权重（有无必要
        {
            float totalWeight = 0.0f;
            for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
                if (vertex.boneIDs[i] != -1)
                    totalWeight += vertex.weights[i];
                else
                    break;
            if (totalWeight > 0.0f)
                for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
                    if (vertex.boneIDs[i] != -1)
                        vertex.weights[i] /= totalWeight;
                    else
                        break;
        }
        return vertices;
    }
    std::vector<unsigned int> processIndices(aiMesh *paiMesh)
    {
        assert(paiMesh != nullptr);
        std::vector<unsigned int> indices;
        indices.reserve(paiMesh->mNumFaces * 3);
        for (unsigned int i = 0; i < paiMesh->mNumFaces; ++i)
            for (unsigned int j = 0; j < paiMesh->mFaces[i].mNumIndices; ++j)
                indices.push_back(paiMesh->mFaces[i].mIndices[j]);
        return indices;
    }
    std::vector<Texture> processTextures(aiMesh *paiMesh, const aiScene *paiScene)
    {
        assert(paiMesh != nullptr);
        assert(paiScene != nullptr);
        std::vector<Texture> textures;
        aiMaterial *paiMaterial = paiScene->mMaterials[paiMesh->mMaterialIndex];
        loadMaterialTextures(textures, paiMaterial, aiTextureType_DIFFUSE, "texture_diffuse");
        loadMaterialTextures(textures, paiMaterial, aiTextureType_SPECULAR, "texture_specular");
        loadMaterialTextures(textures, paiMaterial, aiTextureType_HEIGHT, "texture_normal");
        loadMaterialTextures(textures, paiMaterial, aiTextureType_AMBIENT, "texture_height");
        return textures;
    }
    void loadMaterialTextures(std::vector<Texture> &textures, aiMaterial *paiMaterial, aiTextureType paiTextureType, const std::string &typeName)
    {
        assert(paiMaterial != nullptr);
        unsigned int num = paiMaterial->GetTextureCount(paiTextureType);
        textures.reserve(textures.size() + num);
        for (unsigned int i = 0; i < num; ++i)
        {
            aiString path;
            paiMaterial->GetTexture(paiTextureType, i, &path);
            bool skip = false;
            for (unsigned int j = 0; j < texturesLoaded_.size(); ++j)
                if (std::strcmp(texturesLoaded_[j].path.c_str(), path.C_Str()) == 0)
                {
                    textures.push_back(texturesLoaded_[j]);
                    skip = true;
                    break;
                }
            if (!skip)
            {
                Texture texture;
                texture.id = TextureFromFile(path.C_Str());
                texture.type = typeName;
                texture.path = path.C_Str();
                textures.push_back(texture);
                texturesLoaded_.push_back(texture);
            }
        }
    }
    unsigned int TextureFromFile(const char *filename)
    {
        assert(filename != nullptr);
        std::string path(path_.parent_path() / filename);
        unsigned int textureID;
        glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
        int width, height, comp;
        unsigned char *pImage = stbi_load(path.c_str(), &width, &height, &comp, 0);
        assert(pImage != nullptr);
        GLenum format;
        switch (comp)
        {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            assert(false);
        }
        glTextureStorage2D(textureID, 1 + static_cast<int>(std::log2(std::max(width, height))), format == GL_RGBA ? GL_RGBA8 : GL_RGB8, width, height);
        glTextureSubImage2D(textureID, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, pImage);
        glGenerateTextureMipmap(textureID);
        glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(pImage);
        return textureID;
    }
};

#endif