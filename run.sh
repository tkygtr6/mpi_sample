#!/bin/sh
#PBS -q u-short
#PBS -l select=8:ncpus=32:mpiprocs=32:ompthreads=32
#PBS -W group_list=gc64
#PBS -l walltime=00:30:00
#PBS -N test
#PBS -j oe
cd $PBS_O_WORKDIR
mpicc mpi_time.c
mpiexec -n 16 ./a.out
