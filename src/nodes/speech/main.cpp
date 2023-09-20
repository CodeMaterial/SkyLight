#include "speechNode.h"

int main(int argc, char *argv[]) {

    skylight::SpeechNode speechNode;

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}