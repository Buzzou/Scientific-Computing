#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include "Foundation.hpp"
#include "Metal.hpp"
#include "QuartzCore.hpp"
#include <iostream>
#include <vector>
#include <random>

const int N = 1000; // 矩阵大小 N x N
const int MATRIX_SIZE = N * N;

// 初始化随机矩阵
void initializeMatrix(std::vector<float>& matrix, float scale = 1.0f)
{
    std::mt19937 gen(42); // 固定随机种子
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    for (auto& val: matrix)
    {
        val = dis(gen) * scale;
    }
}

int main()
{
    // Step 1: 准备数据
    std::vector<float> A(MATRIX_SIZE), B(MATRIX_SIZE), C(MATRIX_SIZE);
    float a = 2.0f; // 常系数 a
    float b = 3.0f; // 常系数 b
    initializeMatrix(A);
    initializeMatrix(B);

    // Step 2: 获取 Metal 设备
    MTL::Device* device = MTL::CreateSystemDefaultDevice();
    if (!device)
    {
        std::cerr << "Metal GPU 不可用！" << std::endl;
        return -1;
    }
    std::cout << "使用 Metal GPU: " << device->name()->utf8String() << std::endl;

    // Step 3: 创建 Metal Buffer
    MTL::Buffer* bufferA = device->newBuffer(A.data(), sizeof(float) * MATRIX_SIZE, MTL::ResourceStorageModeShared);
    MTL::Buffer* bufferB = device->newBuffer(B.data(), sizeof(float) * MATRIX_SIZE, MTL::ResourceStorageModeShared);
    MTL::Buffer* bufferC = device->newBuffer(sizeof(float) * MATRIX_SIZE, MTL::ResourceStorageModeShared);

    MTL::Buffer* bufferA_const = device->newBuffer(&a, sizeof(float), MTL::ResourceStorageModeShared);
    MTL::Buffer* bufferB_const = device->newBuffer(&b, sizeof(float), MTL::ResourceStorageModeShared);

    // Step 4: 加载 Metal Shader
    NS::Error* error = nullptr;
    MTL::Library* library = device->newDefaultLibrary();
    if (!library)
    {
        std::cerr << "无法加载 Metal Shader！" << std::endl;
        return -1;
    }

    MTL::Function* function = library->newFunction(NS::String::string("matrix_add", NS::UTF8StringEncoding));
    if (!function)
    {
        std::cerr << "无法找到函数 matrix_add！" << std::endl;
        return -1;
    }

    MTL::ComputePipelineState* pipelineState = device->newComputePipelineState(function, &error);
    if (error)
    {
        std::cerr << "创建 Pipeline 失败：" << error->localizedDescription()->utf8String() << std::endl;
        return -1;
    }

    // Step 5: 创建 Command Queue 和 Command Buffer
    MTL::CommandQueue* commandQueue = device->newCommandQueue();
    MTL::CommandBuffer* commandBuffer = commandQueue->commandBuffer();
    MTL::ComputeCommandEncoder* encoder = commandBuffer->computeCommandEncoder();

    encoder->setComputePipelineState(pipelineState);
    encoder->setBuffer(bufferA, 0, 0);
    encoder->setBuffer(bufferB, 0, 1);
    encoder->setBuffer(bufferC, 0, 2);
    encoder->setBuffer(bufferA_const, 0, 3);
    encoder->setBuffer(bufferB_const, 0, 4);

    // Step 6: 设置线程执行配置
    MTL::Size gridSize(MATRIX_SIZE, 1, 1);
    MTL::Size threadGroupSize(256, 1, 1);
    encoder->dispatchThreads(gridSize, threadGroupSize);

    encoder->endEncoding();

    // Step 7: 提交命令并等待完成
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();

    // Step 8: 提取结果
    float* result = static_cast<float*>(bufferC->contents());
    for (int i = 0; i < 10; ++i)
    {
        std::cout << "C[" << i << "] = " << result[i] << std::endl;
    }

    // Step 9: 清理资源
    bufferA->release();
    bufferB->release();
    bufferC->release();
    bufferA_const->release();
    bufferB_const->release();
    function->release();
    pipelineState->release();
    library->release();
    commandQueue->release();
    commandBuffer->release();
    encoder->release();
    device->release();

    return 0;
}
