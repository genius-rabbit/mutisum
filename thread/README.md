# thread

## 需求解析
- input.txt输入参数M N, 使用output.txt保存结果
    - M : sum from 1 to M / M<2^32
    - N : thread num / N<100
- 自动测试用例 > 5个
- 计算并输出代码运行时长

## 思路
将数字范围分段，每个线程计算各个段，最后将结果相加，将结果加到总结果上时，注意锁的使用


## usage
- make : 编译
- make test : 运行测试
- make clean : 清除中间文件
