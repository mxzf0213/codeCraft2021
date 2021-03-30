class Server:
    """
    服务器类
    mode:服务器型号
    core:CPU核心数
    mem:内存大小
    hard_cost:硬件成本
    soft_cost:每天能耗成本
    """

    def __init__(self, mode: str, core: int, mem: int, hard_cost: int, soft_cost: int):
        self.mode = mode
        self.core = core
        self.mem = mem
        self.hard_cost = hard_cost
        self.soft_cost = soft_cost


class Server1(Server):
    """
    使用服务器类
    mode:服务器型号
    core:CPU核心数
    mem:内存大小
    hard_cost:硬件成本
    soft_cost:每天能耗成本
    index:服务器序号
    left_core:服务器A节点核心数
    right_core:服务器B节点核心数
    left_mem:服务器A节点内存大小
    right_mem:服务器B节点内存大小
    """

    def __init__(self, mode: str, core: int, mem: int, hard_cost: int, soft_cost: int, index: int):
        super().__init__(mode, core, mem, hard_cost, soft_cost)
        self.index = index
        self.left_core = self.right_core = self.core / 2
        self.left_mem = self.right_mem = self.mem / 2


class Vitur:
    """
    虚拟机类
    mode:虚拟机型号
    core:虚拟机CPU核心数
    mem:虚拟机内存大小
    double_node:是否是双节点部署
    """

    def __init__(self, mode: str, core: int, mem: int, double_node: bool):
        self.mode = mode
        self.core = core
        self.mem = mem
        self.double_node = double_node


class Vitur1(Vitur):
    """
    使用虚拟机类
    mode:虚拟机型号
    core:虚拟机CPU核心数
    mem:虚拟机内存大小
    double_node:是否双节点部署
    id:虚拟机id
    server_id:服务器id
    deploy_node:部署节点
    """

    def __init__(self, mode: str, core: int, mem: int, double_node: bool, id: int, server_id: int,
                 deploy_node: int = 0):
        super().__init__(mode, core, mem, double_node)
        self.id = id
        self.server_id = server_id
        self.deploy_node = deploy_node


class Engine:
    """
    数据中心资源类
    """

    def __init__(self):
        self.N = 0
        self.M: int
        self.server_list: list
        self.vitur_list: list
        self.servers: list
        self.viturs: list
        self.viturs_map: dict
        self.vitur_string_map: dict
        self.server_string_map: dict
        # trick member data
        self.server_magic_ids: list

    def read_txt(self, path):
        with open(path, "r") as f:
            lines = f.readlines()
            print(lines)
            self.N = int(lines[0])
            del lines[0]





def main():
    # sys.stdout.flush()


if __name__ == "__main__":
    main()
