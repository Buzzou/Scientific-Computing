#include <iostream>
#include <chrono>
#include <cmath>

double f(double x) {
    return 3.0 + 2.345 * x + 0.98372 * x * x + 0.3221 * x * x * x;
}

/**
* 用数值积分方法计算函数 𝑓(𝑥)=3.0+2.345∗𝑥+0.98372∗𝑥^2+0.3221∗𝑥^3 的特定区间的面积，并记录求解所花费的时间
 * @return
 */
int main() {
    //修改 fre 的值（例如：1,000，10,000，100,000，1,000,000，50,000,000），可以观察面积计算结果和时间消耗的变化。
    const int fre = 50000000; // 子区间数量（积分的采样点数量）
    double start = 0.0;
    double end = 1.0;
    double estimat = 0.0; //用于存储计算出的面积估计值
    double delta = (end - start) / fre;//每个积分子区间的宽度

    // 开始计时
    auto start_time = std::chrono::high_resolution_clock::now();

    //数值积分部分
    for (int i = 0; i < fre; i++) {
        estimat += std::abs(delta * (f(delta * i) + f(delta * (i + 1))) / 2);
    }

    // 结束计时
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end_time - start_time;

    std::cout << "Area is " << estimat << std::endl;
    std::cout << "CPU耗时: " << elapsed.count() << " 毫秒" << std::endl;

    return 0;
}
/*
Area is 4.58093
CPU耗时: 545.224 毫秒
*/
