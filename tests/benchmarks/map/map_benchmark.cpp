#include <map>
#include <random>
#include <unordered_map>
#include <vector>

#include <benchmark/benchmark.h>
#include <boost/container/flat_map.hpp>

inline static std::vector<int> create_random_ids(size_t num_numbers) {
    std::vector<int> result(num_numbers);
    std::mt19937 rng(123);
    std::generate(result.begin(), result.end(), rng);
    return result;
}

inline static std::vector<int> create_ascending_ids(size_t num_numbers) {
    std::vector<int> result(num_numbers);
    std::generate(result.begin(), result.end(), [] {
        static int i = 0;
        return ++i;
    });
    return result;
}

inline static std::vector<std::pair<int, int>> create_pairs(const std::vector<int> ids) {
    std::vector<std::pair<int, int>> result;
    result.reserve(ids.size());
    std::transform(ids.begin(), ids.end(), std::back_inserter(result),
                   [](const int i) { return std::make_pair(i, 1); });
    return result;
}

static const auto RANGE_MAX = 2 << 22;
static const auto ASCENDING_IDS = create_ascending_ids(RANGE_MAX);
static const auto RANDOM_IDS = create_random_ids(RANGE_MAX);
static const auto ASCENDING_PAIRS = create_pairs(ASCENDING_IDS);
static const auto RANDOM_PAIRS = create_pairs(RANDOM_IDS);

// ascending insertion


template <typename C>
inline void map_insertion(C& container,
                          const std::vector<std::pair<int, int>>& pairs,
                          std::size_t num_entries) {
    for (auto i = 0u; i < num_entries; ++i) {
        container.emplace(pairs[i].first, pairs[i].second);
    }
}

static void ascending_insertion__stdmap(benchmark::State& state) {
    for (auto _: state) {
        std::map<int, int> container;
        map_insertion(container, ASCENDING_PAIRS, static_cast<std::size_t>(state.range(0)));
    }
}
BENCHMARK(ascending_insertion__stdmap)->RangeMultiplier(4)->Range(2 << 18, RANGE_MAX);

static void ascending_insertion__stdunordered(benchmark::State& state) {
    for (auto _: state) {
        std::unordered_map<int, int> container(static_cast<std::size_t>(state.range(0)));
        map_insertion(container, ASCENDING_PAIRS, static_cast<std::size_t>(state.range(0)));
    }
}
BENCHMARK(ascending_insertion__stdunordered)->RangeMultiplier(4)->Range(2 << 18, RANGE_MAX);


static void ascending_insertion__flatmap(benchmark::State& state) {
    for (auto _: state) {
        boost::container::flat_map<int, int> container;
        map_insertion(container, ASCENDING_PAIRS, static_cast<std::size_t>(state.range(0)));
    }
}
BENCHMARK(ascending_insertion__flatmap)->RangeMultiplier(4)->Range(2 << 18, RANGE_MAX);


static void ascending_insertion__flatmap_ctor(benchmark::State& state) {
    for (auto _: state) {
        boost::container::flat_map<int, int> container(boost::container::ordered_unique_range_t(),
                                                       ASCENDING_PAIRS.begin(),
                                                       ASCENDING_PAIRS.begin() + state.range(0));
    }
}
BENCHMARK(ascending_insertion__flatmap_ctor)->RangeMultiplier(4)->Range(2 << 18, RANGE_MAX);


// Random insertion

static void unordered_insertion__stdmap(benchmark::State& state) {
    for (auto _: state) {
        std::map<int, int> container;
        map_insertion(container, RANDOM_PAIRS, static_cast<std::size_t>(state.range(0)));
    }
}
BENCHMARK(unordered_insertion__stdmap)->RangeMultiplier(4)->Range(2 << 18, RANGE_MAX);

static void unordered_insertion__stdunordered(benchmark::State& state) {
    for (auto _: state) {
        std::unordered_map<int, int> container(static_cast<std::size_t>(state.range(0)));
        map_insertion(container, RANDOM_PAIRS, static_cast<std::size_t>(state.range(0)));
    }
}
BENCHMARK(unordered_insertion__stdunordered)->RangeMultiplier(4)->Range(2 << 18, RANGE_MAX);


