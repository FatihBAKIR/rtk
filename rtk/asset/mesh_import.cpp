//
// Created by Mehmet Fatih BAKIR on 26/03/2017.
//

#include <rtk/asset/mesh_import.hpp>

#include <cimport.h>
#include <postprocess.h>
#include <Importer.hpp>
#include <scene.h>

namespace rtk {
    namespace assets {
        std::vector<geometry::mesh> load_meshes(const std::string& file)
        {
            Assimp::Importer importer;
            auto scene = importer.ReadFile(file, aiProcess_JoinIdenticalVertices);
            if ((!scene) || (scene->mFlags==AI_SCENE_FLAGS_INCOMPLETE) || (!scene->mRootNode)) {
                throw std::runtime_error("error loading "+file+" :"+std::string(importer.GetErrorString()));
            }

            std::vector<rtk::geometry::mesh> meshes;

            for (int i = 0; i<scene->mNumMeshes; ++i) {
                auto mesh = scene->mMeshes[i];
                std::vector<glm::vec3> pos;
                std::vector<glm::vec3> color;
                std::vector<std::uint32_t> faces;

                for (unsigned int j = 0; j<mesh->mNumVertices; ++j) {
                    glm::vec3 tempVertex;
                    tempVertex.x = mesh->mVertices[j].x;
                    tempVertex.y = mesh->mVertices[j].y;
                    tempVertex.z = mesh->mVertices[j].z;
                    pos.push_back(tempVertex);
                    color.push_back({1.0f, 0.5f, 0.2f});
                }

                for (unsigned int j = 0; j<mesh->mNumFaces; ++j) {
                    aiFace face = mesh->mFaces[j];
                    Expects(face.mNumIndices==3);
                    for (unsigned int k = 0; k<face.mNumIndices; ++k) { faces.push_back(face.mIndices[k]); }
                }

                rtk::geometry::mesh m;
                m.set_vertices(pos);
                m.set_colors(color);
                m.set_faces(faces);

                meshes.push_back(std::move(m));
            }

            return meshes;
        }
    }
}