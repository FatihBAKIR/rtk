//
// Created by fatih on 19.03.2017.
//

#pragma once

namespace RTK_NAMESPACE
{
    class display;
    class window;

    namespace geometry
    {
        class mesh;
        class path;
    }

    namespace gl
    {
        namespace shaders
        {
            struct vertex;
            struct fragment;
            struct geometry;
        }

        template <class>
        class shader;

        using vertex_shader = shader<shaders::vertex>;
        using fragment_shader = shader<shaders::fragment>;
        using geometry_shader = shader<shaders::geometry>;

        class program;
        class mesh;
        class path;

        class texture2d;
        class framebuffer;
    }
}
