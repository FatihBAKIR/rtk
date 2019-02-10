//
// Created by Mehmet Fatih BAKIR on 26/03/2017.
//

#include <rtk/asset/mesh_import.hpp>
#include <rtk/asset/rtk_assimp.hpp>

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

namespace rtk {
    namespace assets {
        namespace assimp
        {
            std::vector<glm::vec3> read_positions(const aiMesh* mesh)
            {
                std::vector<glm::vec3> pos;
                pos.reserve(mesh->mNumVertices);

                for (unsigned int j = 0; j<mesh->mNumVertices; ++j) {
                    glm::vec3 tempVertex;
                    tempVertex.x = mesh->mVertices[j].x;
                    tempVertex.y = mesh->mVertices[j].y;
                    tempVertex.z = mesh->mVertices[j].z;
                    pos.push_back(tempVertex);
                }

                return pos;
            }

            std::vector<std::uint32_t> read_faces(const aiMesh* mesh)
            {
                std::vector<std::uint32_t> faces;
                faces.reserve(mesh->mNumFaces);

                for (unsigned int j = 0; j<mesh->mNumFaces; ++j) {
                    aiFace face = mesh->mFaces[j];
                    //Expects(face.mNumIndices==3);
                    if (face.mNumIndices == 3)
                    {
                        for (unsigned int k = 0; k < face.mNumIndices; ++k) {
                            faces.push_back(face.mIndices[k]);
                        }
                    }
                    else if (face.mNumIndices == 4)
                    {
                        for (unsigned int k = 0; k < 3; ++k) {
                            faces.push_back(face.mIndices[k]);
                        }

                        faces.push_back(face.mIndices[2]);
                        faces.push_back(face.mIndices[3]);
                        faces.push_back(face.mIndices[0]);
                    }
                }

                return faces;
            }

            std::vector<glm::vec2> read_uvs(const aiMesh* mesh)
            {
                std::vector<glm::vec2> uvs;
                uvs.reserve(mesh->mNumVertices);

                for (unsigned int j = 0; j < mesh->mNumVertices; ++j)
                {
                    glm::vec2 uv;
                    uv.x = mesh->mTextureCoords[0][j].x;
                    uv.y = mesh->mTextureCoords[0][j].y;
                    uvs.push_back(uv);
                }

                return uvs;
            }

            std::vector<glm::vec3> read_normals(const aiMesh* mesh)
            {
                std::vector<glm::vec3> normals;
                normals.reserve(mesh->mNumVertices);

                for (unsigned int j = 0; j < mesh->mNumVertices; ++j)
                {
                    glm::vec3 normal;
                    normal.x = mesh->mNormals[j].x;
                    normal.y = mesh->mNormals[j].y;
                    normal.z = mesh->mNormals[j].z;
                    normals.push_back(normal);
                }

                return normals;
            }
        }

        static std::vector<geometry::mesh> load_meshes(const aiScene* scene)
        {
            if ((!scene) || (scene->mFlags==AI_SCENE_FLAGS_INCOMPLETE) || (!scene->mRootNode)) {
                throw std::runtime_error("couldn't load scene");
            }

            std::vector<rtk::geometry::mesh> meshes;

            for (int i = 0; i<scene->mNumMeshes; ++i) {
                auto mesh = scene->mMeshes[i];
                using namespace assimp;

                rtk::geometry::mesh m;
                m.set_vertices(read_positions(mesh));
                m.set_faces(read_faces(mesh));
                if (mesh->HasNormals()) {

                }
                if (mesh->HasTextureCoords(0)) {
                    m.set_uvs(read_uvs(mesh));
                }

                meshes.push_back(std::move(m));
            }

            return meshes;
        }

        std::vector<geometry::mesh> load_meshes(const std::string& file)
        {
            Assimp::Importer importer;
            auto scene = importer.ReadFile(file, aiProcess_JoinIdenticalVertices);
            return load_meshes(scene);
        }

        std::vector<geometry::mesh> load_meshes(const char* file_contents, size_t sz)
        {
            Assimp::Importer importer;
            auto scene = importer.ReadFileFromMemory(file_contents, sz, aiProcess_JoinIdenticalVertices);
            return load_meshes(scene);
        }
    }
}