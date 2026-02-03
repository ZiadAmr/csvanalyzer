#include <iostream>
#include <sstream>
#include <string>

#include "csv_analyzer.h"
#include "command_parser.h"
#include "command.h"
#include "command_registry.h"

/*
TODO:
    - store registry in a seperate place
    - reading large files
    - 
*/

int main(int argc, char* argv[]) {

    if(argc == 1){
        std::cout << "Not enough arguments" << std::endl;
        return 1;
    }

    CommandRegistry registry;

    registry.registerCommand("schema", [](const std::vector<std::string>& args) -> std::unique_ptr<Command>{ 
        return std::make_unique<SchemaCommand>(); 
    });
    registry.registerCommand("exit", [](const std::vector<std::string>& args) -> std::unique_ptr<Command>{ 
        return std::make_unique<ExitCommand>(); 
    });
    registry.registerCommand("rows", [](const std::vector<std::string>& args) -> std::unique_ptr<Command>{ 
        return std::make_unique<RowsCommand>(); 
    });
    registry.registerCommand("columns", [](const std::vector<std::string>& args) -> std::unique_ptr<Command>{ 
        return std::make_unique<ColsCommand>(); 
    });
    registry.registerCommand("stats", [](const std::vector<std::string>& args) -> std::unique_ptr<Command> {
        if(args.size() != 1){
            throw std::runtime_error("Usage: stats <column>");
        }
        return std::make_unique<StatsCommand>(args[0]);
    });
    registry.registerCommand("filter", [](const std::vector<std::string>& args) -> std::unique_ptr<Command> {
        if(args.size() != 3){
            throw std::runtime_error("Usage: filter <expression>");
        }
        return std::make_unique<FilterCommand>(args[0], args[1], args[2]);
    });
    registry.registerCommand("head", [](const std::vector<std::string>& args) -> std::unique_ptr<Command> {
        if(args.size() < 1){
            throw std::runtime_error("Usage: head <number>");
        }
        if(!(!args[0].empty() && std::all_of(args[0].begin(), args[0].end(), ::isdigit))){
            throw std::runtime_error("Usage: head <number>");
        }
        return std::make_unique<HeadCommand>(stoi(args[0]));
    });
    registry.registerCommand("sort", [](const std::vector<std::string>& args) -> std::unique_ptr<Command> {
        if(args.size() != 2 || !(args[1] == "asc" || args[1] == "desc")){
            throw std::runtime_error("Usage: sort <column> [asc|desc]");
        }
        return std::make_unique<SortCommand>(args[0], args[1]);
    });
    registry.registerCommand("group-by", [](const std::vector<std::string>& args) -> std::unique_ptr<Command> {
        if(args.size() < 2 || args.size() > 3){
            throw std::runtime_error("Usage: group-by <column> [agg] <column>");
        }
        if(args[1] != "count" && args[1] != "sum" && args[1] != "min" && args[1] != "max" && args[1] != "avg"){
            throw std::runtime_error("Usage: group-by <column> [agg] <column>");
        }
        if(args[1] != "count" && args.size() <= 2){
            throw std::runtime_error("Usage: group-by <column> count");
        }

        if(args[1] == "count") return std::make_unique<GroupByCommand>(args[0], args[1], "");
        else return std::make_unique<GroupByCommand>(args[0], args[1], args[2]);
    });

    CSVAnalyzer csva(argv[1]);
    std::cout << "Raw Data: cols=" << csva.num_columns() << " rows=" << csva.num_rows() << std::endl;
    
    Table t = csva.make_table();

    std::cout << "> ";
    std::string input;
    while(std::getline(std::cin, input)){
        try{
            std::stringstream ss(input);
            std::string word;
            std::string firstWord;
            std::vector<std::string> args;

            if (ss >> firstWord) {  
                std::string word;
                while (ss >> word) {      
                    args.push_back(word);
                }
            }

            std::unique_ptr<Command> comm = registry.getCommand(firstWord, args);
            if(!comm){
                std::cout << "Unknown command " << std::endl;
            } else comm->execute(t);

            std::cout << "> ";
        } catch(std::runtime_error e){
            std::cerr << e.what() << std::endl;
            std::cout << "> ";
        } catch (std::exception e) {
            std::cerr << e.what() << std::endl;
            std::cout << "> ";
        }  
    }
    
    return 0;
}