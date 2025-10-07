#pragma once
#include <string>
#include <cstddef>

class Poscode{
    private:
        std::string data;
    public:
        Poscode();
        Poscode(std::string _data);
        char getValue(size_t i) const;   
        const std::string& getData() const; 
};
