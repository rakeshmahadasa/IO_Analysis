# IO_Analysis
We are studying the latency and throughput of the IO at system level with varying block size.
- We found that the throughput doesnt vary much with increase in block size
- We are flushing the blocks for every write.
- Parallel IOPS saturate if we are the cumulative write size of all threads is beyond the bandwidth of the disk, else the performance increase is linear wrt to the number of threads

