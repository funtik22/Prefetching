#pragma once

#include <iostream>
#include <iomanip>
#include <stdint.h>

class PrefetcherBase {
protected:
    size_t loads = 0, prefetches = 0, prefetch_hits = 0, misses = 0;
public:
    virtual void processLoad(uint64_t addr, uint64_t ip) = 0;
    virtual std::string name() const = 0;
    virtual ~PrefetcherBase() = default;
    void printStats() const;
};