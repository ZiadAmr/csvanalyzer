#ifndef COLUMN_H
#define COLUMN_H

#include <memory> 
#include <vector>
#include <iostream>
#include <any>
#include <numeric>
#include <algorithm>
#include <iterator>

template <typename T, bool Numeric = std::is_arithmetic_v<T>>
class Column;


enum class ColumnType {
    Int,
    Double,
    String
};

class ColumnBase {
    public:
        ColumnBase() {};
        virtual ~ColumnBase() = default;

        virtual void add(std::string& value) = 0;
        virtual int size() = 0;
        virtual std::string getAsString(int index) const = 0;
        virtual std::vector<std::string> getAllAsString() const = 0;

        virtual bool isNumeric() const = 0;
        virtual double min() const {
            throw std::runtime_error("Column is not numeric");
        }

        virtual double max() const {
            throw std::runtime_error("Column is not numeric");
        }

        virtual double mean() const {
            throw std::runtime_error("Column is not numeric");
        }

        virtual std::vector<int> sortedIndices() const = 0;
        
        template<typename T>
        const std::vector<T>& getAll() const {
            auto* col = dynamic_cast<const Column<T>*>(this);
            if(!col){
                throw std::runtime_error("Column type mismatch in getAll()");
            }
            return col->getAll();
        }

        template<typename T>
        const T& get(size_t i) const {
            auto* col = dynamic_cast<const Column<T>*>(this);
            if(!col) {
                throw std::runtime_error("Column type mismatch in getAll()");
            }
            return col->get(i);
        }

    private:

};

template <typename T> 
class Column<T, true> : public ColumnBase {
    public:
        Column() {};

        void add(std::string& value) override {
            if constexpr (std::is_same_v<T, int>) values.push_back(stoi(value));
            else if constexpr (std::is_same_v<T, double>) values.push_back(stod(value));
        }
        std::string getAsString(int index) const override {
            if(index < 0 || index >= values.size()) throw std::out_of_range("Index out of range");
            return std::to_string(values[index]);
        }

        std::vector<std::string> getAllAsString() const {
            std::vector<std::string> strVector;
            strVector.reserve(values.size());

            std::transform(values.begin(), values.end(), std::back_inserter(strVector), [](T i){
                return std::to_string(i);
            });

            return strVector;
        }

        bool isNumeric() const override { return true; }
        double min() const override {
            return *std::min_element(values.begin(), values.end()); 
        }
        double max() const override {
            return *std::max_element(values.begin(), values.end());
        }
        double mean() const override {
            return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
        }

        int size() override { return values.size(); }

        std::vector<int> sortedIndices() const override {
            std::vector<int> indices(values.size());
            std::iota(indices.begin(), indices.end(), 0);

            std::sort(indices.begin(), indices.end(), [this](const int a, const int b){
                return values[a] > values[b];
            });

            return indices;
        }

        const std::vector<T>& getAll() const { return values; }
        const T& get(size_t i) const { return values[i]; }
    private:
        std::vector<T> values;
};


template <typename T> 
class Column<T, false> : public ColumnBase { // String only 
    public:
        Column() {};

        void add(std::string& value) override {
            if constexpr (std::is_same_v<T, std::string>) values.emplace_back(value);
        }
        std::string getAsString(int index) const override {
            if(index < 0 || index >= values.size()) throw std::out_of_range("Index out of range");
            return values[index];
        }

        std::vector<std::string> getAllAsString() const override{
            return values;
        }


        bool isNumeric() const override { return false; }

        int size() override { return values.size(); }

        std::vector<int> sortedIndices() const override {
            std::vector<int> indices(values.size());
            std::iota(indices.begin(), indices.end(), 0);

            std::sort(indices.begin(), indices.end(), [this](const int a, const int b){
                return values[a] > values[b];
            });

            return indices;
        }

        const std::vector<T>& getAll() const { return values; }
        const T& get(size_t i) const { return values[i]; }
    private:
        std::vector<T> values;
};


#endif