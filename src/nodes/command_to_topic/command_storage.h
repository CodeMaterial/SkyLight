#pragma once

#include <string>
#include <vector>
#include <functional>
#include <map>
#include <filesystem>


namespace skylight {

    typedef std::function<void(std::vector<std::string>, void *)> command_callback;

    class CommandStore {
    public:
        CommandStore();

        ~CommandStore();

        void RegisterOption(std::string optionID, std::vector<std::string> options);

        void RegisterCommand(std::string command, command_callback func);

        std::pair<command_callback, std::vector<std::string>> GetFunc(std::string inputString);

        void GenerateJSGF(std::filesystem::path
                          filepath);

    private:

        std::pair<bool, std::vector<std::string>> Match(std::string inputString, std::string matchString);

        std::map<std::string, command_callback> mCommands;
        std::map<std::string, std::vector<std::string>> mOptions;
    };
}