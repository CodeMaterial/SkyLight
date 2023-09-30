#pragma once

#include <vector>

namespace skylight {

    class RGBA {
    public:
        char r = 0; // 0 - 255
        char g = 0; // 0 - 255
        char b = 0; // 0 - 255
        float a = 0.0; // 0.0 - 1.0
    };

    class Position {
    public:
        int x = 0;
        int y = 0;
        int z = 0;
    };

    class LED {
    public:
        int bufferIndex;
        RGBA rgba;
        skylight::Position position;
        bool valid = true;
    };

}