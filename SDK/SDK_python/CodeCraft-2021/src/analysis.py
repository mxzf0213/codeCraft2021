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
            R = int(f.readline())
            for j in range(R):
                line = f.readline().rstrip().lstrip('(').rstrip(')').split(", ")
                if line[0] == "add":
                    add_req_list.append(line)
                else:
                    del_req_list.append(line)
    server_array = np.array(server_list)
    virtu_array = np.array(virtu_list)
    add_req_array = np.array(add_req_list)
    del_req_array = np.array(del_req_list)


if __name__ == "__main__":
    path = "./training-data/training-2.txt"
    read_txt(path)
    virtu_mode_list = virtu_array[:, 0].tolist()
    virtu_req_mode_count = {}
    for virtu_mode in virtu_mode_list:
        virtu_req_mode_count[virtu_mode] = 0
    virtu_req_mode_list = add_req_array[:, 1].tolist()
    for virtu_req_mode in virtu_req_mode_list:
        virtu_req_mode_count[virtu_req_mode] += 1
    print(virtu_req_mode_count)
    with open("./output-data/vitu_req_mode_cout-2.txt", "w") as f:
        for k, v in virtu_req_mode_count.items():
            f.write(str(v)+"\n")