static void unordered_insertion__flatmap(benchmark::State& state) {
    for (auto _: state) {
        boost::container::flat_map<int, int> container;
        container.reserve(static_cast<std::size_t>(state.range(0)));
        map_insertion(container, RANDOM_PAIRS, static_cast<std::size_t>(state.range(0)));
    }
}
BENCHMARK(unordered_insertion__flatmap)->RangeMultiplier(4)->Range(2 << 18, RANGE_MAX >> 4);

// Container iteration

static void iteration__stdmap(benchmark::State& state) {
    for (auto _: state) {
        state.PauseTiming();
        std::map<int, int> container;
        map_insertion(container, ASCENDING_PAIRS, static_cast<std::size_t>(state.range(0)));
        state.ResumeTiming();
        for (auto const& e: container) {
            static_cast<void>(e);
        }
    }
}
BENCHMARK(iteration__stdmap)->RangeMultiplier(4)->Range(2 << 18, RANGE_MAX);

static void iteration__stdunordered(benchmark::State& state) {
    for (auto _: state) {
        state.PauseTiming();
        std::unordered_map<int, int> container(static_cast<std::size_t>(state.range(0)));
        map_insertion(container, ASCENDING_PAIRS, static_cast<std::size_t>(state.range(0)));
        for (auto const& e: container) {
            static_cast<void>(e);
        }
    }
}
BENCHMARK(iteration__stdunordered)->RangeMultiplier(4)->Range(2 << 18, RANGE_MAX);

static void iteration__flatmap(benchmark::State& state) {
    for (auto _: state) {
        state.PauseTiming();
        boost::container::flat_map<int, int> container(boost::container::ordered_unique_range_t(),
                                                       ASCENDING_PAIRS.begin(),
                                                       ASCENDING_PAIRS.begin() + state.range(0));
        state.ResumeTiming();
        for (auto const& e: container) {
            static_cast<void>(e);
        }
    }
}
BENCHMARK(iteration__flatmap)->RangeMultiplier(4)->Range(2 << 18, RANGE_MAX);

// Random Lookup

template <typename Container>
inline void lookup(const Container& container,
                   const std::vector<int>& ids,
                   std::size_t num_lookups) {
    for (std::size_t i = 0; i < num_lookups; ++i) {
        container.find(ids[i]);
    }
}

static void random_lookup__stdmap(benchmark::State& state) {
    for (auto _: state) {
        state.PauseTiming();
        std::map<int, int> container;
        map_insertion(container, ASCENDING_PAIRS, static_cast<std::size_t>(state.range(0)));
        state.ResumeTiming();
        lookup(container, RANDOM_IDS, 2 << 16);
    }
}
BENCHMARK(random_lookup__stdmap)->RangeMultiplier(4)->Range(2 << 18, RANGE_MAX);


static void random_lookup__stdunordered(benchmark::State& state) {
    const auto identifiers = create_random_ids(static_cast<std::size_t>(state.range(0) * 4));
    for (auto _: state) {
        state.PauseTiming();
        std::unordered_map<int, int> container(static_cast<std::size_t>(state.range(0)));
        map_insertion(container, ASCENDING_PAIRS, static_cast<std::size_t>(state.range(0)));
        state.ResumeTiming();
        lookup(container, RANDOM_IDS, 2 << 16);
    }
}
BENCHMARK(random_lookup__stdunordered)->RangeMultiplier(4)->Range(2 << 18, RANGE_MAX);


static void _random_lookup__flatmap(benchmark::State& state) {
    for (auto _: state) {
        state.PauseTiming();
        boost::container::flat_map<int, int> container(boost::container::ordered_unique_range_t(),
                                                       ASCENDING_PAIRS.begin(),
                                                       ASCENDING_PAIRS.begin() + state.range(0));
        state.ResumeTiming();
        lookup(container, RANDOM_IDS, 2 << 16);
    }
}
BENCHMARK(_random_lookup__flatmap)->RangeMultiplier(4)->Range(2 << 18, RANGE_MAX);


BENCHMARK_MAIN();
