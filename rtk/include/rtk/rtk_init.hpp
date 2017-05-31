//
// Created by fatih on 19.03.2017.
//

#pragma once

#include <rtk/rtk_fwd.hpp>

namespace RTK_NAMESPACE
{
    class RTK_PUBLIC rtk_init
    {
        static int init_count;
    public:
        rtk_init();
        ~rtk_init();
    };
}

