# Benchmark Reference Table

This file makes an inventory of benchmark definitions and results over time.

## Import of 2GB H5 file on BB5 nvme

* H5 file path: `.../proj16/tcarel/basalt/2019-01-28/h5_connectivity_data/neuroglial_connectivity.h5`
* SLURM Allocation: `salloc -N 1 --exclusive --partition prod --constraint nvme --mem 0 --time 2:00:00`
* Command:
   ```bash
   cd /nvme/$USER/$SLURM_JOB_ID
   git clone --recursive git@github.com:BlueBrain/bbp-nixpkgs.git
   source bbp-nixpkgs/sourcethis.sh
   git clone --recursive git@github.com:tristan0x/basalt.git
   pushd basalt
   git checkout <GIT_REVISION>
   nix-build -A pybasalt
   result/bin/basalt-cli ngv import connectivity \
     --create-nodes \
     /gpfs/bbp.cscs.ch/project/proj16/tcarel/basalt/2019-01-28/h5_connectivity_data/neuroglial_connectivity.h5 \
     basalt-db
   ```

Statistics will be available in file `basalt-db/LOG`.

Look for latest occurence of `------- DUMPING STATS -------`

### 2019-01-28

* Git revision: 83820e86bf06cd2fad5b019b2a80f6a7b7d74c51
* Context: first time the file has been imported successfully without a crash

#### Execution on Linux workstation

Report:
```json
{
  "connectivity": {
    "synapses per astrocyte": {
      "minimum": 7,
      "maximum": 61968,
      "mean": 16201
    },
    "neurons per astrocyte": {
      "minimum": 6,
      "maximum": 2871,
      "mean": 1251
    }
  },
  "duration_seconds": 2469.164788864553,
  "h5_dataset": {
    "astrocytes": 14086,
    "neurons": 219422
  },
  "processed_astrocytes": 14086
}
```

