#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <getopt.h>
#include <cstdlib>
#include <stdexcept>
#include "Prefetchers/PrefetcherBase.hpp"
#include "Prefetchers/MarkovPrefetcher.hpp"
#include "Prefetchers/StridePrefetcher.hpp"
#include "Cache/Cache.hpp"

struct Config {
    std::string mode;
    size_t cache_size = 0;
    std::string tracefile;
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

bool parseArguments(int argc, char* argv[], Config& config) {
    const struct option long_options[] = {
        {"mode", required_argument, nullptr, 'm'},
        {"cache", required_argument, nullptr, 'c'},
        {nullptr, 0, nullptr, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "m:c:", long_options, nullptr)) != -1) {
        switch (opt) {
            case 'm':
                config.mode = optarg;
                break;
            case 'c':
                try {
                    config.cache_size = std::stoul(optarg);
                } catch (...) {
                    std::cerr << "Invalid cache size: " << optarg << "\n";
                    return false;
                }
                break;
            default:
                return false;
        }
    }

    if (optind >= argc) {
        std::cerr << "Missing tracefile argument\n";
        return false;
    }

    config.tracefile = argv[optind];
    return true;
}

std::vector<PrefetcherBase*> createPrefetchers(const std::string& mode, Cache& cache) {
    std::vector<PrefetcherBase*> prefetchers;

    static StridePrefetcher sp(cache);
    static MarkovPrefetcher cp(cache);

    if (mode == "stride" || mode == "both") prefetchers.push_back(&sp);
    if (mode == "corr"   || mode == "both") prefetchers.push_back(&cp);

    if (prefetchers.empty()) {
        throw std::invalid_argument("Unknown mode: " + mode);
    }

    return prefetchers;
}

void processTrace(const std::string& filename, std::vector<PrefetcherBase*>& prefetchers) {
    std::ifstream infile(filename);
    if (!infile) {
        throw std::runtime_error("Failed to open trace file: " + filename);
    }

    uint64_t last_ip = 0;
    std::string line;

    while (getline(infile, line)) {
        if (line.empty()) continue;
        line = removeExtraSpaces(line);
        char type = line[0];
        size_t pos = line.find(' ');
        size_t comma = line.find(',', pos + 1);
        if (type == 'I') {
            std::string addr_str = line.substr(pos + 1, comma - (pos + 1));
            last_ip = std::stoull(addr_str, nullptr, 16);
        } else if (type == 'L') {
            std::string addr_str = line.substr(pos + 1, comma - (pos + 1));
            uint64_t addr = std::stoull(addr_str, nullptr, 16);
            for (auto pf : prefetchers) {
                pf->processLoad(addr, last_ip);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    Config config;

    if (!parseArguments(argc, argv, config)) {
        std::cerr << "Usage: " << argv[0] << " --mode=[stride|corr|both] --cache=N tracefile\n";
        return 1;
    }

    try {
        Cache shared_cache(config.cache_size);
        auto prefetchers = createPrefetchers(config.mode, shared_cache);
        processTrace(config.tracefile, prefetchers);

        for (auto pf : prefetchers) {
            pf->printStats();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
