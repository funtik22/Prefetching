#include "StridePrefetcher.hpp"

void StridePrefetcher::processLoad(uint64_t addr, uint64_t ip) {
    loads++;
    if (cache.contains(addr)) {
        if (prefetched.erase(addr)) prefetch_hits++;
    } else {
        misses++;
        cache.insert(addr);
    }

    auto& entry = lastInfo[ip];
    uint64_t last_addr = entry.first;
    int64_t last_stride = entry.second;

    if (last_addr == 0 && last_stride == 0) {
        entry.first = addr;
    } else {
        int64_t stride = static_cast<int64_t>(addr) - static_cast<int64_t>(last_addr);
        if (last_stride != 0 && stride == last_stride) {
            uint64_t next = addr + stride;
            if (!cache.contains(next)) {
                cache.insert(next);
                prefetched.insert(next);
                prefetches++;
            }
        }
        entry.first = addr;
        entry.second = stride;
    }
}