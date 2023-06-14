#pragma once

#include <chrono>

namespace skylight {
    int64_t Now() {
        std::chrono::duration d = std::chrono::high_resolution_clock::now().time_since_epoch();
        return static_cast<int64_t>(std::chrono::duration_cast<std::chrono::microseconds>(d).count());
    }
}