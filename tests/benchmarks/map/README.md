# Benchmark maps


## Run the benchmarks and export results in a JSON file

   ```
   make
   ./map_benchmark --benchmark_out=map_benchmark.json --benchmark_out_format=json
   ```

## Create a Jupyter notebook

1. Generate Jupyter notebook from JSON file

    ```
    python google-benchmark-plot.py notebook map_benchmark.json > map_benchmark.ipynb
    ```

1. Evaluate the Jupyter notebook to generate the plots 

    ```
    jupyter nbconvert --to notebook --execute --inplace map_benchmark
    ```

## Create PNG images

```
python google-benchmark-plot.py plot map_benchmark.json
```
