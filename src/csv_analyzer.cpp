#include "csv_analyzer.h"

#include <fstream>
#include <sstream>
#include <iostream>

CSVAnalyzer::CSVAnalyzer(std::string filename) 
{
    std::ifstream file(filename); // Open the CSV file
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file." << std::endl;
        return;
    }

    std::string line;
    // Read data, line by line
    while (std::getline(file, line)) {
        std::stringstream ss(line); // Create a stringstream from the line
        std::string field;
        std::vector<std::string> row;

        // Parse each line using comma as a delimiter
        while (std::getline(ss, field, ',')) {
            row.push_back(field);
        }

        table.push_back(row);
        m_num_rows++;

        if(row.size() > m_num_columns) m_num_columns = row.size();
    }
}

Table CSVAnalyzer::make_table()
{
    std::vector<ColumnType> types(m_num_columns, ColumnType::Int);
    bool first = true;
    for(auto row: table){
        if(first){
            first = false;
            continue;
        }
        for(int i = 0; i < row.size(); i++){
            if(isDouble(row[i])){
                if(types[i] != ColumnType::String && !isInt(row[i])) types[i] = ColumnType::Double;
            } else { // string
                types[i] = ColumnType::String;
            }
        }
    }
    // for(auto type : types) std::cout << ((type == ColumnType::String) ? "String ": "Number") << std::endl;;
    Table result{types};

    // result.printTypes();

    first = true;
    for(auto row : table){
        if(first){
            first = false;
            result.addHeader(row);
        } else {
            result.addRow(row);
        }
    }

    return result;
}
