#include <Accelerate/Accelerate.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <chrono>

// 矩阵求解: aA + bB = C
void matrix_solver(float alpha, float beta,
                   const std::vector<float>& A,
                   const std::vector<float>& B,
                   std::vector<float>& C, int N)
{
    // 使用 Accelerate 的 SGEMM 单精度矩阵乘法函数进行矩阵运算 - 基于 BLAS 的 CPU 优化接口 - GEMM：通用矩阵乘法
    // 计算公式：C=α×A×B+β×C
    // 如果数据是列优先存储，为了加快读取速度（避免跳跃访问），则需要转置
    // 如果转置，BLAS 需要额外调整数据存取方式，可能降低性能
    cblas_sgemm(
        CblasRowMajor, // 矩阵存储格式为行优先
        CblasNoTrans, // 矩阵 A 不转置
        CblasNoTrans, // 矩阵 B 不转置
        N, N, N, // 矩阵维度 N x N
        alpha, // 标量 alpha
        A.data(), N, // 矩阵 A 和它的 leading dimension
        B.data(), N, // 矩阵 B 和它的 leading dimension
        beta, // 标量 beta
        C.data(), N // 矩阵 C 和它的 leading dimension
    );
}

int main()
{
    const int N = 1000; // 矩阵维度
    const int N2 = N * N;

    // 初始化矩阵和标量
    std::vector<float> A(N2), B(N2), C(N2);
    float alpha = 2.0f;
    float beta = 3.0f;

    // 给矩阵赋随机值
    std::srand(42); // 固定随机种子
    /**
    *std::rand() 生成一个介于 0 和 RAND_MAX 之间的整数
    *RAND_MAX 是一个常量，表示 std::rand() 能生成的最大值（通常是一个大整数，具体值由实现定义，常见值为 32767 或 2147483647）
    *除以 RAND_MAX 后，结果被缩放到范围  [0,1]
    */
    for (int i = 0; i < N2; ++i)
    {
        A[i] = static_cast<float>(std::rand()) / RAND_MAX;
        B[i] = static_cast<float>(std::rand()) / RAND_MAX;
        C[i] = static_cast<float>(std::rand()) / RAND_MAX;
    }

    // CPU 计时
    auto start = std::chrono::high_resolution_clock::now();
    matrix_solver(alpha, beta, A, B, C, N);
    auto end = std::chrono::high_resolution_clock::now();

    // 计算运行时间
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "Matrix computation completed.\n";
    std::cout << "Time taken: " << duration.count() << " ms.\n";

    // 输出部分结果
    std::cout << "Sample Result (C[0]): " << C[0] << "\n";

    return 0;
}
/*
Matrix computation completed.
Time taken: 4.57399 ms.
Sample Result (C[0]): 467.56
*/
