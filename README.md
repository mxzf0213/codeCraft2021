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
| 2    | 1200734879 | 0        | 1.631    | 采购：按照hard_cost + soft_cost * 100对服务器列表排序；购买服务器时按排好序后的服务器列表从前向后遍历选择 |
| 3    | 1161580534 | 73209    | 114.015  | 迁移：目的服务器按照可用核心数从小到大排序，原服务器按照已用A核心节点数从小到大排序；部署服务器时未优先考虑部署可用核心数少的服务器，后考虑没用（当日断电）的服务器 |
| 4    | 1155674454 | 71651    | 89.578   | 采购：服务器列表先按core_mem + 1/core_mem排序，同时对于这个差值在4.5范围以内的按照性价比升序排序，可以出现替换最频繁购买的服务器同时性价比更优的情况；另外分析数据发现频繁购买的服务器只有13种，不超过6000台，可以考虑迁移时替换链表，直接二重for循环。 |
| 5    | 1152462872 | 71795    | 86.539   | 部署：部署时，对于cpu > mem的虚拟机，寻找剩余cpu > mem的服务器，对于cpu < mem的虚拟机同理 |
| 6    | 1150480563 | 68164    | 82.551   | 迁移：优化迁移策略：原服务器尽量选择虚拟机数量少的，若虚拟机数量一致，则优先考虑服务器核心数多的 |
| 7    | 1147361398 | 64355    | 70.346   | 采购：购买服务器时优先考虑满足(2 * cpu, 2 * mem)的服务器     |
| 8    | 1141681048 | 60880    | 57.256   | 采购：购买服务器参数consider_times = 2.64最佳                |
| 9    | 1118273385 | 62079    | 60.452   | 部署：最佳适应算法（Best Fit），效果显著；下一步考虑最佳适应下降算法 |
| 10   | 1115792235 | 61760    | 58.776   | 部署：优化pick_weight参数为0.32；下一步考虑离线必须要买服务器的虚拟机，应用最佳适应下降算法 |
| 11   | 1115249897 | 61280    | 60.488   | 迁移：优化目的服务器排序函数，添加右节点core至排序函数       |
| 12   | 1111369067 | 62580    | 78.919   | 采购：应用离线采购策略和最佳适应下降算法（Best Fit Desc）    |
| 13   | 1087303603 | 126694   | 112.695  | 迁移：枚举服务器改为枚举虚拟机；虚拟机按所在服务器虚拟机个数从小到大优先排序，一样则按资源从大到小；服务器按剩余资源从小到大排序 |

