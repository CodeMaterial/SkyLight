#pragma once
#include <chrono>

namespace skylight {
    uint64_t Now() {
        std::chrono::duration d = std::chrono::high_resolution_clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
    }
}