```
Cumulative writes: 23K writes, 698M keys, 23K batches, 1.0 writes per batch, ingest: 16.06 GB, 5.31 MB/s

2019/01/24-11:24:32.421767 7fe663043700 [WARN] ------- DUMPING STATS -------
2019/01/24-11:24:32.421787 7fe663043700 [WARN]
** Compaction Stats [default] **
Level    Files   Size(MB) Score Read(GB)  Rn(GB) Rnp1(GB) Write(GB) Wnew(GB) Moved(GB) W-Amp Rd(MB/s) Wr(MB/s) Comp(sec) Comp(cnt) Avg(sec) Stall(cnt)  KeyIn KeyDrop
---------------------------------------------------------------------------------------------------------------------------------------------------------------------
  L0      5/0     128.83   1.2      0.0     0.0      0.0       0.0      0.0       0.0   0.0      0.0     27.8         0         1    0.455          0       0      0
  L1     10/0     647.59   1.0      0.0     0.0      0.0       0.0      0.0       0.0   0.0      0.0      0.0         0         0    0.000          0       0      0
  L2      3/0     195.61   0.0      0.0     0.0      0.0       0.0      0.0       0.0   0.0      0.0      0.0         0         0    0.000          0       0      0
 Sum     18/0     972.03   0.0      0.0     0.0      0.0       0.0      0.0       0.0   1.0      0.0     27.8         0         1    0.455          0       0      0
 Int      0/0       0.00   0.0      0.0     0.0      0.0       0.0      0.0       0.0   1.0      0.0     27.8         0         1    0.455          0       0      0
Flush(GB): cumulative 0.012, interval 0.012
Stalls(count): 0 level0_slowdown, 0 level0_slowdown_with_compaction, 0 level0_numfiles, 0 level0_numfiles_with_compaction, 0 pending_compaction_bytes, 0 memtable_compaction, 0 leveln_slowdown_soft, 0 leveln_slowdown_hard

** Compaction Stats [connections] **
Level    Files   Size(MB) Score Read(GB)  Rn(GB) Rnp1(GB) Write(GB) Wnew(GB) Moved(GB) W-Amp Rd(MB/s) Wr(MB/s) Comp(sec) Comp(cnt) Avg(sec) Stall(cnt)  KeyIn KeyDrop
---------------------------------------------------------------------------------------------------------------------------------------------------------------------
  L0     16/0     495.55   4.0      0.0     0.0      0.0       0.0      0.0       0.0   0.0      0.0     30.6         0         1    0.408          0       0      0
  L1     41/0    2848.92   4.5      0.0     0.0      0.0       0.0      0.0       0.0   0.0      0.0      0.0         0         0    0.000          0       0      0
  L2     36/0    2561.39   0.4      0.0     0.0      0.0       0.0      0.0       0.0   0.0      0.0      0.0         0         0    0.000          0       0      0
 Sum     93/0    5905.85   0.0      0.0     0.0      0.0       0.0      0.0       0.0   1.0      0.0     30.6         0         1    0.408          0       0      0
 Int      0/0       0.00   0.0      0.0     0.0      0.0       0.0      0.0       0.0   1.0      0.0     30.6         0         1    0.408          0       0      0
Flush(GB): cumulative 0.012, interval 0.012
Stalls(count): 0 level0_slowdown, 0 level0_slowdown_with_compaction, 0 level0_numfiles, 0 level0_numfiles_with_compaction, 0 pending_compaction_bytes, 0 memtable_compaction, 0 leveln_slowdown_soft, 0 leveln_slowdown_hard

** DB Stats **
Uptime(secs): 12.2 total, 12.2 interval
Cumulative writes: 0 writes, 0 keys, 0 batches, 0.0 writes per batch, ingest: 0.00 GB, 0.00 MB/s
Cumulative WAL: 0 writes, 0 syncs, 0.00 writes per sync, written: 0.00 GB, 0.00 MB/s
Cumulative compaction: 0.01 GB write, 1.04 MB/s write, 0.00 GB read, 0.00 MB/s read, 0.5 seconds
Cumulative stall: 00:00:0.000 H:M:S, 0.0 percent
Interval writes: 0 writes, 0 keys, 0 batches, 0.0 writes per batch, ingest: 0.00 MB, 0.00 MB/s
Interval WAL: 0 writes, 0 syncs, 0.00 writes per sync, written: 0.00 MB, 0.00 MB/s
Interval compaction: 0.01 GB write, 1.04 MB/s write, 0.00 GB read, 0.00 MB/s read, 0.5 seconds
Interval stall: 00:00:0.000 H:M:S, 0.0 percent
** Level 0 read latency histogram (micros):
Count: 20  Average: 270.7000  StdDev: 381.95
Min: 0.0000  Median: 140.0000  Max: 1151.0000
Percentiles: P50: 140.00 P75: 225.00 P99: 1151.00 P99.9: 1151.00 P99.99: 1151.00
------------------------------------------------------
[       0,       1 )        6  30.000%  30.000% ######
[       5,       6 )        1   5.000%  35.000% #
[      14,      16 )        1   5.000%  40.000% #
[     120,     140 )        2  10.000%  50.000% ##
[     140,     160 )        1   5.000%  55.000% #
[     160,     180 )        3  15.000%  70.000% ###
[     200,     250 )        2  10.000%  80.000% ##
[     900,    1000 )        2  10.000%  90.000% ##
[    1000,    1200 )        2  10.000% 100.000% ##

** Level 1 read latency histogram (micros):
Count: 40  Average: 1134.8000  StdDev: 1763.66
Min: 0.0000  Median: 170.0000  Max: 4530.0000
Percentiles: P50: 170.00 P75: 300.00 P99: 4530.00 P99.9: 4530.00 P99.99: 4530.00
------------------------------------------------------
[       0,       1 )        9  22.500%  22.500% #####
[      12,      14 )        1   2.500%  25.000% #
[     120,     140 )        8  20.000%  45.000% ####
[     160,     180 )        4  10.000%  55.000% ##
[     200,     250 )        6  15.000%  70.000% ###
[     250,     300 )        2   5.000%  75.000% #
[    3500,    4000 )        3   7.500%  82.500% ##
[    4000,    4500 )        5  12.500%  95.000% ###
[    4500,    5000 )        2   5.000% 100.000% #

** Level 2 read latency histogram (micros):
Count: 12  Average: 1285.3333  StdDev: 2114.82
Min: 1.0000  Median: 212.5000  Max: 6712.0000
Percentiles: P50: 212.50 P75: 250.00 P99: 6712.00 P99.9: 6712.00 P99.99: 6712.00
------------------------------------------------------
[       0,       1 )        3  25.000%  25.000% #####
[     160,     180 )        2  16.667%  41.667% ###
[     200,     250 )        4  33.333%  75.000% #######
[    3500,    4000 )        2  16.667%  91.667% ###
[    6000,    7000 )        1   8.333% 100.000% ##

2019/01/24-11:24:32.421864 7fe663043700 [WARN] STATISTICS:
 rocksdb.block.cache.miss COUNT : 0
rocksdb.block.cache.hit COUNT : 0
rocksdb.block.cache.add COUNT : 0
rocksdb.block.cache.index.miss COUNT : 0
rocksdb.block.cache.index.hit COUNT : 0
rocksdb.block.cache.filter.miss COUNT : 0
rocksdb.block.cache.filter.hit COUNT : 0
rocksdb.block.cache.data.miss COUNT : 0
rocksdb.block.cache.data.hit COUNT : 0
rocksdb.block.cache.bytes.read COUNT : 0
rocksdb.block.cache.bytes.write COUNT : 0
rocksdb.bloom.filter.useful COUNT : 0
rocksdb.memtable.hit COUNT : 0
rocksdb.memtable.miss COUNT : 0
rocksdb.l0.hit COUNT : 0
rocksdb.l1.hit COUNT : 0
rocksdb.l2andup.hit COUNT : 0
rocksdb.compaction.key.drop.new COUNT : 0
rocksdb.compaction.key.drop.obsolete COUNT : 0
rocksdb.compaction.key.drop.user COUNT : 0
rocksdb.number.keys.written COUNT : 0
rocksdb.number.keys.read COUNT : 0
rocksdb.number.keys.updated COUNT : 0
rocksdb.bytes.written COUNT : 0
rocksdb.bytes.read COUNT : 0
rocksdb.number.db.seek COUNT : 0
rocksdb.number.db.next COUNT : 0
rocksdb.number.db.prev COUNT : 0
rocksdb.number.db.seek.found COUNT : 0
rocksdb.number.db.next.found COUNT : 0
rocksdb.number.db.prev.found COUNT : 0
rocksdb.db.iter.bytes.read COUNT : 0
rocksdb.no.file.closes COUNT : 0
rocksdb.no.file.opens COUNT : 111
rocksdb.no.file.errors COUNT : 0
rocksdb.l0.slowdown.micros COUNT : 0
rocksdb.memtable.compaction.micros COUNT : 0
rocksdb.l0.num.files.stall.micros COUNT : 0
rocksdb.stall.micros COUNT : 0
rocksdb.db.mutex.wait.micros COUNT : 4
rocksdb.rate.limit.delay.millis COUNT : 0
rocksdb.num.iterators COUNT : 0
rocksdb.number.multiget.get COUNT : 0
rocksdb.number.multiget.keys.read COUNT : 0
rocksdb.number.multiget.bytes.read COUNT : 0
rocksdb.number.deletes.filtered COUNT : 0
rocksdb.number.merge.failures COUNT : 0
rocksdb.sequence.number COUNT : 737555492
rocksdb.bloom.filter.prefix.checked COUNT : 0
rocksdb.bloom.filter.prefix.useful COUNT : 0
rocksdb.number.reseeks.iteration COUNT : 0
rocksdb.getupdatessince.calls COUNT : 0
rocksdb.block.cachecompressed.miss COUNT : 0
rocksdb.block.cachecompressed.hit COUNT : 0
rocksdb.wal.synced COUNT : 0
rocksdb.wal.bytes COUNT : 0
rocksdb.write.self COUNT : 0
rocksdb.write.other COUNT : 0
rocksdb.write.wal COUNT : 0
rocksdb.flush.write.bytes COUNT : 0
rocksdb.compact.read.bytes COUNT : 0
rocksdb.compact.write.bytes COUNT : 26373261
rocksdb.number.direct.load.table.properties COUNT : 0
rocksdb.number.superversion_acquires COUNT : 0
rocksdb.number.superversion_releases COUNT : 0
rocksdb.number.superversion_cleanups COUNT : 0
rocksdb.number.block.not_compressed COUNT : 0
rocksdb.merge.operation.time.nanos COUNT : 0
rocksdb.filter.operation.time.nanos COUNT : 0
rocksdb.row.cache.hit COUNT : 0
rocksdb.row.cache.miss COUNT : 0
rocksdb.db.get.micros statistics Percentiles :=> 50 : 0.000000 95 : 0.000000 99 : 0.000000
rocksdb.db.write.micros statistics Percentiles :=> 50 : 208.333333 95 : 40261.363636 99 : 45787.500000
rocksdb.compaction.times.micros statistics Percentiles :=> 50 : 0.000000 95 : 0.000000 99 : 0.000000
rocksdb.subcompaction.setup.times.micros statistics Percentiles :=> 50 : 0.000000 95 : 0.000000 99 : 0.000000
rocksdb.table.sync.micros statistics Percentiles :=> 50 : 27380.000000 95 : 27380.000000 99 : 27380.000000
rocksdb.compaction.outfile.sync.micros statistics Percentiles :=> 50 : 0.000000 95 : 0.000000 99 : 0.000000
rocksdb.wal.file.sync.micros statistics Percentiles :=> 50 : 0.000000 95 : 0.000000 99 : 0.000000
rocksdb.manifest.file.sync.micros statistics Percentiles :=> 50 : 5000.000000 95 : 5352.000000 99 : 5352.000000
rocksdb.table.open.io.micros statistics Percentiles :=> 50 : 28152.173913 95 : 58456.521739 99 : 68900.000000
rocksdb.db.multiget.micros statistics Percentiles :=> 50 : 0.000000 95 : 0.000000 99 : 0.000000
rocksdb.read.block.compaction.micros statistics Percentiles :=> 50 : 0.000000 95 : 0.000000 99 : 0.000000
rocksdb.read.block.get.micros statistics Percentiles :=> 50 : 0.000000 95 : 0.000000 99 : 0.000000
rocksdb.write.raw.block.micros statistics Percentiles :=> 50 : 0.500595 95 : 0.951131 99 : 0.991179
rocksdb.l0.slowdown.count statistics Percentiles :=> 50 : 0.000000 95 : 0.000000 99 : 0.000000
rocksdb.memtable.compaction.count statistics Percentiles :=> 50 : 0.000000 95 : 0.000000 99 : 0.000000
rocksdb.num.files.stall.count statistics Percentiles :=> 50 : 0.000000 95 : 0.000000 99 : 0.000000
rocksdb.hard.rate.limit.delay.count statistics Percentiles :=> 50 : 0.000000 95 : 0.000000 99 : 0.000000
rocksdb.soft.rate.limit.delay.count statistics Percentiles :=> 50 : 0.000000 95 : 0.000000 99 : 0.000000
rocksdb.numfiles.in.singlecompaction statistics Percentiles :=> 50 : 0.000000 95 : 0.000000 99 : 0.000000
rocksdb.db.seek.micros statistics Percentiles :=> 50 : 0.000000 95 : 0.000000 99 : 0.000000
rocksdb.db.write.stall statistics Percentiles :=> 50 : 0.000000 95 : 0.000000 99 : 0.000000
rocksdb.sst.read.micros statistics Percentiles :=> 50 : 0.000000 95 : 0.000000 99 : 0.000000
rocksdb.num.subcompactions.scheduled statistics Percentiles :=> 50 : 0.000000 95 : 0.000000 99 : 0.000000
```

