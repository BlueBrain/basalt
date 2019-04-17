# Benchmark maps

## Update results in this directory

If benchmark source code is modified, or executed in another environment, here are
the instructions to regenerate the 2 following files:
* `map_benchmark.json`: JSON file written by the benchmark process
* `map_benchmark.ipynb`: Jupyter notebook that provide the plots

1. Run the benchmarks and export results in a JSON file

   ```
   make
   ./map_benchmark --benchmark_out=map_benchmark.json --benchmark_out_format=json
   ```

1. Update benchmark informations in README.md if relevant

1. Generate Jupyter notebook from JSON file and README.md

    ```
    python google-benchmark-plot.py notebook -r README.md -o map_benchmark.ipynb \
        map_benchmark.json 
    ```

1. Execute the Jupyter notebook to generate the plots

    ```
    jupyter nbconvert --to notebook --execute --inplace map_benchmark
    ```

## Create PNG images from JSON file

```
python google-benchmark-plot.py plot map_benchmark.json
```


## Benchmarks

### Context

Run on (72 X 3700 MHz CPUs) on BB5 CPU node  
CPU Caches
* L1 Data 32000000 (x 2)
* L1 Instruction 32000000 (x 2)
* L2 Unified 1024000000 (x 2)
* L3 Unified 25344000000 (x 36)

Tested types:

* *stdmap*: `std::map`
* *stdunordered*: `std::unordered_map`
* *flatmap*: `boost::container::flat_map`

Both keys and values are 32bit integers.

### ascending_insertion

#### Header

Insert an ordered collection of key/value pairs, for instance:

```cpp
std::vector<std::pair<int, int>> v{
    {1, 1},
    {2, 1},
    {3, 1}
    // ...
};
```

*flatmap_ctor* line uses `flat_map(ordered_unique_range_t, InputIterator, InputIterator)`
optimized constructor that assumes the input iterator is sorted.

### unordered_insertion

#### Header

Insert random keys in the containers.

#### Footer

We can observe that the *flat_map* keeps reordering the elements of the vector during insertions
of random values, which leads to very poor performance.   

### iteration

#### Header

Iterate over the entire collection.

### _512K random_lookup

#### Header

After creating the containers,  measure the time required to perform 512k random lookups.
