---
title: Performance of OpenMP Task Parallelism
date: 2015-12-07
---
Researchers from the University of Houston have developed an approach for translating parallel [OpenMP](http://www.openmp.org/) programs to the [Multicore Task Management API (MTAPI)](http://www.multicore-association.org/workgroup/mtapi.php). In their paper [Deploying OpenMP Task Parallelism on Multicore Embedded Systems with MCA Task APIs](http://dx.doi.org/10.1109/HPCC-CSS-ICESS.2015.88), which was presented at 17<sup>th</sup> IEEE International Conference on High Performance Computing and Communications (HPCC), the authors describe the design of an appropriate runtime library (RTL) and compare the performance with state-of-the-art solutions. Interestingly, the RTL based on the MTAPI implementation provided by EMB² has the least task overhead for more than eight cores and scales much better than the widely used GCC-based implementation for the SparseLU benchmark.
