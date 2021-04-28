import numpy as np
import matplotlib.pyplot as plt

plt.rcParams['font.sans-serif'] = ['SimHei']

N = 0
server_list = []
server_array = None
M = 0
virtu_list = []
virtu_array = None
T = 0
K = 0
add_req_list = []
add_req_array = None
del_req_list = []
del_req_array = None


def read_txt(path: str):
    global N, server_list, M, virtu_list, T, add_req_list, del_req_list, server_array, virtu_array, add_req_array, del_req_array
    with open(path, "r") as f:
        N = int(f.readline())
        for i in range(N):
            server_list.append(f.readline().rstrip().lstrip(
                '(').rstrip(')').split(', '))
        M = int(f.readline())
        for i in range(M):
            virtu_list.append(f.readline().rstrip().lstrip(
                '(').rstrip(')').split(", "))
        T = int(f.readline().rstrip().split(" ")[0])

        for i in range(T):
            add_temp = []
            del_temp = []
            R = int(f.readline())
            for j in range(R):
                line = f.readline().rstrip().lstrip('(').rstrip(')').split(", ")
                if line[0] == "add":
                    add_req_list.append(line)
                    add_temp.append(line[2])
                else:
                    del_req_list.append(line)
                    del_temp.append(line[1])
            for a in add_temp:
                for d in del_temp:
                    if a == d:
                        print("出现当天创建当天删除的虚拟机")
    server_array = np.array(server_list)
    virtu_array = np.array(virtu_list)
    add_req_array = np.array(add_req_list)
    del_req_array = np.array(del_req_list)


def print_list(i_list: list):
    for i in i_list:
        print(i)


def print_dict(kv_dict: dict):
    for k, v in kv_dict.items():
        print(str(k) + " : " + str(v))


def write_txt(i_list: list):
    with open("temp.txt", mode="w") as f:
        for i in i_list:
            f.write(i + "\n")


def writedict_txt(i_dict: dict):
    with open("temp.txt", mode="w") as f:
        for k, v in i_dict.items():
            f.write(str(v) + "\n")


def draw_x_y(x: np.ndarray, y: np.ndarray):
    plt.plot(x, y)
    plt.xlabel("用户报价")
    plt.ylabel("资源占用")
    plt.title("trian_2用户报价~(生命周期*(cpu+0.4*mem))")
    plt.savefig("trian_2用户报价~性价比(0.4)图.jpg")
    plt.show()


def draw_y(y: np.ndarray):
    x = np.array(range(len(y)))
    plt.bar(x, y)
    plt.xlabel("第n次请求")
    plt.ylabel("用户报价")
    plt.title("train3用户报价柱状图")
    plt.savefig("train3用户报价柱状图.jpg")
    plt.show()


def draw_life(y: np.ndarray):
    x = np.array(range(len(y)))
    plt.bar(x, y)
    plt.xlabel("第n次请求")
    plt.ylabel("生命周期")
    plt.title("train_5生命周期柱状图")
    plt.savefig("train_5生命周期柱状图.jpg")
    plt.show()


