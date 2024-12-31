#include <mpi.h>
#include <iostream>
#include <thread>      // for std::this_thread::sleep_for -  C++ 标准库的线程休眠函数
#include <chrono>      // for std::chrono::seconds - seconds 指定了以秒为单位的时间

int main(int argc, char* argv[]) {
    //当前进程的编号（排名，Rank），当前通信域中的进程总数，主机名称的长度，用于检测 MPI 是否已初始化
    int myrank, nprocs, name_len, flag;
    //记录进程睡眠操作的起始时间和结束时间
    double start_time, end_time;
    //存储主机名称 - 使用跨平台常量 MPI_MAX_PROCESSOR_NAME，确保不会因主机名过长导致缓冲区溢出
    char host_name[MPI_MAX_PROCESSOR_NAME]; // MPI_MAX_PROCESSOR_NAME is a cross-platform constant

    // 检查 MPI 是否已初始化 - flag 为非零表示 MPI 已经初始化；为零表示尚未初始化
    MPI_Initialized(&flag);
    std::cerr << "flag: " << flag << std::endl;

    // 初始化 MPI 环境
    MPI_Init(&argc, &argv);

    // 获取进程的 rank、总进程数；以及当前进程所在主机的名称，并存储在 host_name 中，同时返回名称长度
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Get_processor_name(host_name, &name_len);

    // 仅 Rank 0 的进程打印 MPI_Wtime 精度和主机名
    if (myrank == 0) {
        //返回 MPI_Wtime 的精度（通常是定时器的分辨率）
        std::cerr << "Precision of MPI_WTIME(): " << MPI_Wtick() << std::endl;
        std::cerr << "Host Name: " << host_name << std::endl;
    }

    // 使用 MPI_Wtime 测量时间并模拟任务延迟
    start_time = MPI_Wtime();//返回当前时间（通常以秒为单位），用于测量代码的执行时间
    // 当前进程暂停执行，时间为 myrank 乘以 3 秒
    // Rank 0 睡眠 0 秒，Rank 1 睡眠 3 秒，Rank 2 睡眠 6 秒，以此类推
    std::this_thread::sleep_for(std::chrono::seconds(myrank * 3));
    end_time = MPI_Wtime();

    // 每个进程打印自己的 rank 和睡眠时间（计算方法为 end_time - start_time）
    std::cerr << "myrank: " << myrank << ". I have slept "
              << end_time - start_time << " seconds." << std::endl;

    // 释放 MPI 环境，结束所有通信操作
    MPI_Finalize();

    return 0;
}

/**flag: 0
flag: 0
flag: 0
flag: 0
flag: 0
flag: 0
Precision of MPI_WTIME(): 1e-06
Host Name: komikado.local
myrank: 0. I have slept 0 seconds.
myrank: 1. I have slept 3.00387 seconds.
myrank: 2. I have slept 6.00261 seconds.
myrank: 3. I have slept 9.00269 seconds.
myrank: 4. I have slept 12.0039 seconds.
myrank: 5. I have slept 15.0024 seconds.*/
