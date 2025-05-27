#include "MarkovPrefetcher.hpp"

void MarkovPrefetcher::processLoad(uint64_t addr, uint64_t /*ip*/) {
    loads++;
    if (cache.contains(addr)) {
        if (prefetched.erase(addr)) prefetch_hits++;
    } else {
        misses++;
        cache.insert(addr);
    }

    auto it = transitions.find(addr);
    if (it != transitions.end() && !it->second.empty()) {
        uint64_t predicted = 0;
        size_t max_count = 0;
        for (const auto& [next, count] : it->second) {
            if (count > max_count) {
                predicted = next;
                max_count = count;
            }
        }
        if (predicted && !cache.contains(predicted)) {
            cache.insert(predicted);
            prefetched.insert(predicted);
            prefetches++;
        }
    }

    if (last_addr != 0) {
        transitions[last_addr][addr]++;
    }
    last_addr = addr;
}
