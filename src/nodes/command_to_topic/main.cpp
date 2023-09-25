#include <iostream>
#include "CLI11.hpp"
#include "spdlog/spdlog.h"

#include "skylight_messaging.h"

#include "skylight_message/simple_void.hpp"
#include "skylight_message/simple_string.hpp"
#include "skylight_message/simple_float.hpp"

#include <stdexcept>

typedef std::function<void(std::vector<std::string>)> command_callback;

class CommandStore {
public:
    CommandStore() = default;

    ~CommandStore() = default;

    void RegisterOption(std::string optionID, std::vector<std::string> options) {
        mOptions[optionID] = options;
    }

    void RegisterCommand(std::string command, command_callback func) {
        mCommands[command] = func;
    }

    bool CallFunc(std::string inputString) {

        for (auto const &[command, callback]: mCommands) {
            auto [matchSuccess, arguments] = Match(inputString, command);
            if (matchSuccess) {
                callback(arguments);
                return true;
            }
        }
        return false;
    }

private:

    std::pair<bool, std::vector<std::string>> Match(std::string inputString, std::string matchString) {

        std::pair<bool, std::vector<std::string>> results = {true, {}};

        int inputStringIndex = 0;
        int matchStringIndex = 0;

        while (inputStringIndex < inputString.size()) {

            if (inputString[inputStringIndex] == matchString[matchStringIndex]) {
                inputStringIndex++;
                matchStringIndex++;

            } else if (matchString[matchStringIndex] == '<') {
                std::stringstream ss;
                matchStringIndex++;
                while (matchString[matchStringIndex] != '>') {
                    ss << matchString[matchStringIndex];
                    matchStringIndex++;
                }

                for (auto const &optionString: mOptions[ss.str()]) {
                    if (inputString.substr(inputStringIndex, optionString.size()).compare(optionString) == 0) {
                        results.second.push_back(optionString);
                        inputStringIndex += static_cast<int>(optionString.size());
                        break;
                    }
                }

                if (results.second.empty()) {
                    return std::make_pair<bool, std::vector<std::string>>(false, {});
                }
            } else if (inputString[inputStringIndex] != matchString[matchStringIndex]) {
                return std::make_pair<bool, std::vector<std::string>>(false, {});
            }
        }

        return results;
    }

    std::map<std::string, command_callback> mCommands;
    std::map<std::string, std::vector<std::string>> mOptions;
};

void ExampleCallback(std::vector<std::string> args) {

    if (args.size() == 0) spdlog::info("no args");
    for (auto arg: args) {
        spdlog::info("arg: {}", arg);
    }
}

int main(int argc, char **argv) {
    CommandStore cs;

    cs.RegisterOption("number", {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten"});

    cs.RegisterCommand("set volume to <number>", ExampleCallback);

    cs.RegisterCommand("play thing", ExampleCallback);

    cs.CallFunc("set volume to two"); // succ
    cs.CallFunc("set volume to eleven"); // fail
    cs.CallFunc("play thing"); // succ



}


