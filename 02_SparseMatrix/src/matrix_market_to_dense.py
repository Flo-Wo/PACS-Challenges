from scipy.io import mmread
from scipy.sparse import csr_matrix, csc_matrix


if __name__ == "__main__":
    matrix = mmread("small_example.mtx")

    # print("CSR-MATRIX")
    # csr = csr_matrix(matrix)
    # print(csr)
    # print(csr.data)
    # print(csr.indices)
    # print(csr.indptr)

    print("CSC-MATRIX")
    csc = csc_matrix(matrix)
    print(csc)
    print(csc.data)
    print(csc.indices)
    print(csc.indptr)
