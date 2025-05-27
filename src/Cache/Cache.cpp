#include "Cache.hpp"

void Cache::insert(uint64_t addr){
    auto it = lru_map.find(addr);
    if (it != lru_map.end()) {
        lru_list.erase(it->second);
        lru_list.push_front(addr);
        lru_map[addr] = lru_list.begin();
        return;
    }

    if (lru_list.size() >= capacity) {
        uint64_t last = lru_list.back();
        lru_list.pop_back();
        lru_map.erase(last);
    }

    lru_list.push_front(addr);
    lru_map[addr] = lru_list.begin();
}

void Cache::erase(uint64_t addr) {
    auto it = lru_map.find(addr);
    if (it != lru_map.end()) {
        lru_list.erase(it->second);
        lru_map.erase(it);
    }
}