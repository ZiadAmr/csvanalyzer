#include "table.h"

#include "exceptions.h"

#include <algorithm>

Table::Table(std::vector<ColumnType> types)
    :types{types}
{
    for(auto type : types){
        switch (type)
        {
            case ColumnType::Int:
                columns.push_back(std::make_unique<Column<int>>());
                break;
            case ColumnType::Double:
                columns.push_back(std::make_unique<Column<double>>());
                break;
            case ColumnType::String:
                columns.push_back(std::make_unique<Column<std::string>>());
                break;
        }
    }
}

void Table::addRow(std::vector<std::string> row)
{
    if(row.size() > column_names.size()) throw MismatchColumnLength(columns[0]->size()+1);
    for(int i = 0; i < row.size(); i++){
        columns[i]->add(row[i]);
    }
    if(row.size() < column_names.size()){
        for(int i = row.size(); i < column_names.size(); i++){
            std::string value = defaultValue(types[i]);
            columns[i]->add(value);
        }
    }
}

void Table::addHeader(std::vector<std::string> headers)
{
    for(auto header: headers) column_names.push_back(header);
}

double Table::colMin(std::string colName) const 
{
    int colNum = getColNum(colName);
    return columns[colNum]->min();   
}

double Table::colMax(std::string colName) const 
{
    int colNum = getColNum(colName);
    return columns[colNum]->max();   
}

double Table::colMean(std::string colName) const
{
    int colNum = getColNum(colName);
    return columns[colNum]->mean();   
}

bool Table::isColNumeric(std::string colName) const
{
    int colNum = getColNum(colName);
    return columns[colNum]->isNumeric();
}

bool Table::existsCol(std::string colName) const
{
    return (std::find(column_names.begin(), column_names.end(), colName) != column_names.end());
}

std::string Table::defaultValue(ColumnType &type)
{
    switch (type)
    {
        case ColumnType::Int:
            return "0";
        case ColumnType::Double:
            return "0.00";
        case ColumnType::String:
            return "";
    }
    return "";
}

int Table::getColNum(const std::string &colName) const
{
    int colNum = -1;
    for(int i = 0; i < column_names.size(); i++){
        if(column_names[i] == colName) colNum = i;
    }

    return colNum;
}
