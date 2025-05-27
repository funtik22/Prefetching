#pragma once
#include "PrefetcherBase.hpp"
#include "../Cache/Cache.hpp"
#include <unordered_map>
#include <unordered_set>

class StridePrefetcher : public PrefetcherBase {
private:
    std::unordered_map<uint64_t, std::pair<uint64_t, int64_t>> lastInfo;
    Cache& cache;
    std::unordered_set<uint64_t> prefetched;
public:
    explicit StridePrefetcher(Cache& shared_cache) : cache(shared_cache) {}
    void processLoad(uint64_t addr, uint64_t ip) override;
    std::string name() const override { return "Strided Prefetcher"; }
};