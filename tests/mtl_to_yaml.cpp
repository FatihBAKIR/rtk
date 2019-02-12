 //
// Created by fatih on 2/11/19.
//

#include <yaml-cpp/yaml.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <iostream>

YAML::Node val2yaml(const aiColor3D& v)
{
    YAML::Node n;
    n = std::vector<float>{
         v.r, v.g, v.b
    };
    return n;
}

YAML::Node val2yaml(const aiVector3D& v)
{
    YAML::Node n;
    n = std::vector<float>{
            v.x, v.y, v.z
    };
    return n;
}

YAML::Node val2yaml(const aiString& v)
{
    YAML::Node n;
    n = v.C_Str();
    return n;
}

YAML::Node mat2yaml(const aiMaterial* mat)
{
    YAML::Node res;

    aiString str;
    auto r = mat->Get(AI_MATKEY_NAME, str);
    if (r == aiReturn_SUCCESS)
        res["name"] = val2yaml(str);

    aiColor3D diffuse;
    r = mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
    if (r == aiReturn_SUCCESS)
        res["diffuse"] = val2yaml(diffuse);

    aiColor3D specular;
    r = mat->Get(AI_MATKEY_COLOR_SPECULAR, specular);
    if (r == aiReturn_SUCCESS)
        res["specular"] = val2yaml(specular);

    aiColor3D ambient;
    r = mat->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
    if (r == aiReturn_SUCCESS)
        res["ambient"] = val2yaml(ambient);

    aiString tex;
    r = mat->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), tex);
    if (r == aiReturn_SUCCESS)
        res["diffuse_map"] = tex.C_Str();

    r = mat->Get(AI_MATKEY_TEXTURE_SPECULAR(0), tex);
    if (r == aiReturn_SUCCESS)
        res["specular_map"] = tex.C_Str();

    r = mat->Get(AI_MATKEY_TEXTURE_HEIGHT(0), tex);
    std::string nm = tex.C_Str();
    nm = nm.substr(0, nm.find(' '));
    if (r == aiReturn_SUCCESS)
        res["normal_map"] = nm;

    ai_real phong{};
    r = mat->Get(AI_MATKEY_REFRACTI, phong);
    if (r == aiReturn_SUCCESS)
        res["phong_exponent"] = phong;

    return res;
}

int main(int argc, char** argv)
{
    Assimp::Importer importer;
    auto scene = importer.ReadFile(argv[1], 0);

    YAML::Node root;
    for (int i = 0; i < scene->mNumMaterials; ++i)
    {
        auto mat = scene->mMaterials[i];
        auto yml = mat2yaml(mat);
        root[yml["name"].as<std::string>()] = yml;
    }
    std::cout << root << '\n';
}
