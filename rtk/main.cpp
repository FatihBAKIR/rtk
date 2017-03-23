#include <iostream>

#include <rtk/rtk_init.hpp>
#include <rtk/window.hpp>
#include <rtk/graphics/units.hpp>


int main() {
    rtk::rtk_init init;

    using namespace rtk::literals;

    rtk::window w({800_px, 600_px});

    for (auto& disp : rtk::get_displays())
    {
        std::cout << disp.get_name() << " " << disp.get_resolution().width << ", " << disp.get_resolution().height << '\n';
    }

    return 0;
}