if __name__ == "__main__":
    train_id = 6
    path = "./training-data/training-" + str(train_id) + ".txt"
    read_txt(path)

    server_mode_list = server_array[:, 0].tolist()
    # print_list(server_mode_list)
    # write_txt(server_mode_list)

    server_core_list = server_array[:, 1].tolist()
    # print_list(server_core_list)
    # write_txt(server_core_list)

    server_mem_list = server_array[:, 2].tolist()
    # print_list(server_mem_list)
    # write_txt(server_mem_list)

    server_hard_cost_list = server_array[:, 3].tolist()
    # print_list(server_hard_cost_list)
    # write_txt(server_hard_cost_list)

    server_soft_cost_list = server_array[:, 4].tolist()
    # print_list(server_soft_cost_list)
    # write_txt(server_soft_cost_list)

    virtu_mode_list = virtu_array[:, 0].tolist()
    # print_list(virtu_mode_list)
    # write_txt(virtu_mode_list)

    virtu_core_list = virtu_array[:, 1].tolist()
    # print_list(virtu_core_list)
    # write_txt(virtu_core_list)

    virtu_mem_list = virtu_array[:, 2].tolist()
    # print_list(virtu_mem_list)
    # write_txt(virtu_mem_list)

    virtu_double_list = virtu_array[:, 3].tolist()
    # print_list(virtu_double_list)
    # write_txt(virtu_double_list)

    virtu_req_mode_count = {}
    for virtu_mode in virtu_mode_list:
        virtu_req_mode_count[virtu_mode] = 0
    virtu_req_mode_list = add_req_array[:, 1].tolist()
    for virtu_req_mode in virtu_req_mode_list:
        virtu_req_mode_count[virtu_req_mode] += 1
    # print_dict(virtu_req_mode_count)

    virtu_req_life_sum = {}
    for virtu_mode in virtu_mode_list:
        virtu_req_life_sum[virtu_mode] = 0
    virtu_req_life_list = add_req_array[:, 3].tolist()
    for i, virtu_req_mode in enumerate(virtu_req_mode_list):
        virtu_req_life_sum[virtu_req_mode] += int(virtu_req_life_list[i])
    # print_dict(virtu_req_life_sum)
    # writedict_txt(virtu_req_life_sum)

    virtu_req_price_sum = {}
    for virtu_mode in virtu_mode_list:
        virtu_req_price_sum[virtu_mode] = 0
    virtu_req_price_list = add_req_array[:, 4].tolist()
    for i, virtu_req_mode in enumerate(virtu_req_mode_list):
        virtu_req_price_sum[virtu_req_mode] += int(virtu_req_price_list[i])
    # print_dict(virtu_req_price_sum)
    # writedict_txt(virtu_req_price_sum)

    # 画用户报价~生命周期图
    virtu_req_price_life = []
    # print(len(virtu_req_price_list))
    for i in range(len(virtu_req_price_list)):
        virtu_req_price_life.append([int(virtu_req_price_list[i]), int(virtu_req_life_list[i])])
    virtu_req_price_life.sort(key=lambda x: (x[0], x[1]))
    virtu_req_price = np.array(virtu_req_price_life)[:, 0]
    virtu_req_life = np.array(virtu_req_price_life)[:, 1]
    # print(virtu_req_price[99999])
    # draw_x_y(virtu_req_price[0:90000], virtu_req_life[0:90000])

    # 画用户报价~(生命周期*cpu)图
    virtu_req_price_lifetcore = []
    virtu_mode_core = {}
    for i in range(len(virtu_mode_list)):
        virtu_mode_core[virtu_mode_list[i]] = int(virtu_core_list[i])
    virtu_req_lifetcore_list = []
    for i in range(len(virtu_req_life_list)):
        virtu_req_lifetcore_list.append(int(virtu_req_life_list[i]) * virtu_mode_core[virtu_req_mode_list[i]])
    for i in range(len(virtu_req_price_list)):
        virtu_req_price_lifetcore.append([int(virtu_req_price_list[i]), int(virtu_req_lifetcore_list[i])])
    virtu_req_price_lifetcore.sort(key=lambda x: (x[0], x[1]))
    virtu_req_price = np.array(virtu_req_price_lifetcore)[:, 0]
    virtu_req_lifetcore = np.array(virtu_req_price_lifetcore)[:, 1]
    # draw_x_y(virtu_req_price, virtu_req_lifetcore)

    # 画用户报价~(生命周期*(cpu+0.4*mem))图
    mem_weight = 0.4
    virtu_req_price_lifetcoreamem = []
    virtu_mode_core = {}
    virtu_mode_mem = {}
    for i in range(len(virtu_mode_list)):
        virtu_mode_core[virtu_mode_list[i]] = int(virtu_core_list[i])
        virtu_mode_mem[virtu_mode_list[i]] = int(virtu_mem_list[i])
    virtu_req_lifetcoreamem_list = []
    for i in range(len(virtu_req_life_list)):
        virtu_req_lifetcoreamem_list.append(int(virtu_req_life_list[i]) * (
                virtu_mode_core[virtu_req_mode_list[i]] + mem_weight * virtu_mode_mem[virtu_req_mode_list[i]]))
    for i in range(len(virtu_req_price_list)):
        virtu_req_price_lifetcoreamem.append([int(virtu_req_price_list[i]), int(virtu_req_lifetcoreamem_list[i])])
    virtu_req_price_lifetcoreamem.sort(key=lambda x: (x[0], x[1]))
    virtu_req_price = np.array(virtu_req_price_lifetcoreamem)[:, 0]
    virtu_req_lifetcoreamem = np.array(virtu_req_price_lifetcoreamem)[:, 1]
    # draw_x_y(virtu_req_price, virtu_req_lifetcoreamem)
    # 计算(生命周期*(cpu+0.4*mem))/用户报价比值中位数
    rate_array = virtu_req_lifetcoreamem / virtu_req_price
    # draw_y(rate_array)
    rate_med = np.median(rate_array)
    # print(rate_med)

    # 画用户报价图
    # virtu_req_price_list = add_req_array[:, 4].tolist()
    # req_price = []
    # for virtu_req_price in virtu_req_price_list:
    #     req_price.append(int(virtu_req_price))
    # draw_y(np.array(req_price))

    # 画生命周期图
    virtu_req_life_list = add_req_array[:, 3].tolist()
    req_life = []
    for virtu_req_life in virtu_req_life_list:
        req_life.append(int(virtu_req_life))
    draw_life(np.array(req_life))
