# Notebook

Processor: Intel(R) Xeon(R) CPU E3-1505M v5 @ 2.80GHz
OMP_NUM_THREADS: 3

## Results
Serial: 10.49s

### Simple

0_naive: 86.3847s
1_atomic: 41.9244s
2_no_crit: 6.48693s
3_simd: 6.65743s
4_scheduler: 6.93647s

### Force Matrix

0_force_matrix: 16.29s
1_simd: 18.54s
2_scheduler: 15.86s

