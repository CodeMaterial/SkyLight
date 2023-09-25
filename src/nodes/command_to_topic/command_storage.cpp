#include "command_storage.h"
#include <sstream>
#include <fstream>
#include <spdlog/spdlog.h>

skylight::CommandStore::CommandStore() = default;

skylight::CommandStore::~CommandStore() = default;


void skylight::CommandStore::RegisterOption(std::string optionID, std::vector<std::string> options) {
    mOptions[optionID] = options;
}

void skylight::CommandStore::RegisterCommand(std::string command, command_callback func) {
    mCommands[command] = func;

}

std::pair<skylight::command_callback, std::vector<std::string>>
skylight::CommandStore::GetFunc(std::string inputString) {
    for (auto const &[command, callback]: mCommands) {
        auto [matchSuccess, arguments] = Match(inputString, command);
        if (matchSuccess) {
            spdlog::info("found the callback {}", command);
            return std::make_pair(callback, arguments);
        }
    }
    return {};
}


std::pair<bool, std::vector<std::string>>
skylight::CommandStore::Match(std::string inputString, std::string matchString) {

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

std::string MakeJsgfList(std::vector<std::string> stringVec) {

    std::stringstream out;
    out << "[";
    bool first = true;
    for (auto const &str: stringVec) {
        if (!first) out << " | ";
        first = false;
        out << str;
    }
    out << "]";
    return out.str();
}

void skylight::CommandStore::GenerateJSGF(std::filesystem::path filepath) {
    spdlog::info("generating new jsgf file: {}", filepath.string());
    std::ofstream ss;

    ss.open(filepath.c_str(), std::ofstream::out | std::ofstream::trunc);

    ss << "#JSGF V1.0;\n";
    ss << "grammar skylight;\n";

    // public main time
    ss << "public <main> = ";

    std::vector<std::string> commands;
    commands.reserve(mCommands.size());
    for (auto const &[command, callback]: mCommands)
        commands.push_back(command);

    ss << MakeJsgfList(commands) << ";\n";

    // right option time

    for (auto const &[option, options]: mOptions) {
        ss << "<" << option << "> = " << MakeJsgfList(options) << ";\n";
    }

    // should be good
    ss.close();

}



























