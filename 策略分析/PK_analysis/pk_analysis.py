import matplotlib.pyplot as plt
import numpy as np


def read_log(path: str) -> (list, list):
    p0_list = []
    p1_list = []
    with open(path, "r") as f:
        while True:
            line = f.readline().rstrip().split(' ')
            if line[0] == "start":
                print(line)
            elif line[0].startswith("["):
                print(line)
            elif line[0] == "Day":
                print(line)
            elif line[0].startswith("player"):
                print(line)
            elif line[0].startswith("{"):
                print(line)
            elif line[0] == "end":
                print(line)
            elif line[0] == "":
                print(line)
                break
            else:
                p0_list.append(int(line[0]))
                p1_list.append(int(line[1]))
    return p0_list, p1_list


def draw_y0_y1(y0: list, y1: list, title_name: str):
    x = range(len(y0))
    plt.bar(x, np.array(y0), label="player0lose")
    plt.bar(x, -np.array(y1), label="player1win")
    plt.title(title_name)
    plt.legend()
    plt.show()


def modify_req(p0_list: list, p1_list: list):
    for i in range(len(p0_list)):
        if p0_list[i] == p1_list[i]:
            pass
        elif p0_list[i] > p1_list[i]:
            p1_list[i] = 0
        else:
            p0_list[i] = 0


def draw_y(y: list):
    x = range(len(y))
    plt.scatter(x, y, label="player0get")
    plt.legend()
    plt.show()


if __name__ == '__main__':
    log_path = "2504_trian2.log"
    p0, p1 = read_log(log_path)
    # p0 = [1, 2, 3]
    # p1 = [4, 5, 6]
    modify_req(p0, p1)
    draw_y0_y1(p0, p1, log_path)
    # g_list = get_req(p0, p1)
    # draw_y(g_list)
