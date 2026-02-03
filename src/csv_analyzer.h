#ifndef CSV_ANALYZER
#define CSV_ANALYZER

#include <vector>
#include <string>

#include "table.h"
// Loads the CSV file and populates a 2D vector of strings

class CSVAnalyzer {
    public:
        CSVAnalyzer(std::string filename);
        int num_columns() {return m_num_columns;}
        int num_rows() {return m_num_rows;}

        Table make_table();
    private:
        std::vector< std::vector<std::string> > table;
        size_t m_num_rows = 0;
        size_t m_num_columns = 0;

        bool isInt(const std::string& s){
            try{
                std::size_t pos;
                std::stoi(s, &pos);

                return pos == s.length();
            } catch (const std::exception& e){
                return false;
            }
        }

        bool isDouble(const std::string& s){
            try{
                std::size_t pos;
                std::stod(s, &pos);

                return pos == s.length();
            } catch (const std::exception& e){
                return false;
            }
        }
};

#endif /* CSV_ANALYZER */