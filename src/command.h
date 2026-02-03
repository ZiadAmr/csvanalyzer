#ifndef COMMAND_H
#define COMMAND_H

#include "table.h"

#include <vector>
#include <iomanip>
#include <algorithm>
#include <map>
#include <set>
#include <functional>

class Command {
    public:
        Command() {};
        virtual void execute(const Table& table) = 0;
    private:
};

class SchemaCommand : public Command {
    public:
        SchemaCommand() {};
        
        void execute(const Table& table) override {
            // header
            std::cout   << std::left << std::setw(15) << "Columns"
                        << std::left << std::setw(10) << "Type"
                        << std::endl;
            
            std::cout << std::string(25, '-') << std::endl;

            // data
            auto names = table.getColNames();
            auto types = table.getColTypes();
            for(int i = 0; i < table.num_columns(); i++){
                std::cout   << std::left << std::setw(15) << names[i]
                            << std::left << std::setw(10) << (types[i]==ColumnType::String?"String":(types[i]==ColumnType::Int?"Integer":"Double"))
                            << std::endl;
            }
        }
    private:
};

class RowsCommand : public Command {
    public:
        RowsCommand() {};

        void execute(const Table& table) override {
            std::cout << table.num_rows() << std::endl; 
        }
    private:
};

class ColsCommand : public Command {
    public:
        ColsCommand() {};

        void execute(const Table& table) override {
            std::cout << table.num_columns() << std::endl; 
        }
    private:
};

class StatsCommand : public Command {
    public:
        StatsCommand(const std::string& col)
            : colName(col) {}
        
        void execute(const Table& table) override {

            if(!table.existsCol(colName)) throw std::runtime_error("Error: unknown column " + colName); 

            std::cout << "count: " << table.num_rows() << std::endl;
            if(table.isColNumeric(colName)){
                std::cout << "min: " << table.colMin(colName) << std::endl;
                std::cout << "max: " << table.colMax(colName) << std::endl;
                std::cout << "mean: " << table.colMean(colName) << std::endl;
            }
        }
    private:
        std::string colName;
};

class FilterCommand : public Command {
    public:
        FilterCommand(std::string colName, std::string op, std::string value)
            : colName(colName), op(op), value(value) { }
        
        void execute(const Table& table) override {
            if(!table.existsCol(colName)) throw std::runtime_error("Error: unknown column " + colName); 
            if(!validateInstruction(table)) throw std::runtime_error("Error: invalid instruction");

            const auto& data = table.getTable(); 
            int colNum = table.getColNum(colName);
            for(int i = 0; i < table.num_rows(); i++){
                std::string info = data[colNum]->getAsString(i);
                if(table.isColNumeric(colName)){
                    double a = std::stod(info);
                    double b = std::stod(value);
                    if (dComp[op](a,b)){
                        printLine(data, i);
                    }
                } else {
                    if(info == value) printLine(data, i);
                }
            }
        }

        bool validateInstruction(const Table& table){
            ColumnType colType = table.getColType(colName);

            if(operators.find(op) == operators.end()) return false;

            if(colType == ColumnType::String){
                if(op != "=") return false;
            }

            return true;
        }

        void printLine(const std::vector<std::unique_ptr<ColumnBase>>& data, int row_num){
            for(const auto& col : data){
                std::string str = col->getAsString(row_num);
                std::cout << std::left << std::setw(20) << str.substr(0, std::min(str.length(), static_cast<std::string::size_type>(15)));
            }
            std::cout << std::endl;
        }
    private:
        std::string colName;
        std::string op;
        std::string value;
        std::set<std::string> operators = {"!=", ">", "<", ">=", "<=", "="};
        std::map<std::string, std::function<bool(double,double)>>  dComp = {
            {"!=", [](double a, double b){return a != b;}},
            {">", [](double a, double b){return a > b;}},
            {"<", [](double a, double b){return a < b;}},
            {">=", [](double a, double b){return a >= b;}},
            {"<=", [](double a, double b){return a <= b;}},
            {"=", [](double a, double b){return a == b;}}
        };
        
};

class HeadCommand : public Command {
    public:
        HeadCommand(int number) 
            :number(number) {}
        
