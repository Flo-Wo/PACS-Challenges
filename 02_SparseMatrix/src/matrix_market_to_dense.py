from scipy.io import mmread
from scipy.sparse import csr_matrix, csc_matrix
import numpy as np


if __name__ == "__main__":
    matrix = mmread("small_example.mtx")
    to_multiply = np.array([42, -17, 100, 0, 73, -5, 21, 8, -33, 55])

    print("CSR-MATRIX")
    csr = csr_matrix(matrix)
    print(csr @ to_multiply)
    # print(csr)
    # print(csr.data)
    # print(csr.indices)
    # print(csr.indptr)

    # print("CSC-MATRIX")
    # csc = csc_matrix(matrix)
    # print(csc)
    # print(csc.data)
    # print(csc.indices)
    # print(csc.indptr)
