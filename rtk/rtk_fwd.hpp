//
// Created by fatih on 19.03.2017.
//

#pragma once

namespace rtk
{
    class display;
    class window;

    namespace gl
    {
        struct vertex;
        struct fragment;

        template <class>
        class shader;

        using vertex_shader = shader<vertex>;
        using fragment_shader = shader<fragment>;

        class program;
    }
}
