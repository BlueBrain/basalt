# Basalt Benchmarking Report

This report contains the results of some basalt library benchmarks between a workstation and bb5 nodes using the gpfs and nvme local storage. More precisely, the test measured the time needed for importing a neuroglial connectivity network from an **2GB H5** file to a **basalt database** and the time needed to read all the synapses and neurons from the astrocytes of the imported network. 

The workstation consists of:

| Component | Model                                      |
|-----------|--------------------------------------------|
| CPU       | Core i7 7700  @4.2GHz                      |
| Storage   | SanDisk SSD X400 2.5 7MM 512GB (X4152012)  |

## Importing Network 
The benchmarking of the network was done for four different astrocyte populations. From the same h5 file containing the full network there were loaded 1, 10, 100 and 1000 astrocytes each time and the reading benchmarks were run based on each of these distinct basalt databases.
To import a basalt database from an H5 file:
```
git clone --recursive git@github.com:BlueBrain/bbp-nixpkgs.git
source bbp-nixpkgs/sourcethis.sh
git clone --recursive git@github.com:tristan0x/basalt.git
pushd basalt
git checkout <GIT_REVISION>
nix-build -A pybasalt
result/bin/basalt-cli ngv import connectivity \
  --create-nodes \
  --max-astrocytes=1
  /gpfs/bbp.cscs.ch/project/proj16/tcarel/basalt/2019-01-28/h5_connectivity_data/neuroglial_connectivity.h5 \
  basalt-db_1
  ```
### basalt-cli arguments
- ``--max-astrocyte`` flag defines the number of astrocytes read from the H5 file
- ``/gpfs/bbp.cscs.ch/project/proj16/tcarel/basalt/2019-01-28/h5_connectivity_data/neuroglial_connectivity.h5`` is the H5 file path
### Compilers and libraries
- Compilers
	Workstation: gcc 7.3.0
	BB5: gcc 6.4.0
- Libraries
-- Rocksdb
	Workstation: 5.17.2
	BB5: 5.17.2
-- Google Benchmark
	Workstation: 1.4.0
	BB5: 1.2.0
### To import the basalt db using the nvme nodes
- ``salloc -N 1 --exclusive --partition prod --account <projXX> --constraint nvme --mem 0 --time 2:00:00`` SLURM allocation of nvme node
- ``cd /nvme/$USER/$SLURM_JOB_ID`` use this path to export the basalt database into
### Results
| Type       | Number of astrocytes | Time (s) |
|------------|----------------------|----------|
|Workstation |    1                 | 0.13001  |
|            |    10                | 0.87319  |
|            |    100               | 10.77292 |
|            |    1000              | 113.94762|
|BB5 GPFS    |    1                 | 0.178435 |
|            |    10                | 0.76435  |
|            |    100               | 9.30319  |
|            |    1000              | 111.98764|
|BB5 nvme    |    1                 | 0.10147  |
|            |    10                | 0.69634  |
|            |    100               | 9.20372  |
|            |    1000              | 100.46639|
### Conclusion
For the **2GB H5** file import:
Nvme node faster ~11% than gpfs storage system
Nvme node faster ~13% than workstation

