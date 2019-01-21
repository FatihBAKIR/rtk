cmake_minimum_required(VERSION 3.7)
project(imed)

add_library(imgui
    src/imgui.cpp
    src/imgui_demo.cpp
    src/imgui_draw.cpp

		include/rtk/imgui.h
		include/rtk/imgui_internal.h
		include/rtk/imconfig.h
		include/rtk/stb_rect_pack.h
		include/rtk/stb_textedit.h
		include/rtk/stb_truetype.h
        src/imgui_plot.cpp include/rtk/imgui_plot.h)
target_include_directories(imgui PUBLIC "include")
target_include_directories(imgui PRIVATE "include/rtk")
