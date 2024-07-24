# symNMF Clustering Project

## Introduction

This project implements a clustering algorithm based on symmetric Non-negative Matrix Factorization (symNMF). The algorithm is applied to various datasets and compared with the K-means clustering algorithm. The symNMF method is derived from the mathematical foundations and algorithms described in the project guidelines.

## Algorithm Overview

### SymNMF Algorithm
Given a set of \( n \) points \( X = x_1, x_2, ..., x_N \in \mathbb{R}^d \), the symNMF algorithm involves the following steps:

1. **Form the Similarity Matrix \( A \)**:
   \[
   a_{ij} = \exp \left( - \frac{\|x_i - x_j\|^2}{2} \right) \quad \text{if} \quad i \neq j, \quad a_{ii} = 0
   \]
   where \( \| \cdot \| \) denotes the Euclidean distance.

2. **Compute the Diagonal Degree Matrix \( D \)**:
   \[
   d_i = \sum_{j=1}^{n} a_{ij}
   \]

3. **Compute the Normalized Similarity Matrix \( W \)**:
   \[
   W = D^{-1/2} A D^{-1/2}
   \]

4. **Find \( H \) that solves**:
   \[
   \min_{H \geq 0} \|W - H H^T\|_F^2
   \]
   where \( \| \cdot \|_F \) denotes the Frobenius norm.

### Optimizing \( H \)

1. **Initialize \( H \)**:
   Randomly initialize \( H \) with values from the interval \([0, 2 \cdot \sqrt{m/k}]\), where \( m \) is the average of all entries of \( W \).

2. **Update \( H \)**:
   \[
   H_{ij}^{(t+1)} \leftarrow H_{ij}^{(t)} \left(1 - \beta + \beta \frac{(W H^{(t)})_{ij}}{(H^{(t)} (H^{(t)})^T H^{(t)})_{ij}} \right)
   \]
   where \( \beta = 0.5 \).

3. **Convergence**:
   Update \( H \) until the maximum number of iterations is reached or \( \|H^{(t+1)} - H^{(t)}\|_F < \epsilon \).

### Clustering Solution
\( H \) can be viewed as an association matrix that assigns each element to the cluster with the highest association score.

## Assignment Description

### Files to Implement

1. **symnmf.py**: Python interface for the symNMF algorithm.
2. **symnmf.h**: C header file defining function prototypes.
3. **symnmf.c**: C implementation of the symNMF algorithm.
4. **symnmfmodule.c**: Python C API wrapper.
5. **analysis.py**: Script to analyze the algorithm and compare it to K-means.
6. **setup.py**: Setup file for building the C extension.
7. **Makefile**: Script to build the C interface.
8. Additional *.c/h files as needed.

### Python Program (symnmf.py)
- **Arguments**:
  - `k` (int, < N): Number of clusters.
  - `goal`: One of `symnmf`, `sym`, `ddg`, `norm`.
  - `file_name` (.txt): Path to the input file.

- **Goals**:
  - `symnmf`: Perform full symNMF and output \( H \).
  - `sym`: Output the similarity matrix \( A \).
  - `ddg`: Output the Diagonal Degree Matrix \( D \).
  - `norm`: Output the normalized similarity matrix \( W \).

### C Program (symnmf.c)
- **Arguments**:
  - `goal`: One of `sym`, `ddg`, `norm`.
  - `file_name` (.txt): Path to the input file.

### Python C API (symnmfmodule.c)
Defines the functions: `symnmf`, `sym`, `ddg`, `norm` for Python.

### C Header file (symnmf.h)
Defines all function prototypes used in `symnmfmodule.c` and implemented in `symnmf.c`.

### Analysis (analysis.py)
Compares symNMF to K-means using the silhouette score from `sklearn.metrics`.

### Setup (setup.py)
Builds the C extension for the symNMF algorithm.

### Makefile
Script to compile the C interface with appropriate flags.

## Build and Run

### Build
```bash
python3 setup.py build_ext --inplace