## Read Synapses and Neurons
This part of the benchmarking focuses on the time needed to read all the synapses and neurons that are connected to the astrocytes that have been imported to the basalt database.
For writing the benchmark for this case there was used the [Google Benchmark](https://github.com/google/benchmark) library. 
The code source for this benchmark can be found at ``/tests/benchmarks/basalt_g_bench.cpp`` and is configured to run only in the nvme bb5 node. To run the benchmark in a different machine there must be a change in the directory (``db_path``) that the source codes uses to find the basalt databases. 
For all the benchmarks there was used a ``break`` command to stop the ``while`` loop that searches all the nodes(rocksdb keys) that are connected to an ``astrocyte`` and checks which ones of those are ``neurons`` or ``synapses`` and are connected to this ``astrocyte``.
To run the benchmark:
- There must be 4 basalt databases of 1, 10, 100 and 1000 astrocytes imported. The names of the basalt databases must be basalt-db_\${number_of_astrocytes} and saved in ``/nvme/$USER/$SLURM_JOB_ID/basalt-db_${number_of_astrocytes}`` to run in the nvme node.
- There are also some pre-imported basalt databases that can be copied straight to the nvme node directory to run the benchmark without importing the basalt databases from the H5 file first:
``cp -r /gpfs/bbp.cscs.ch/project/proj16/magkanar/basalt/basalt_db/* /nvme/$USER/$SLURM_JOB_ID/``
- To run the benchmark on the nvme node:
```
cd basalt
nix-shell -A pybasalt
mkdir _build_nix
cd _build_nix
cmake ..
make -j
./tests/benchmarks/gbench_basalt
```
### Results
#### BB5 nvme vs Workstation
- Compiler: gcc 7.3.0
- Libraries: 
	Rocksdb: 5.17.2
	Google Benchmark: 1.2.0
	
##### Workstation

| Benchmark             |         Time     |   CPU        |   Iterations  |
|-----------------------|------------------|--------------|---------------|
|Read_Astr_Neur/1       |    114981 ns     |  114943 ns   |  6070         |
|Read_Astr_Neur/10      |    922595 ns     |  922581 ns   |   759         |
|Read_Astr_Neur/100     |   9434409 ns     | 9434333 ns   |    76         |
|Read_Astr_Neur/1000    | 117551585 ns     |117550079 ns  |     6         |
|Read_Astr_Syn/1        |   2258358 ns     | 2258313 ns   |   302         |
|Read_Astr_Syn/10       |  14780503 ns     | 14780220 ns  |    48         |
|Read_Astr_Syn/100      | 151014843 ns     |151012791 ns  |     5         |
|Read_Astr_Syn/1000     |1564653633 ns     |1564636220 ns |     1         |

##### BB5 nvme
| Benchmark             |         Time     |   CPU        |   Iterations  |
|-----------------------|------------------|--------------|---------------|
|Read_Astr_Neur/1       |    174368 ns     |    174365 ns |  4013         |
|Read_Astr_Neur/10      |   1379976 ns     |   1379981 ns |   507         |
|Read_Astr_Neur/100     |  13847523 ns     |  13847567 ns |    51         |
|Read_Astr_Neur/1000    | 173528169 ns     | 173526244 ns |     4         |
|Read_Astr_Syn/1        |   3631554 ns     |   3631562 ns |   192         |
|Read_Astr_Syn/10       |  22871246 ns     |  22871309 ns |    30         |
|Read_Astr_Syn/100      | 227870719 ns     | 227871409 ns |     3         |
|Read_Astr_Syn/1000     |2461923412 ns     |2461916311 ns |     1         |

##### Conclusion
BB5 nvme is ~57% slower than workstation

#### BB5 nvme nix vs spack installation
- Compiler: gcc 6.4.0
- Libraries: 
	Rocksdb: 5.17.2
	Google Benchmark: 1.2.0
##### BB5 nvme nix

| Benchmark             |         Time     |   CPU        |   Iterations  |
|-----------------------|------------------|--------------|---------------|
|Read_Astr_Neur/1       |    164654 ns     |    164600 ns |  4259         |
|Read_Astr_Neur/10      |   1316381 ns     |   1316397 ns |   533         |
|Read_Astr_Neur/100     |  13186837 ns     |  13186975 ns |    53         |
|Read_Astr_Neur/1000    | 180210428 ns     | 180212534 ns |     4         |
|Read_Astr_Syn/1        |   3456526 ns     |   3456566 ns |   203         |
|Read_Astr_Syn/10       |  24400601 ns     |  24395949 ns |    28         |
|Read_Astr_Syn/100      | 229148784 ns     | 229151438 ns |     3         |
|Read_Astr_Syn/1000     |2519981970 ns     |2520012784 ns |     1         |

##### BB5 nvme spack

| Benchmark             |         Time     |   CPU        |   Iterations  |
|-----------------------|------------------|--------------|---------------|
|Read_Astr_Neur/1       |    175430 ns     |    175392 ns |  3997         |
|Read_Astr_Neur/10      |   1401569 ns     |   1401572 ns |   498         |
|Read_Astr_Neur/100     |  14127929 ns     |  14127956 ns |    49         |
|Read_Astr_Neur/1000    | 178032490 ns     | 178032562 ns |     4         |
|Read_Astr_Syn/1        |   3916723 ns     |   3916730 ns |   179         |
|Read_Astr_Syn/10       |  24838565 ns     |  24837398 ns |    28         |
|Read_Astr_Syn/100      | 244731573 ns     | 244732001 ns |     3         |
|Read_Astr_Syn/1000     |2535910357 ns     |2535907338 ns |     1         |

##### Conclusion
Spack installation and nix installation are very close. The difference is ~6% for the smaller populations where nix build is faster than spack.

#### BB5 nvme vs gpfs
- Installation with nix for both
- Compiler: gcc 6.4.0 (same as above)
- Libraries: (same as above)
	Rocksdb: 5.17.2
	Google Benchmark: 1.2.0
	
##### BB5 gpfs

| Benchmark             |         Time     |   CPU        |   Iterations  |
|-----------------------|------------------|--------------|---------------|
|Read_Astr_Neur/1       |    164786 ns     |    164786 ns |  4242         |
|Read_Astr_Neur/10      |   1316870 ns     |   1316867 ns |   529         |
|Read_Astr_Neur/100     |  13395991 ns     |  13396007 ns |    50         |
|Read_Astr_Neur/1000    | 197783921 ns     | 197783903 ns |     3         |
|Read_Astr_Syn/1        |   4749135 ns     |   4749166 ns |   147         |
|Read_Astr_Syn/10       |  28976231 ns     |  28976432 ns |    24         |
|Read_Astr_Syn/100      | 284152908 ns     | 284154570 ns |     2         |
|Read_Astr_Syn/1000     |2900400512 ns     |2899230439 ns |     1         |

##### Conclusion
For large populations of astrocytes gpfs is ~10-20% slower than the nvme storage.

### General Conclusion
There was observed a greater difference between the workstation and bb5 nvme node performance than anticipated. 
The difference between spack and nix installation doesn't have a great impact in the execution time. The difference between nix and spack is also due to the use of the jmallon allocation used by nix.
In the neuroglia network the number of synapses is a lot greater than the number of neurons (x10 times) and this of course impacts a lot the benchmark runtime.
