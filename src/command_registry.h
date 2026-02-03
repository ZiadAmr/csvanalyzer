#ifndef COMMAND_REGISTRY_H
#define COMMAND_REGISTRY_H

#include "command.h"

#include <memory>
#include <unordered_map>
#include <functional>

using Factory = std::function<std::unique_ptr<Command>(const std::vector<std::string>& args)>;

class CommandRegistry {
    public:
        void registerCommand(const std::string& name, Factory factory) {registry[name] = factory;}

        std::unique_ptr<Command> getCommand(const std::string& name, std::vector<std::string>& args) {
            auto it = registry.find(name);
            if(it != registry.end()){
                return it->second(args);
            }

            return nullptr;
        }
    private:
        std::unordered_map<std::string, Factory> registry;
};
#endif /* COMMAND_REGISTRY_H */