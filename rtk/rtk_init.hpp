//
// Created by fatih on 19.03.2017.
//

#pragma once

namespace rtk
{
    class rtk_init
    {
        static int init_count;
    public:
        rtk_init();
        ~rtk_init();
    };
}

