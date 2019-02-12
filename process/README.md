# process

## 需求解析
- input.txt输入参数M N, 使用output.txt保存结果
    - M : sum from 1 to M / M<2^32
    - N : thread num / N<100
- 自动测试用例 > 5个
- 计算并输出代码运行时长

## 思路
- 主进程分配好N个进程应该计算的数据
- 主进程fork copy一份当前进程，父进程创建锁和共享内存，等待子进程结束
- 子进程作为新的父进程继续fork出N个子进程，每个进程计算分配好的数字范围，将结果加到共享内存中                                      

## usage
- make : 编译
- make test : 运行测试
- make clean : 清除中间文件