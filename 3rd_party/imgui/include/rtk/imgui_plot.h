//
// Created by fatih on 3/9/18.
//

#pragma once

#include "imgui.h"

namespace ImGui
{
    void PlotMultiHistograms(
            const char* label,
            int num_hists,
            const char** names,
            const ImColor* colors,
            float(*getter)(const void* data, int idx),
            const void * const * datas,
            int values_count,
            float scale_min = FLT_MAX,
            float scale_max = FLT_MAX,
            ImVec2 graph_size = ImVec2(0, 0));
    void PlotMultiLines(
            const char* label,
            int num_datas,
            const char** names,
            const ImColor* colors,
            float(*getter)(const void* data, int idx),
            const void * const * datas,
            int values_count,
            float scale_min,
            float scale_max,
            ImVec2 graph_size);
}