#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <iostream>
#include <string>

class MismatchColumnLength : public std::exception {
    public:
        MismatchColumnLength(const int line_num)
            : m_msg(line_num) {}

        virtual const char* what() const throw () {
            return ("Error: Mismatch row length on row " + std::to_string(m_msg)).c_str();
        }

    private:
        const int m_msg;
};

class ColumnNotFound : public std::exception {
    public:
        ColumnNotFound(const std::string col_name)
            :m_col_name{col_name} {}

        virtual const char* what() const throw() {
            return ("Error: Column " + m_col_name + " not found").c_str();
        }
    private:
        const std::string m_col_name;
};

#endif /* EXCEPTIONS_H */