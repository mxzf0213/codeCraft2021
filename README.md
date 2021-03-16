# codeCraft2021

### 资料说明

[链接](https://developer.huaweicloud.com/hero/thread-112802-1-1.html)

3月10发布练习赛赛题相关材料如下：

※ 初赛赛题任务书

※ 训练数据集（training-data）

※ SDK

※ 2021华为软件精英挑战赛技术Q+A

※ 初赛判题错误提示说明

### 提交记录

| 次数 | 总成本     | 迁移次数 | 运行时间 | 说明                                                         |
| ---- | ---------- | -------- | -------- | ------------------------------------------------------------ |
| 1    | 1776422750 | 0        | 0.624    | 纯暴力；购买服务器和配置虚拟机都按从前向后的顺序遍历         |
| 2    | 1200734879 | 0        | 1.631    | 按照hard_cost + soft_cost * 100对服务器列表排序；购买服务器时按排好序后的服务器列表从前向后遍历选择 |
| 3    | 1161580534 | 73209    | 114.015  | 迁移目的服务器按照可用核心数从小到大排序，原服务器按照已用A核心节点数从小到大排序；部署服务器时未优先考虑部署可用核心数少的服务器，后考虑没用（当日断电）的服务器 |
| 4    | 1155674454 | 71651    | 89.578   | 服务器列表先按core_mem + 1/core_mem排序，同时对于这个差值在4.5范围以内的按照性价比升序排序，可以出现替换最频繁购买的服务器同时性价比更优的情况；另外分析数据发现频繁购买的服务器只有13种，不超过6000台，可以考虑迁移时替换链表，直接二重for循环。 |
| 5    | 1152462872 | 71795    | 86.539   | 部署时，对于cpu > mem的虚拟机，寻找剩余cpu > mem的服务器，对于cpu < mem的虚拟机同理 |
| 6    | 1150480563 | 68164    | 82.551   | 优化迁移策略：原服务器尽量选择虚拟机数量少的，若虚拟机数量一致，则优先考虑服务器核心数多的 |

