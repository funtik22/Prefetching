#pragma once
#include <list>
#include <unordered_map>
#include <stdint.h>

class Cache {
private:
    size_t capacity;
    std::list<uint64_t> lru_list;
    std::unordered_map<uint64_t, std::list<uint64_t>::iterator> lru_map;

public:
    explicit Cache(size_t cap) : capacity(cap) {}

    bool contains(uint64_t addr) const {
        return lru_map.find(addr) != lru_map.end();
    }

    void insert(uint64_t addr);
    void erase(uint64_t addr);
};