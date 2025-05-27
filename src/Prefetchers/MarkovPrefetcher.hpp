#include "PrefetcherBase.hpp"
#include "../Cache/Cache.hpp"
#include <unordered_map>
#include <unordered_set>

class MarkovPrefetcher : public PrefetcherBase {
private:
    std::unordered_map<uint64_t, std::unordered_map<uint64_t, size_t>> transitions;
    Cache& cache;
    std::unordered_set<uint64_t> prefetched;
    uint64_t last_addr = 0;
public:
    explicit MarkovPrefetcher(Cache& shared_cache) : cache(shared_cache) {}
    void processLoad(uint64_t addr, uint64_t /*ip*/) override;
    std::string name() const override { return "Markov Prefetcher"; }
};