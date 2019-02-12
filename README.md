# multisum

## process 多进程求和
- 采用fork，linux的unistd.h库
- 求1~M的和，线程数为N
- N = 1~20（不包括20），M = 1 ~ 2^32（不包括2^32）

## thread 多线程求和
- 采用pthread
- 求1~M的和，线程数为N
- N = 1~20（不包括20），M = 1 ~ 2^32（不包括2^32）