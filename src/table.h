#ifndef TABLE_H
#define TABLE_H

#include "column.h"

#include <memory> 
#include <vector>
#include <iostream>
#include <any>
#include <numeric>
#include <algorithm>

class Table {
    public:
        Table(std::vector<ColumnType> types);
    
        void addRow(std::vector<std::string> row);
        void addHeader(std::vector<std::string> headers);
    
        int num_columns() const {return columns.size();}
        int num_rows() const {return columns.size() ? columns[0]->size(): 0;}

        double colMin(std::string colName) const;
        double colMax(std::string colName) const;
        double colMean(std::string colName) const;

        bool isColNumeric(std::string colName) const;
        bool existsCol(std::string colName) const;

        std::vector<std::string> getColNames() const { return column_names; }
        std::vector<ColumnType> getColTypes() const { return types; }
        const std::vector<std::unique_ptr<ColumnBase>>& getTable() const { return columns; }

        ColumnType getColType(std::string colName) const { return types[getColNum(colName)]; }
        int getColNum(const std::string& colName) const;

    private:
        std::string defaultValue(ColumnType& type);

        std::vector<std::unique_ptr<ColumnBase>> columns;
        std::vector<ColumnType> types;
        std::vector<std::string> column_names;
};

#endif /* TABLE_H */