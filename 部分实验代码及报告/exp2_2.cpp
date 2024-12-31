#include <iostream>
#include <mpi.h>
#include <chrono>
#include <cmath>

double Trap(double a, double b, int n, double h)
/**
 *函数 Trap 使用梯形法计算当前进程负责区间的积分面积
 */
{
    double integral = (3.0 + 2.345 * a + 0.98372 * a * a + 0.3221 * a * a * a) / 2.0;
    for (int i = 1; i < n; i++)
    {
        double x = a + i * h;
        integral += 3.0 + 2.345 * x + 0.98372 * x * x + 0.3221 * x * x * x;
    }
    integral += (3.0 + 2.345 * b + 0.98372 * b * b + 0.3221 * b * b * b) / 2.0;
    integral *= h;
    return integral;
}

int main(int argc, char** argv)
{
    int myrank, nprocs;//nprocs 是 MPI 进程的总数
    //local_int 是每个进程计算的局部积分结果，total_int 是进程 0 汇总的所有进程的积分结果
    double start = 0.0, end = 1.0, local_a, local_b, h, local_int, total_int;
    int fre = 5000000; // 全局的梯形数目 - 调整此值测试性能
    auto start_time = std::chrono::high_resolution_clock::now();

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    h = (end - start) / fre;//每个小梯形的宽度
    int local_n = fre / nprocs;//每个进程需要处理的梯形数量，
    //每个进程根据自己的 myrank，确定分配到的积分区间 [local_a,local_b]
    local_a = start + myrank * local_n * h;//每个进程计算的积分区间起始点
    local_b = local_a + local_n * h;//每个进程计算的积分区间结束点

    //每个进程计算自己负责区间的面积 local_int
    local_int = Trap(local_a, local_b, local_n, h);

    if (myrank != 0)
        //非 0 进程 - 将局部结果 local_int 发送给进程 0
    {
        MPI_Send(&local_int, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
    else
        //进程 0 - 接收其他进程的局部结果，并将其加到 total_int 中，得到最终结果
    {
        total_int = local_int;
        for (int source = 1; source < nprocs; source++)
        {
            MPI_Recv(&local_int, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_int += local_int;
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end_time - start_time;
        std::cout << "Area from " << start << " to " << end << " = " << total_int << std::endl;
        std::cout << "MPI耗时: " << elapsed.count() << " 毫秒" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
/*
Area from 0 to 1 = 4.58093
MPI耗时: 39.1414 毫秒
*/
