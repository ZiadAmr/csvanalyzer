#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <string>

#include "command.h"

class CommandParser {
    public:
        CommandParser() = delete;

        static std::unique_ptr<Command> parseInput(std::string input) {
            std::string firstWord;
            size_t spacePos = input.find(' ');    
            if(spacePos != std::string::npos) firstWord = input.substr(0, spacePos);
            else firstWord = input;

            if(firstWord == "schema"){
                return std::make_unique<SchemaCommand>();
            } else if (firstWord == "rows") {
                return std::make_unique<RowsCommand>();
            } else if (firstWord == "columns") {
                return std::make_unique<ColsCommand>();
            } else if (firstWord == "exit") {
                return std::make_unique<ExitCommand>();
            }else {
                throw std::runtime_error("Unknown command");
            }
        }
    private:

};

#endif /* COMMAND_PARSER_H */