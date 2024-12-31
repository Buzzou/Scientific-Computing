#include <mpi.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>

// 辅助函数：保存温度矩阵到文件
void saveDataFile(const std::vector<std::vector<double> >& T, int timeRound, int dimension, int rank)
{
    std::ofstream file("output_" + std::to_string(rank) + "_time" + std::to_string(timeRound) + ".txt");
    for (int i = 0; i < dimension; ++i)
    {
        for (int j = 0; j < dimension; ++j)
        {
            file << std::fixed << std::setprecision(2) << T[i][j] << " ";
        }
        file << "\n";
    }
    file.close();
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // 当前进程的 rank - 用来区分每个计算节点
    MPI_Comm_size(MPI_COMM_WORLD, &size); // 进程总数 - 用于划分矩阵块

    // 参数初始化
    constexpr int dimension = 100; // 矩阵的维度
    constexpr int timeRounds = 100; // 时间步数
    constexpr double deltaT = 0.01; // 时间步长
    constexpr double deltaX = 0.1; // 空间步长 X - 影响扩散速度
    constexpr double deltaY = 0.1; // 空间步长 Y - 影响扩散速度
    constexpr double conductCoeff = 0.1; // 导热系数
    constexpr double gasDensity = 1.0; // 气体密度
    constexpr double br = 1.0; // 比热容

    // 每个进程分配的计算域大小（均匀分块，忽略无法整除情况）- 温度矩阵 T 被分割成多个子矩阵，每个进程负责一个分块
    int rowsPerProcess = dimension / size;
    int localDimension = rowsPerProcess + 2; // 增加 2 行（包括额外的边界行）用于边界交换/邻域计算

    // 局部温度矩阵（每个进程分配的子矩阵，包含边界行）
    // 创建一个大小为 localDimension 的 vector，其中每个元素是 vector<double> 类型的 vector，即每一行是一个 vector<double>
    // 第二个参数再将每一行初始化为一个大小为 dimension 的 vector<double>，其中每个元素都为 0.0
    std::vector<std::vector<double> > T(localDimension, std::vector<double>(dimension, 0.0));
    std::vector<std::vector<double> > newT(localDimension, std::vector<double>(dimension, 0.0));

    // 初始化温度矩阵
    for (int i = 0; i < localDimension; ++i)
    {
        for (int j = 0; j < dimension; ++j)
        {
            T[i][j] = rank; // 简单初始化为当前进程编号
        }
    }

    // 主计算循环
    for (int timeRound = 1; timeRound <= timeRounds; ++timeRound)
    {
        // 边界交换 - 使用 MPI_Send 和 MPI_Recv 实现块之间的边界数据交换
        /**
         *虚拟边界不用于存储本进程的计算数据，但在计算过程中会被访问。
         *每个进程发送自己的顶部或底部行给相邻进程
         *同时接收相邻进程的边界行
         */
        if (rank > 0)
        {
            // 发送当前块第二行（第一行是接收的虚拟边界）
            MPI_Send(T[1].data(), dimension, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
            // 接收上块的倒数第二行，并存储在当前块的虚拟边界 T[0] 中
            MPI_Recv(T[0].data(), dimension, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (rank < size - 1)
        {
            // 发送当前块的倒数第二行（倒数第一行为虚拟边界）
            MPI_Send(T[localDimension - 2].data(), dimension, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
            MPI_Recv(T[localDimension - 1].data(), dimension, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE); // 接收下块的第二行，并存储在当前块的虚拟边界中
        }

        // 计算热扩散 - 逐点计算 - 各进程彼此本地进行 无需通信
        for (int i = 1; i < localDimension - 1; ++i)
        {
            for (int j = 1; j < dimension - 1; ++j)
            {
                newT[i][j] = T[i][j] + deltaT * (conductCoeff / (gasDensity * br)) *
                ((T[i + 1][j] + T[i - 1][j] - 2 * T[i][j]) / (deltaX * deltaX) +
                    (T[i][j + 1] + T[i][j - 1] - 2 * T[i][j]) / (deltaY * deltaY));
            }
        }

        // 更新矩阵
        T.swap(newT);

        // 保存结果到文件
        if (timeRound % 10 == 0)
        {
            // 每 10 个时间步保存一次
            saveDataFile(T, timeRound, localDimension, rank);
        }

        if (rank == 0)
        {
            std::cout << "Time round " << timeRound << " completed.\n";
        }
    }

    MPI_Finalize();
    return 0;
}
