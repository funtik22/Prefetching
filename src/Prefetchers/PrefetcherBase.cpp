#include "PrefetcherBase.hpp"

void PrefetcherBase::printStats() const  {
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