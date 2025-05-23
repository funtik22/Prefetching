#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <vector>
#include <string>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <iomanip>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cstdint>
using namespace std;

// ===== Cache (LRU) =====
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

    void insert(uint64_t addr) {
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

    void erase(uint64_t addr) {
        auto it = lru_map.find(addr);
        if (it != lru_map.end()) {
            lru_list.erase(it->second);
            lru_map.erase(it);
        }
    }
};

// ===== PrefetcherBase (abstract) =====
class PrefetcherBase {
protected:
    size_t loads = 0, prefetches = 0, prefetch_hits = 0, misses = 0;
public:
    virtual void processLoad(uint64_t addr, uint64_t ip) = 0;
    void printStats() const  {
        std::cout << name() << ":\n";
        std::cout << "  Loads: " << loads << "\n";
        std::cout << "  Misses: " << misses << "\n";
        std::cout << "  Prefetches: " << prefetches << "\n";
        std::cout << "  Prefetch hits: " << prefetch_hits << "\n";
        std::cout << "  Useless prefetches: " << (prefetches - prefetch_hits) << "\n";
        double accuracy = prefetches ? static_cast<double>(prefetch_hits) / prefetches : 0.0;
        double coverage = misses ? static_cast<double>(prefetch_hits) / (misses+prefetch_hits) : 0.0;
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "  Accuracy: " << (accuracy * 100.0) << "%\n";
        std::cout << "  Coverage: " << (coverage * 100.0) << "%\n";
    }
    virtual std::string name() const = 0;
    virtual ~PrefetcherBase() = default;
};

// ===== StridedPrefetcher =====
class StridedPrefetcher : public PrefetcherBase {
private:
    std::unordered_map<uint64_t, std::pair<uint64_t, int64_t>> lastInfo;
    Cache& cache;
    std::unordered_set<uint64_t> prefetched;
public:
    explicit StridedPrefetcher(Cache& shared_cache) : cache(shared_cache) {}

    void processLoad(uint64_t addr, uint64_t ip) override {
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

    std::string name() const override { return "Strided Prefetcher"; }
};

// ===== MarkovPrefetcher =====
class MarkovPrefetcher : public PrefetcherBase {
private:
    std::unordered_map<uint64_t, std::unordered_map<uint64_t, size_t>> transitions;
    Cache& cache;
    std::unordered_set<uint64_t> prefetched;
    uint64_t last_addr = 0;
public:
    explicit MarkovPrefetcher(Cache& shared_cache) : cache(shared_cache) {}

    void processLoad(uint64_t addr, uint64_t /*ip*/) override {
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



    std::string name() const override { return "Markov Prefetcher"; }
};
    



std::string removeExtraSpaces(const std::string& input) {
    std::string result;
    bool inSpace = false;

    size_t start = 0;
    while (start < input.size() && std::isspace(input[start])) {
        ++start;
    }

    size_t end = input.size();
    while (end > start && std::isspace(input[end - 1])) {
        --end;
    }

    for (size_t i = start; i < end; ++i) {
        if (std::isspace(input[i])) {
            if (!inSpace) {
                result += ' ';
                inSpace = true;
            }
        } else {
            result += input[i];
            inSpace = false;
        }
    }

    return result;
}



int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " --mode=[stride|corr|both] tracefile --cache=N\n";
        return 1;
    }

    string mode_arg, tracefile, cache_arg;

    // Определим аргументы (чтобы не зависеть от порядка)
    for (int i = 1; i < 4; ++i) {
        string arg = argv[i];
        if (arg.rfind("--mode=", 0) == 0) {
            mode_arg = arg;
        } else if (arg.rfind("--cache=", 0) == 0) {
            cache_arg = arg;
        } else {
            tracefile = arg;
        }
    }

    if (mode_arg.empty() || cache_arg.empty() || tracefile.empty()) {
        cerr << "Error: missing required parameters.\n";
        cerr << "Usage: " << argv[0] << " --mode=[stride|corr|both] tracefile --cache=N\n";
        return 1;
    }

    string mode = mode_arg.substr(7);
    bool useStride = (mode == "stride" || mode == "both");
    bool useCorr   = (mode == "corr"   || mode == "both");

    if (!useStride && !useCorr) {
        cerr << "Unknown mode: " << mode << "\n";
        return 1;
    }

    size_t cache_size = 0;
    try {
        cache_size = stoul(cache_arg.substr(8));
    } catch (...) {
        cerr << "Invalid cache size: " << cache_arg << "\n";
        return 1;
    }

    ifstream infile(tracefile);
    if (!infile) {
        cerr << "Failed to open trace file: " << tracefile << "\n";
        return 1;
    }

    Cache shared_cache(cache_size);
    StridedPrefetcher sp(shared_cache);
    MarkovPrefetcher cp(shared_cache);
    vector<PrefetcherBase*> prefetchers;
    if (useStride) prefetchers.push_back(&sp);
    if (useCorr)   prefetchers.push_back(&cp);

    uint64_t last_ip = 0;
    string line;
    while (getline(infile, line)) {
        if (line.empty()) continue;
        line = removeExtraSpaces(line);
        char type = line[0];
        size_t pos = line.find(' ');
        size_t comma = line.find(',', pos+1);
        if (type == 'I') {
            string addr_str = line.substr(pos+1, comma - (pos+1));
            last_ip = stoull(addr_str, nullptr, 16);
        } else if (type == 'L') {
            string addr_str = line.substr(pos+1, comma - (pos+1));
            uint64_t addr = stoull(addr_str, nullptr, 16);
            for (auto pf : prefetchers) {
                pf->processLoad(addr, last_ip);
            }
        }
    }

    for (auto pf : prefetchers) {
        pf->printStats();
    }

    return 0;
}