#include <metal_stdlib>
using namespace metal;

// 定义 Metal 的 Kernel 函数
kernel void matrix_add(const device float* A [[buffer(0)]],
                       const device float* B [[buffer(1)]],
                       device float* C [[buffer(2)]],
                       constant float& a [[buffer(3)]],
                       constant float& b [[buffer(4)]],
                       uint id [[thread_position_in_grid]]) {
    // 每个线程处理一个矩阵元素
    C[id] = a * A[id] + b * B[id];
}