        void execute(const Table& table) override {
            int num_rows = std::min(number, table.num_rows());
            const auto& data = table.getTable();
            const auto& colNames = table.getColNames();
            for(auto colName : colNames) std::cout << std::left << std::setw(20) << colName.substr(0, std::min(colName.length(), static_cast<std::string::size_type>(15)));
            std::cout << std::endl;
            for(int row = 0; row < num_rows; row++){
                for(int col = 0; col < data.size(); col++){
                    std::string str = data[col]->getAsString(row);
                    std::cout << std::left << std::setw(20) <<  str.substr(0, std::min(str.length(),  static_cast<std::string::size_type>(15)));
                }
                std::cout << std::endl;
            }
        }
    private:
        int number;
};

class SortCommand : public Command {
    public:
        SortCommand(std::string colName, std::string state)
            :colName(colName), state(state) { }

        void execute(const Table& table) override {
            if(!table.existsCol(colName)) throw std::runtime_error("Error: unknown column " + colName); 

            const auto& data = table.getTable();
            int colNum = table.getColNum(colName);

            const auto& colNames = table.getColNames();
            for(auto colName : colNames) std::cout << std::left << std::setw(20) << colName.substr(0, std::min(colName.length(), static_cast<std::string::size_type>(15)));
            std::cout << std::endl;

            auto sortedIndices = data[colNum]->sortedIndices();

            if(state == "asc") std::reverse(sortedIndices.begin(), sortedIndices.end());

            for(int index : sortedIndices){
                for(int col = 0; col < data.size(); col++){
                    std::string str = data[col]->getAsString(index);
                    std::cout << std::left << std::setw(20) << str.substr(0, std::min(str.length(),  static_cast<std::string::size_type>(15)));
                }
                std::cout << std::endl;
            }
        }
    private:
        std::string colName;
        std::string state;
};

class GroupByCommand : public Command {
    public:
        GroupByCommand(std::string colName, std::string aggregationMode, std::string col2Name)
            : colName(colName), aggregationMode(aggregationMode), col2Name(col2Name) { }

        void execute(const Table& table) override {
            if(!table.existsCol(colName)) throw std::runtime_error("Error: unknown column " + colName); 
            if(!table.existsCol(col2Name) && aggregationMode != "count" && col2Name != "") throw std::runtime_error("Error: unknown column " + col2Name); 

            const auto& data = table.getTable();
            int colNum = table.getColNum(colName);
            int col2Num = table.getColNum(col2Name);

            std::cout << std::left << std::setw(20) << colName.substr(0, std::min(colName.length(), static_cast<std::string::size_type>(15)));
            if(col2Name == "") col2Name = "count";
            std::cout << std::left << std::setw(20) << col2Name.substr(0, std::min(col2Name.length(), static_cast<std::string::size_type>(15)));
            std::cout << std::endl;


            std::map<std::string, double> counts;
            if(aggregationMode == "count"){
                for (const std::string& val: data[colNum]->getAllAsString()) {
                    counts[val]++;
                }
            } else if(aggregationMode == "sum" && table.isColNumeric(col2Name)) {
                int i = 0;
                for(const std::string& val: data[colNum]->getAllAsString()) {
                    counts[val] += table.getColType(col2Name) == ColumnType::Int ? data[col2Num]->get<int>(i) : data[col2Num]->get<double>(i);
                    
                    i++;
                }
            } else if(aggregationMode == "min" && table.isColNumeric(col2Name)) {
                int i = 0;
                for(const std::string& val: data[colNum]->getAllAsString()) {
                    counts[val] += (counts[val] == 0) ? stod(data[col2Num]->getAsString(i)) : std::min(counts[val], stod(data[col2Num]->getAsString(i)));
                    
                    i++;
                }
            } else if(aggregationMode == "max" && table.isColNumeric(col2Name)) {
                int i = 0;
                for(const std::string& val: data[colNum]->getAllAsString()) {
                    counts[val] += std::max(counts[val], stod(data[col2Num]->getAsString(i)));
                    
                    i++;
                }
            } else if(aggregationMode == "avg" && table.isColNumeric(col2Name)) {
                int i = 0;
                for(const std::string& val: data[colNum]->getAllAsString()) {
                    counts[val] += (counts[val] + stod(data[col2Num]->getAsString(i))) / 2 ;
                    
                    i++;
                }
            }

            for(auto& [key, value]: counts){
                std::cout << std::left << std::setw(20) << key
                << std::left << std::setw(20) << value
                << std::endl;
            }
        }
    private:
        std::string colName;
        std::string aggregationMode; // count, sum, min, max, avg
        std::string col2Name;
};

class ExitCommand : public Command {
    public:
        ExitCommand() {}

        void execute(const Table& table) override {
            std::cout << "Exiting..." << std::endl;
            exit(0);
        }
    private:

};

#endif /* COMMAND_H */