#pragma once

#include <gdr/gdr.hpp>

namespace macrogd {
struct MacroReplay : gdr::Replay<MacroReplay, gdr::Input<>> {
    MacroReplay() : gdr::Replay<MacroReplay, gdr::Input<>>("MacroGD", 2) {
        author = "HayrX_Dev";
        description = "Recorded with MacroGD";
        gameVersion = 22074;
        framerate = 240.0;
        platformer = false;
        ldm = false;
    }
};
} // namespace macrogd
