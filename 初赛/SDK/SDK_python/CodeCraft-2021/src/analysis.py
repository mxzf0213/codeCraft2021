import numpy as np

N = 0
server_list = []
server_array = None
M = 0
virtu_list = []
virtu_array = None
T = 0
add_req_list = []
add_req_array = None
del_req_list = []
del_req_array = None


def read_txt(path: str):
    global N, server_list, M, virtu_list, T, add_req_list, del_req_list, server_array, virtu_array, add_req_array, del_req_array
    with open(path, "r") as f:
        N = int(f.readline())
        for i in range(N):
            server_list.append(f.readline().rstrip().lstrip('(').rstrip(')').split(', '))
        M = int(f.readline())
        for i in range(M):
            virtu_list.append(f.readline().rstrip().lstrip('(').rstrip(')').split(", "))
        T = int(f.readline())

        for i in range(T):
            # print(i)
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
            # print("创建")
            # print(len(add_temp))
            # print("删除")
            # print(len(del_temp))
            # for a in add_temp:
            #     for d in del_temp:
            #         if a == d:
            #             print("出现当天创建当天删除的虚拟机")
            # print(add_req_list)
            # print(del_req_list)
            # print()
            # break
    server_array = np.array(server_list)
    virtu_array = np.array(virtu_list)
    add_req_array = np.array(add_req_list)
    del_req_array = np.array(del_req_list)


def print_list(i_list: list):
    for i in i_list:
        print(i)


if __name__ == "__main__":
    trian_id = 1
    path = "./training-data/training-" + str(trian_id) + ".txt"
    read_txt(path)

    server_mode_list = server_array[:, 0].tolist()
    # print_list(server_mode_list)

    server_core_list = server_array[:, 1].tolist()
    # print_list(server_core_list)

    server_mem_list = server_array[:, 2].tolist()
    # print_list(server_mem_list)

    server_hard_cost_list = server_array[:, 3].tolist()
    # print_list(server_hard_cost_list)

    server_soft_cost_list = server_array[:, 4].tolist()
    # print_list(server_soft_cost_list)

    virtu_mode_list = virtu_array[:, 0].tolist()
    # print_list(virtu_mode_list)

    virtu_core_list = virtu_array[:, 1].tolist()
    # print_list(virtu_core_list)

    virtu_mem_list = virtu_array[:, 2].tolist()
    # print_list(virtu_mem_list)

    virtu_double_list = virtu_array[:, 3].tolist()
    # print_list(virtu_double_list)

    virtu_req_mode_count = {}
    for virtu_mode in virtu_mode_list:
        virtu_req_mode_count[virtu_mode] = 0
    virtu_req_mode_list = add_req_array[:, 1].tolist()
    for virtu_req_mode in virtu_req_mode_list:
        virtu_req_mode_count[virtu_req_mode] += 1
