#include <iostream>

#include <rtk/rtk_init.hpp>
#include <rtk/window.hpp>
#include <rtk/graphics/units.hpp>

#include <rtk/gl/shader.hpp>
#include <rtk/geometry/mesh.hpp>
#include <fstream>
#include <rtk/gl/program.hpp>
#include <chrono>
#include <thread>

#include <cimport.h>
#include <postprocess.h>
#include <Importer.hpp>
#include <scene.h>

std::string read_text_file(const std::string& path)
{
    std::ifstream f(path);
    return {std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
}

int main() {
    rtk::rtk_init init;

    using namespace rtk::literals;
    using namespace std::chrono_literals;

    rtk::window w({800_px, 600_px});

    Assimp::Importer importer;
    auto scene = importer.ReadFile("../assets/sponza/sponza.obj", aiProcess_Triangulate | aiProcess_GenNormals);
    if ((!scene) || (scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE) || (!scene->mRootNode))
    {
        std::cerr << "Error loading mymodel.obj: " << std::string(importer.GetErrorString()) << std::endl;
        return -1;
    }

    std::vector<rtk::geometry::mesh> meshes;

    for (int i = 0; i < scene->mNumMeshes; ++i)
    {
        auto mesh = scene->mMeshes[i];
        std::vector<glm::vec3> pos;
        std::vector<glm::vec3> color;
        std::vector<std::uint32_t> faces;

        for (unsigned int j = 0; j < mesh->mNumVertices; ++j)
        {
            glm::vec3 tempVertex;
            tempVertex.x = mesh->mVertices[j].z;
            tempVertex.y = mesh->mVertices[j].y;
            tempVertex.z = mesh->mVertices[j].x;
            pos.push_back(tempVertex / 500.f);
            color.push_back({1.0f, 0.5f, 0.2f});
        }

        for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
        {
            aiFace face = mesh->mFaces[j];
            Expects(face.mNumIndices == 3);
            for (unsigned int k = 0; k < face.mNumIndices; ++k) { faces.push_back(face.mIndices[k]); }
        }

        rtk::geometry::mesh m;
        m.set_vertices(pos);
        m.set_colors(color);
        m.set_faces(faces);
        m.load();

        meshes.push_back(std::move(m));
    }

    rtk::gl::vertex_shader vs { read_text_file("../shaders/basic.vert").c_str() };
    rtk::gl::fragment_shader fs { read_text_file("../shaders/basic.frag").c_str() };
    rtk::gl::program shader;
    shader.attach(vs);
    shader.attach(fs);
    shader.link();

    while (true)
    {
        w.begin_draw();

        for (auto& m : meshes)
        {
            m.draw(shader);
        }

        w.end_draw();
        std::this_thread::sleep_for(10ms);
    }

    return 0;
}