#### Execution on BB5 uc4 node

```json
{
  "connectivity": {
    "synapses per astrocyte": {
      "minimum": 7,
      "maximum": 61968,
      "mean": 16201
    },
    "neurons per astrocyte": {
      "minimum": 6,
      "maximum": 2871,
      "mean": 1251
    }
  },
  "duration_seconds": 2145.447045217268,
  "h5_dataset": {
    "astrocytes": 14086,
    "neurons": 219422
  },
  "processed_astrocytes": 14086
}
```

```
------- DUMPING STATS -------
2019/01/25-11:15:03.304839 7fff7f64d700 [WARN] [db/db_impl.cc:626]
** DB Stats **
Uptime(secs): 2088.1 total, 782.0 interval
Cumulative writes: 24K writes, 704M keys, 24K commit groups, 1.0 writes per commit group, ingest: 16.19 GB, 7.94 MB/s
Cumulative WAL: 24K writes, 0 syncs, 24071.00 writes per sync, written: 16.19 GB, 7.94 MB/s
Cumulative stall: 00:14:13.151 H:M:S, 40.9 percent
Interval writes: 5886 writes, 156M keys, 5886 commit groups, 1.0 writes per commit group, ingest: 3686.18 MB, 4.71 MB/s
Interval WAL: 5886 writes, 0 syncs, 5886.00 writes per sync, written: 3.60 MB, 4.71 MB/s
Interval stall: 00:08:28.124 H:M:S, 65.0 percent

** Compaction Stats [default] **
Level    Files   Size     Score Read(GB)  Rn(GB) Rnp1(GB) Write(GB) Wnew(GB) Moved(GB) W-Amp Rd(MB/s) Wr(MB/s) Comp(sec) Comp(cnt) Avg(sec) KeyIn KeyDrop
----------------------------------------------------------------------------------------------------------------------------------------------------------
  L0      5/0   174.83 MB   1.2      0.0     0.0      0.0       2.3      2.3       0.0   1.0      0.0     29.6        78        65    1.200       0      0
  L1      5/0   648.20 MB   0.5      5.4     2.1      3.4       4.0      0.6       0.0   1.9     15.0     11.0       371        10   37.073   1081M  8794K
 Sum     10/0   823.03 MB   0.0      5.4     2.1      3.4       6.2      2.9       0.0   2.8     12.4     14.3       449        75    5.983   1081M  8794K
 Int      0/0    0.00 KB   0.0      2.3     0.5      1.7       2.3      0.6       0.0   4.5     11.9     12.3       194        18   10.765    541M  2525K
Uptime(secs): 2088.1 total, 782.0 interval
Flush(GB): cumulative 2.251, interval 0.515
AddFile(GB): cumulative 0.000, interval 0.000
AddFile(Total Files): cumulative 0, interval 0
AddFile(L0 Files): cumulative 0, interval 0
AddFile(Keys): cumulative 0, interval 0
Cumulative compaction: 6.25 GB write, 3.06 MB/s write, 5.44 GB read, 2.67 MB/s read, 448.7 seconds
Interval compaction: 2.32 GB write, 3.04 MB/s write, 2.25 GB read, 2.95 MB/s read, 193.8 seconds
Stalls(count): 0 level0_slowdown, 0 level0_slowdown_with_compaction, 0 level0_numfiles, 0 level0_numfiles_with_compaction, 0 stop for pending_compaction_bytes, 0 slowdown for pending_compaction_bytes, 0 memtable_compaction, 0 memtable_slowdown, interval 0 total count

** File Read Latency Histogram By Level [default] **
** Level 0 read latency histogram (micros):
Count: 892104 Average: 0.9345  StdDev: 0.90
Min: 0  Median: 0.5392  Max: 99
Percentiles: P50: 0.54 P75: 0.81 P99: 2.67 P99.9: 4.05 P99.99: 8.94
------------------------------------------------------
[       0,       1 ]   827265  92.732%  92.732% ###################
(       1,       2 ]    44454   4.983%  97.715% #
(       2,       3 ]    17066   1.913%  99.628%
(       3,       4 ]     2408   0.270%  99.898%
(       4,       6 ]      766   0.086%  99.984%
(       6,      10 ]       76   0.009%  99.992%
(      10,      15 ]        3   0.000%  99.993%
(      15,      22 ]        1   0.000%  99.993%
(      51,      76 ]       20   0.002%  99.995%
(      76,     110 ]       45   0.005% 100.000%

** Level 1 read latency histogram (micros):
Count: 3643349 Average: 0.5700  StdDev: 1.57
Min: 0  Median: 0.5089  Max: 1108
Percentiles: P50: 0.51 P75: 0.76 P99: 1.61 P99.9: 2.97 P99.99: 5.48
------------------------------------------------------
[       0,       1 ]  3579850  98.257%  98.257% ####################
(       1,       2 ]    44521   1.222%  99.479%
(       2,       3 ]    15783   0.433%  99.912%
(       3,       4 ]     2050   0.056%  99.969%
(       4,       6 ]     1057   0.029%  99.998%
(       6,      10 ]       51   0.001%  99.999%
(      10,      15 ]        1   0.000%  99.999%
(      22,      34 ]        1   0.000%  99.999%
(      76,     110 ]        1   0.000%  99.999%
(     110,     170 ]        1   0.000%  99.999%
(     170,     250 ]        2   0.000%  99.999%
(     250,     380 ]        7   0.000%  99.999%
(     380,     580 ]       19   0.001% 100.000%
(     580,     870 ]        4   0.000% 100.000%
(     870,    1300 ]        1   0.000% 100.000%


** Compaction Stats [connections] **
Level    Files   Size     Score Read(GB)  Rn(GB) Rnp1(GB) Write(GB) Wnew(GB) Moved(GB) W-Amp Rd(MB/s) Wr(MB/s) Comp(sec) Comp(cnt) Avg(sec) KeyIn KeyDrop
----------------------------------------------------------------------------------------------------------------------------------------------------------
  L0      3/0   99.12 MB   0.8      0.0     0.0      0.0       6.2      6.2       0.0   1.0      0.0     25.4       251       187    1.343       0      0
  L1     29/0    4.45 GB   3.6     34.9     6.1     28.7      33.2      4.5       0.0   5.4     21.8     20.7      1639        19   86.274   3386M      0
 Sum     32/0    4.55 GB   0.0     34.9     6.1     28.7      39.4     10.7       0.0   6.3     18.9     21.3      1890       206    9.177   3386M      0
 Int      0/0    0.00 KB   0.0     12.6     1.6     11.0      13.5      2.5       0.0   9.9     19.6     21.1       658        45   14.624   1259M      0
Uptime(secs): 2088.1 total, 782.0 interval
Flush(GB): cumulative 6.228, interval 1.370
AddFile(GB): cumulative 0.000, interval 0.000
AddFile(Total Files): cumulative 0, interval 0
AddFile(L0 Files): cumulative 0, interval 0
AddFile(Keys): cumulative 0, interval 0
Cumulative compaction: 39.41 GB write, 19.33 MB/s write, 34.86 GB read, 17.09 MB/s read, 1890.4 seconds
Interval compaction: 13.55 GB write, 17.74 MB/s write, 12.62 GB read, 16.53 MB/s read, 658.1 seconds
Stalls(count): 61 level0_slowdown, 60 level0_slowdown_with_compaction, 0 level0_numfiles, 0 level0_numfiles_with_compaction, 0 stop for pending_compaction_bytes, 0 slowdown for pending_compaction_bytes, 0 memtable_compaction, 0 memtable_slowdown, interval 13 total count

** File Read Latency Histogram By Level [connections] **
** Level 0 read latency histogram (micros):
Count: 2770664 Average: 0.9731  StdDev: 10.68
Min: 0  Median: 0.5364  Max: 1943
Percentiles: P50: 0.54 P75: 0.80 P99: 2.41 P99.9: 3.92 P99.99: 89.49
------------------------------------------------------
[       0,       1 ]  2582780  93.219%  93.219% ###################
(       1,       2 ]   146869   5.301%  98.520% #
(       2,       3 ]    32495   1.173%  99.692%
(       3,       4 ]     6278   0.227%  99.919%
(       4,       6 ]     1651   0.060%  99.979%
(       6,      10 ]      205   0.007%  99.986%
(      10,      15 ]        8   0.000%  99.986%
(      15,      22 ]        2   0.000%  99.986%
(      22,      34 ]        1   0.000%  99.986%
(      34,      51 ]        6   0.000%  99.987%
(      51,      76 ]       34   0.001%  99.988%
(      76,     110 ]      146   0.005%  99.993%
(     110,     170 ]        1   0.000%  99.993%
(     170,     250 ]        1   0.000%  99.993%
(     580,     870 ]       30   0.001%  99.994%
(     870,    1300 ]       85   0.003%  99.997%
(    1300,    1900 ]       70   0.003% 100.000%
(    1900,    2900 ]        2   0.000% 100.000%

** Level 1 read latency histogram (micros):
Count: 17114768 Average: 0.7606  StdDev: 31.85
Min: 0  Median: 0.5125  Max: 13400
Percentiles: P50: 0.51 P75: 0.77 P99: 1.72 P99.9: 2.99 P99.99: 5.68
------------------------------------------------------
[       0,       1 ] 16696787  97.558%  97.558% ####################
(       1,       2 ]   340463   1.989%  99.547%
(       2,       3 ]    60781   0.355%  99.902%
(       3,       4 ]    11206   0.065%  99.968%
(       4,       6 ]     4539   0.027%  99.994%
(       6,      10 ]      493   0.003%  99.997%
(      10,      15 ]       37   0.000%  99.997%
(      15,      22 ]        8   0.000%  99.997%
(      22,      34 ]        1   0.000%  99.997%
(      34,      51 ]        1   0.000%  99.997%
(      51,      76 ]        2   0.000%  99.997%
(      76,     110 ]        1   0.000%  99.997%
(     110,     170 ]        4   0.000%  99.997%
(     170,     250 ]        6   0.000%  99.997%
(     250,     380 ]      175   0.001%  99.998%
(     380,     580 ]       38   0.000%  99.999%
(     580,     870 ]        2   0.000%  99.999%
(     870,    1300 ]        1   0.000%  99.999%
(    1300,    1900 ]        2   0.000%  99.999%
(    1900,    2900 ]        3   0.000%  99.999%
(    2900,    4400 ]        4   0.000%  99.999%
(    4400,    6600 ]       17   0.000%  99.999%
(    6600,    9900 ]      149   0.001% 100.000%
(    9900,   14000 ]       48   0.000% 100.000%


** Compaction Stats [default] **
Level    Files   Size     Score Read(GB)  Rn(GB) Rnp1(GB) Write(GB) Wnew(GB) Moved(GB) W-Amp Rd(MB/s) Wr(MB/s) Comp(sec) Comp(cnt) Avg(sec) KeyIn KeyDrop
----------------------------------------------------------------------------------------------------------------------------------------------------------
  L0      5/0   174.83 MB   1.2      0.0     0.0      0.0       2.3      2.3       0.0   1.0      0.0     29.6        78        65    1.200       0      0
  L1      5/0   648.20 MB   0.5      5.4     2.1      3.4       4.0      0.6       0.0   1.9     15.0     11.0       371        10   37.073   1081M  8794K
 Sum     10/0   823.03 MB   0.0      5.4     2.1      3.4       6.2      2.9       0.0   2.8     12.4     14.3       449        75    5.983   1081M  8794K
 Int      0/0    0.00 KB   0.0      0.0     0.0      0.0       0.0      0.0       0.0   0.0      0.0      0.0         0         0    0.000       0      0
Uptime(secs): 2088.1 total, 0.0 interval
Flush(GB): cumulative 2.251, interval 0.000
AddFile(GB): cumulative 0.000, interval 0.000
AddFile(Total Files): cumulative 0, interval 0
AddFile(L0 Files): cumulative 0, interval 0
AddFile(Keys): cumulative 0, interval 0
Cumulative compaction: 6.25 GB write, 3.06 MB/s write, 5.44 GB read, 2.67 MB/s read, 448.7 seconds
Interval compaction: 0.00 GB write, 0.00 MB/s write, 0.00 GB read, 0.00 MB/s read, 0.0 seconds
Stalls(count): 0 level0_slowdown, 0 level0_slowdown_with_compaction, 0 level0_numfiles, 0 level0_numfiles_with_compaction, 0 stop for pending_compaction_bytes, 0 slowdown for pending_compaction_bytes, 0 memtable_compaction, 0 memtable_slowdown, interval 0 total count

** File Read Latency Histogram By Level [default] **
** Level 0 read latency histogram (micros):
Count: 892104 Average: 0.9345  StdDev: 0.90
Min: 0  Median: 0.5392  Max: 99
Percentiles: P50: 0.54 P75: 0.81 P99: 2.67 P99.9: 4.05 P99.99: 8.94
------------------------------------------------------
[       0,       1 ]   827265  92.732%  92.732% ###################
(       1,       2 ]    44454   4.983%  97.715% #
(       2,       3 ]    17066   1.913%  99.628%
(       3,       4 ]     2408   0.270%  99.898%
(       4,       6 ]      766   0.086%  99.984%
(       6,      10 ]       76   0.009%  99.992%
(      10,      15 ]        3   0.000%  99.993%
(      15,      22 ]        1   0.000%  99.993%
(      51,      76 ]       20   0.002%  99.995%
(      76,     110 ]       45   0.005% 100.000%

** Level 1 read latency histogram (micros):
Count: 3643349 Average: 0.5700  StdDev: 1.57
Min: 0  Median: 0.5089  Max: 1108
Percentiles: P50: 0.51 P75: 0.76 P99: 1.61 P99.9: 2.97 P99.99: 5.48
------------------------------------------------------
[       0,       1 ]  3579850  98.257%  98.257% ####################
(       1,       2 ]    44521   1.222%  99.479%
(       2,       3 ]    15783   0.433%  99.912%
(       3,       4 ]     2050   0.056%  99.969%
(       4,       6 ]     1057   0.029%  99.998%
(       6,      10 ]       51   0.001%  99.999%
(      10,      15 ]        1   0.000%  99.999%
(      22,      34 ]        1   0.000%  99.999%
(      76,     110 ]        1   0.000%  99.999%
(     110,     170 ]        1   0.000%  99.999%
(     170,     250 ]        2   0.000%  99.999%
(     250,     380 ]        7   0.000%  99.999%
(     380,     580 ]       19   0.001% 100.000%
(     580,     870 ]        4   0.000% 100.000%
(     870,    1300 ]        1   0.000% 100.000%


** Compaction Stats [connections] **
Level    Files   Size     Score Read(GB)  Rn(GB) Rnp1(GB) Write(GB) Wnew(GB) Moved(GB) W-Amp Rd(MB/s) Wr(MB/s) Comp(sec) Comp(cnt) Avg(sec) KeyIn KeyDrop
----------------------------------------------------------------------------------------------------------------------------------------------------------
  L0      3/0   99.12 MB   0.8      0.0     0.0      0.0       6.2      6.2       0.0   1.0      0.0     25.4       251       187    1.343       0      0
  L1     29/0    4.45 GB   3.6     34.9     6.1     28.7      33.2      4.5       0.0   5.4     21.8     20.7      1639        19   86.274   3386M      0
 Sum     32/0    4.55 GB   0.0     34.9     6.1     28.7      39.4     10.7       0.0   6.3     18.9     21.3      1890       206    9.177   3386M      0
 Int      0/0    0.00 KB   0.0      0.0     0.0      0.0       0.0      0.0       0.0   0.0      0.0      0.0         0         0    0.000       0      0
Uptime(secs): 2088.1 total, 0.0 interval
Flush(GB): cumulative 6.228, interval 0.000
AddFile(GB): cumulative 0.000, interval 0.000
AddFile(Total Files): cumulative 0, interval 0
AddFile(L0 Files): cumulative 0, interval 0
AddFile(Keys): cumulative 0, interval 0
Cumulative compaction: 39.41 GB write, 19.33 MB/s write, 34.86 GB read, 17.09 MB/s read, 1890.4 seconds
Interval compaction: 0.00 GB write, 0.00 MB/s write, 0.00 GB read, 0.00 MB/s read, 0.0 seconds
Stalls(count): 61 level0_slowdown, 60 level0_slowdown_with_compaction, 0 level0_numfiles, 0 level0_numfiles_with_compaction, 0 stop for pending_compaction_bytes, 0 slowdown for pending_compaction_bytes, 0 memtable_compaction, 0 memtable_slowdown, interval 0 total count

** File Read Latency Histogram By Level [connections] **
** Level 0 read latency histogram (micros):
Count: 2770664 Average: 0.9731  StdDev: 10.68
Min: 0  Median: 0.5364  Max: 1943
Percentiles: P50: 0.54 P75: 0.80 P99: 2.41 P99.9: 3.92 P99.99: 89.49
------------------------------------------------------
[       0,       1 ]  2582780  93.219%  93.219% ###################
(       1,       2 ]   146869   5.301%  98.520% #
(       2,       3 ]    32495   1.173%  99.692%
(       3,       4 ]     6278   0.227%  99.919%
(       4,       6 ]     1651   0.060%  99.979%
(       6,      10 ]      205   0.007%  99.986%
(      10,      15 ]        8   0.000%  99.986%
(      15,      22 ]        2   0.000%  99.986%
(      22,      34 ]        1   0.000%  99.986%
(      34,      51 ]        6   0.000%  99.987%
(      51,      76 ]       34   0.001%  99.988%
(      76,     110 ]      146   0.005%  99.993%
(     110,     170 ]        1   0.000%  99.993%
(     170,     250 ]        1   0.000%  99.993%
(     580,     870 ]       30   0.001%  99.994%
(     870,    1300 ]       85   0.003%  99.997%
(    1300,    1900 ]       70   0.003% 100.000%
(    1900,    2900 ]        2   0.000% 100.000%

** Level 1 read latency histogram (micros):
Count: 17114768 Average: 0.7606  StdDev: 31.85
Min: 0  Median: 0.5125  Max: 13400
Percentiles: P50: 0.51 P75: 0.77 P99: 1.72 P99.9: 2.99 P99.99: 5.68
------------------------------------------------------
[       0,       1 ] 16696787  97.558%  97.558% ####################
(       1,       2 ]   340463   1.989%  99.547%
(       2,       3 ]    60781   0.355%  99.902%
(       3,       4 ]    11206   0.065%  99.968%
(       4,       6 ]     4539   0.027%  99.994%
(       6,      10 ]      493   0.003%  99.997%
(      10,      15 ]       37   0.000%  99.997%
(      15,      22 ]        8   0.000%  99.997%
(      22,      34 ]        1   0.000%  99.997%
(      34,      51 ]        1   0.000%  99.997%
(      51,      76 ]        2   0.000%  99.997%
(      76,     110 ]        1   0.000%  99.997%
(     110,     170 ]        4   0.000%  99.997%
(     170,     250 ]        6   0.000%  99.997%
(     250,     380 ]      175   0.001%  99.998%
(     380,     580 ]       38   0.000%  99.999%
(     580,     870 ]        2   0.000%  99.999%
(     870,    1300 ]        1   0.000%  99.999%
(    1300,    1900 ]        2   0.000%  99.999%
(    1900,    2900 ]        3   0.000%  99.999%
(    2900,    4400 ]        4   0.000%  99.999%
(    4400,    6600 ]       17   0.000%  99.999%
(    6600,    9900 ]      149   0.001% 100.000%
(    9900,   14000 ]       48   0.000% 100.000%

2019/01/25-11:15:03.310491 7fff7f64d700 [WARN] [db/db_impl.cc:576] STATISTICS:
 rocksdb.block.cache.miss COUNT : 24418421
rocksdb.block.cache.hit COUNT : 0
rocksdb.block.cache.add COUNT : 0
rocksdb.block.cache.add.failures COUNT : 0
rocksdb.block.cache.index.miss COUNT : 0
rocksdb.block.cache.index.hit COUNT : 0
rocksdb.block.cache.index.add COUNT : 0
rocksdb.block.cache.index.bytes.insert COUNT : 0
rocksdb.block.cache.index.bytes.evict COUNT : 0
rocksdb.block.cache.filter.miss COUNT : 0
rocksdb.block.cache.filter.hit COUNT : 0
rocksdb.block.cache.filter.add COUNT : 0
rocksdb.block.cache.filter.bytes.insert COUNT : 0
rocksdb.block.cache.filter.bytes.evict COUNT : 0
rocksdb.block.cache.data.miss COUNT : 24418421
rocksdb.block.cache.data.hit COUNT : 0
rocksdb.block.cache.data.add COUNT : 0
rocksdb.block.cache.data.bytes.insert COUNT : 0
rocksdb.block.cache.bytes.read COUNT : 0
rocksdb.block.cache.bytes.write COUNT : 0
rocksdb.bloom.filter.useful COUNT : 0
rocksdb.bloom.filter.full.positive COUNT : 0
rocksdb.bloom.filter.full.true.positive COUNT : 0
rocksdb.persistent.cache.hit COUNT : 0
rocksdb.persistent.cache.miss COUNT : 0
rocksdb.sim.block.cache.hit COUNT : 0
rocksdb.sim.block.cache.miss COUNT : 0
rocksdb.memtable.hit COUNT : 0
rocksdb.memtable.miss COUNT : 0
rocksdb.l0.hit COUNT : 0
rocksdb.l1.hit COUNT : 0
rocksdb.l2andup.hit COUNT : 0
rocksdb.compaction.key.drop.new COUNT : 8794650
rocksdb.compaction.key.drop.obsolete COUNT : 0
rocksdb.compaction.key.drop.range_del COUNT : 0
rocksdb.compaction.key.drop.user COUNT : 0
rocksdb.compaction.range_del.drop.obsolete COUNT : 0
rocksdb.compaction.optimized.del.drop.obsolete COUNT : 0
rocksdb.compaction.cancelled COUNT : 0
rocksdb.number.keys.written COUNT : 704591684
rocksdb.number.keys.read COUNT : 0
rocksdb.number.keys.updated COUNT : 0
rocksdb.bytes.written COUNT : 17380008442
rocksdb.bytes.read COUNT : 0
rocksdb.number.db.seek COUNT : 0
rocksdb.number.db.next COUNT : 0
rocksdb.number.db.prev COUNT : 0
rocksdb.number.db.seek.found COUNT : 0
rocksdb.number.db.next.found COUNT : 0
rocksdb.number.db.prev.found COUNT : 0
rocksdb.db.iter.bytes.read COUNT : 0
rocksdb.no.file.closes COUNT : 0
rocksdb.no.file.opens COUNT : 513
rocksdb.no.file.errors COUNT : 0
rocksdb.l0.slowdown.micros COUNT : 0
rocksdb.memtable.compaction.micros COUNT : 0
rocksdb.l0.num.files.stall.micros COUNT : 0
rocksdb.stall.micros COUNT : 853151011
rocksdb.db.mutex.wait.micros COUNT : 0
rocksdb.rate.limit.delay.millis COUNT : 0
rocksdb.num.iterators COUNT : 0
rocksdb.number.multiget.get COUNT : 0
rocksdb.number.multiget.keys.read COUNT : 0
rocksdb.number.multiget.bytes.read COUNT : 0
rocksdb.number.deletes.filtered COUNT : 0
rocksdb.number.merge.failures COUNT : 0
rocksdb.bloom.filter.prefix.checked COUNT : 0
rocksdb.bloom.filter.prefix.useful COUNT : 0
rocksdb.number.reseeks.iteration COUNT : 0
rocksdb.getupdatessince.calls COUNT : 0
rocksdb.block.cachecompressed.miss COUNT : 0
rocksdb.block.cachecompressed.hit COUNT : 0
rocksdb.block.cachecompressed.add COUNT : 0
rocksdb.block.cachecompressed.add.failures COUNT : 0
rocksdb.wal.synced COUNT : 0
rocksdb.wal.bytes COUNT : 17380008442
rocksdb.write.self COUNT : 24071
rocksdb.write.other COUNT : 0
rocksdb.write.timeout COUNT : 0
rocksdb.write.wal COUNT : 48142
rocksdb.compact.read.bytes COUNT : 43246934126
rocksdb.compact.write.bytes COUNT : 39915258986
rocksdb.flush.write.bytes COUNT : 9104982648
rocksdb.number.direct.load.table.properties COUNT : 0
rocksdb.number.superversion_acquires COUNT : 0
rocksdb.number.superversion_releases COUNT : 0
rocksdb.number.superversion_cleanups COUNT : 0
rocksdb.number.block.compressed COUNT : 28046182
rocksdb.number.block.decompressed COUNT : 24418733
rocksdb.number.block.not_compressed COUNT : 0
rocksdb.merge.operation.time.nanos COUNT : 0
rocksdb.filter.operation.time.nanos COUNT : 0
rocksdb.row.cache.hit COUNT : 0
rocksdb.row.cache.miss COUNT : 0
rocksdb.read.amp.estimate.useful.bytes COUNT : 0
rocksdb.read.amp.total.read.bytes COUNT : 0
rocksdb.number.rate_limiter.drains COUNT : 0
rocksdb.number.iter.skip COUNT : 0
rocksdb.blobdb.num.put COUNT : 0
rocksdb.blobdb.num.write COUNT : 0
rocksdb.blobdb.num.get COUNT : 0
rocksdb.blobdb.num.multiget COUNT : 0
rocksdb.blobdb.num.seek COUNT : 0
rocksdb.blobdb.num.next COUNT : 0
rocksdb.blobdb.num.prev COUNT : 0
rocksdb.blobdb.num.keys.written COUNT : 0
rocksdb.blobdb.num.keys.read COUNT : 0
rocksdb.blobdb.bytes.written COUNT : 0
rocksdb.blobdb.bytes.read COUNT : 0
rocksdb.blobdb.write.inlined COUNT : 0
rocksdb.blobdb.write.inlined.ttl COUNT : 0
rocksdb.blobdb.write.blob COUNT : 0
rocksdb.blobdb.write.blob.ttl COUNT : 0
rocksdb.blobdb.blob.file.bytes.written COUNT : 0
rocksdb.blobdb.blob.file.bytes.read COUNT : 0
rocksdb.blobdb.blob.file.synced COUNT : 0
rocksdb.blobdb.blob.index.expired.count COUNT : 0
rocksdb.blobdb.blob.index.expired.size COUNT : 0
rocksdb.blobdb.blob.index.evicted.count COUNT : 0
rocksdb.blobdb.blob.index.evicted.size COUNT : 0
rocksdb.blobdb.gc.num.files COUNT : 0
rocksdb.blobdb.gc.num.new.files COUNT : 0
rocksdb.blobdb.gc.failures COUNT : 0
rocksdb.blobdb.gc.num.keys.overwritten COUNT : 0
rocksdb.blobdb.gc.num.keys.expired COUNT : 0
rocksdb.blobdb.gc.num.keys.relocated COUNT : 0
rocksdb.blobdb.gc.bytes.overwritten COUNT : 0
rocksdb.blobdb.gc.bytes.expired COUNT : 0
rocksdb.blobdb.gc.bytes.relocated COUNT : 0
rocksdb.blobdb.fifo.num.files.evicted COUNT : 0
rocksdb.blobdb.fifo.num.keys.evicted COUNT : 0
rocksdb.blobdb.fifo.bytes.evicted COUNT : 0
rocksdb.txn.overhead.mutex.prepare COUNT : 0
rocksdb.txn.overhead.mutex.old.commit.map COUNT : 0
rocksdb.txn.overhead.duplicate.key COUNT : 0
rocksdb.txn.overhead.mutex.snapshot COUNT : 0
rocksdb.number.multiget.keys.found COUNT : 0
rocksdb.db.get.micros P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.db.write.micros P50 : 36576.600210 P95 : 235594.280608 P99 : 1148120.689655 P100 : 4355667.000000 COUNT : 24070 SUM : 2017800871
rocksdb.compaction.times.micros P50 : 47400000.000000 P95 : 217375000.000000 P99 : 221846908.000000 P100 : 221846908.000000 COUNT : 29 SUM : 2009938596
rocksdb.subcompaction.setup.times.micros P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.table.sync.micros P50 : 25586.956522 P95 : 32366.304348 P99 : 32968.913043 P100 : 112296.000000 COUNT : 252 SUM : 6035411
rocksdb.compaction.outfile.sync.micros P50 : 133571.428571 P95 : 167128.571429 P99 : 180400.000000 P100 : 193023.000000 COUNT : 261 SUM : 30620423
rocksdb.wal.file.sync.micros P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.manifest.file.sync.micros P50 : 72.702454 P95 : 134.176471 P99 : 193.400000 P100 : 222.000000 COUNT : 283 SUM : 22567
rocksdb.table.open.io.micros P50 : 3203.191489 P95 : 18957.000000 P99 : 18957.000000 P100 : 18957.000000 COUNT : 513 SUM : 3700238
rocksdb.db.multiget.micros P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.read.block.compaction.micros P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.read.block.get.micros P50 : 4.362329 P95 : 5.905645 P99 : 8.247584 P100 : 1037.000000 COUNT : 24418421 SUM : 115590215
rocksdb.write.raw.block.micros P50 : 0.500919 P95 : 0.951746 P99 : 0.991820 P100 : 28744.000000 COUNT : 28047852 SUM : 36584392
rocksdb.l0.slowdown.count P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.memtable.compaction.count P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.num.files.stall.count P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.hard.rate.limit.delay.count P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.soft.rate.limit.delay.count P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.numfiles.in.singlecompaction P50 : 5.833333 P95 : 19.462500 P99 : 20.000000 P100 : 20.000000 COUNT : 29 SUM : 244
rocksdb.db.seek.micros P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.db.write.stall P50 : 0.841910 P95 : 1391094.890511 P99 : 2552571.428571 P100 : 4349147.000000 COUNT : 3632 SUM : 853151639
rocksdb.sst.read.micros P50 : 0.515498 P95 : 0.979447 P99 : 1.850231 P100 : 13400.000000 COUNT : 24420885 SUM : 18623958
rocksdb.num.subcompactions.scheduled P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.bytes.per.read P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.bytes.per.write P50 : 235874.125874 P95 : 2484692.962797 P99 : 3086102.564103 P100 : 4585660.000000 COUNT : 24071 SUM : 17380008442
rocksdb.bytes.per.multiget P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.bytes.compressed P50 : 3650.008290 P95 : 4325.025523 P99 : 4385.027055 P100 : 4919382.000000 COUNT : 28046182 SUM : 115122733075
rocksdb.bytes.decompressed P50 : 3650.009767 P95 : 4325.029339 P99 : 4385.031079 P100 : 4919382.000000 COUNT : 24418733 SUM : 100357680341
rocksdb.compression.times.nanos P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.decompression.times.nanos P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.read.num.merge_operands P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.blobdb.key.size P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.blobdb.value.size P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.blobdb.write.micros P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.blobdb.get.micros P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.blobdb.multiget.micros P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.blobdb.seek.micros P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.blobdb.next.micros P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.blobdb.prev.micros P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.blobdb.blob.file.write.micros P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.blobdb.blob.file.read.micros P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.blobdb.blob.file.sync.micros P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.blobdb.gc.micros P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.blobdb.compression.micros P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.blobdb.decompression.micros P50 : 0.000000 P95 : 0.000000 P99 : 0.000000 P100 : 0.000000 COUNT : 0 SUM : 0
rocksdb.db.flush.micros P50 : 1472320.000000 P95 : 1472320.000000 P99 : 1472320.000000 P100 : 1472320.000000 COUNT : 252 SUM : 329152374
```
