import numpy as np


def eval_func_on_uniform_grid(n_points: int, func: callable):
    h = 1.0 / (n_points - 1)
    sol_matrix = np.zeros((n_points, n_points))
    for i in range(n_points):
        for j in range(n_points):
            sol_matrix[i, j] = func(i * h, j * h)
    return sol_matrix


if __name__ == "__main__":
    forcing_func = lambda x, y: 8 * np.pi**2 * np.sin(np.pi * x) * np.cos(np.pi * y)
    k = 2
    print("forcing function")
    print(eval_func_on_uniform_grid(2**k, forcing_func))
    sol_func = lambda x, y: np.sin(2 * np.pi * x) * np.cos(2 * np.pi * y)
    print("solution matrix")
    print(eval_func_on_uniform_grid(2**k, sol_func))
