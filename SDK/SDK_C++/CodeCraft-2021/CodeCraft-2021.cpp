#include <vector>
#include <unordered_map>
#include <cstdio>
#include <algorithm>
#include <assert.h>
#include <cstring>
using namespace std;
typedef long long ll;

//服务器
class server {
public:
    string mode;
    int core, mem;
    int hard_cost, soft_cost;
    server(){}
    server(string mode, int core, int mem, int hard_cost, int soft_cost) :
            mode(mode), core(core), mem(mem), hard_cost(hard_cost), soft_cost(soft_cost) {}
};

class _server : public server {
public:
    int index;
    int left_core, left_mem;
    int right_core, right_mem;
    _server(){}
    _server(string mode, int core, int mem, int hard_cost, int soft_cost,int index) :
            server(mode, core, mem, hard_cost, soft_cost),index(index) {
        left_core = right_core = core / 2;
        left_mem = right_mem = mem / 2;
    }
};

//虚拟机
class vitur {
public:
    string mode;
    int core, mem;
    bool double_node;
    vitur(){}
    vitur(string mode, int core, int mem, bool double_node) :
            mode(mode), core(core), mem(mem), double_node(double_node) {}
};

class _vitur : public vitur {
public:
    int id;
    int server_id;
    int deploy_node;
    _vitur(){}
    _vitur(string mode, int core, int mem, bool double_node, int id, int server_id, int deploy_node = 0) :
            vitur(mode, core, mem, double_node), id(id), server_id(server_id), deploy_node(deploy_node) {}
};

//维护数据中心资源
class Engine {
public:
    int N, M;
    vector<server> server_list;
    vector<vitur> vitur_list;
    vector<_server> servers;
    vector<_vitur> viturs;
    unordered_map<int, int> viturs_map;
    unordered_map<string, int> vitur_string_map, server_string_map;
};

class IoEngine {
private:
    int N;
    char buffer[512];
    char temp[512];
    string mode;
    int core, mem, hard_cost, soft_cost;
    bool double_node;
    static const char *sep;
public:
    int read_int() {
        fscanf(stdin, "%d", &N);
//        fprintf(stderr, "read number: %d\n", N);
        return N;
    }

    server read_server() {
        fgets(temp, 511, stdin);
        char* token = strtok(temp, sep);
        int cnt = 0;
        while(token != NULL)
        {
            if(cnt == 0)mode = token;
            else if(cnt == 1)core = atoi(token);
            else if(cnt == 2)mem = atoi(token);
            else if(cnt == 3)hard_cost = atoi(token);
            else soft_cost = atoi(token);
            token = strtok(NULL, sep);
            cnt += 1;
        }
        server s(mode, core, mem, hard_cost, soft_cost);
//        fprintf(stderr, "read server: mode = %s, core = %d, mem = %d, hard_cost = %d, soft_cost = %d\n",
//                mode.c_str(), core, mem, hard_cost, soft_cost);
        return s;
    }

    vitur read_vitur() {
        fgets(temp, 511, stdin);
        char* token = strtok(temp, sep);
        int cnt = 0;
        while(token != NULL)
        {
            if(cnt == 0)mode = token;
            else if(cnt == 1)core = atoi(token);
            else if(cnt == 2)mem = atoi(token);
            else double_node = atoi(token);
            token = strtok(NULL, sep);
            cnt += 1;
        }
        vitur v(mode, core, mem, double_node);
//        fprintf(stderr, "read vitur: mode = %s, core = %d, mem = %d, double_node = %d\n",
//                mode.c_str(), core, mem, double_node);
        return v;
    }

    pair<string, int> read_request() {
        fgets(temp, 511, stdin);
        if (temp[1] == 'a')
            return read_add();
        else
            return read_del();
    }

    pair<string, int> read_add() {
        char* token = strtok(temp, sep);
        int cnt = 0;
        while(token != NULL)
        {
            if(cnt == 1)mode = token;
            else if(cnt == 2)N = atoi(token);
            token = strtok(NULL, sep);
            cnt += 1;
        }
//        fprintf(stderr, "read add: mode = %s, id = %d\n",mode.c_str(), N);
        return {mode, N};
    }

    pair<string, int> read_del() {
        char* token = strtok(temp, sep);
        int cnt = 0;
        while(token != NULL)
        {
            if(cnt == 1)N = atoi(token);
            token = strtok(NULL, sep);
            cnt += 1;
        }
//        fprintf(stderr, "read del: id = %d\n", N);
        return {"", N};
    }

    void output_purchase(int N) {
        fprintf(stdout, "(purchase, %d)\n", N);
    }

    void output_buy_server(string mode, int number) {
        fprintf(stdout, "(%s, %d)\n", mode.c_str(), number);
    }

    void output_migration(int N) {
        fprintf(stdout, "(migration, %d)\n", N);
    }

    void output_migrate(int vitur_id, int target_id, int node = 0) {
        if (node)
            fprintf(stdout, "(%d, %d, %s)\n", vitur_id, target_id, node == 1 ? "A" : "B");
        else
            fprintf(stdout, "(%d, %d)\n", vitur_id, target_id);
    }

    void output_deploy(int server_id, int node = 0) {
        if (node)
            fprintf(stdout, "(%d, %s)\n", server_id, node == 1 ? "A" : "B");
        else
            fprintf(stdout, "(%d)\n", server_id);
    }
};
const char* IoEngine::sep = "\n (),";

void Main() {
    Engine engine;
    IoEngine ioEngine;

    int N = ioEngine.read_int();
    getchar();
    engine.N = N;
    engine.server_list.resize(N);
    for (int i = 0; i < N; i++) {
        server s = ioEngine.read_server();
        engine.server_list[i] = s;
        engine.server_string_map[s.mode] = i;
    }

    int M = ioEngine.read_int();
    getchar();
    engine.M = M;
    engine.vitur_list.resize(M);
    for (int i = 0; i < M; i++) {
        vitur v = ioEngine.read_vitur();
        engine.vitur_list[i] = v;
        engine.vitur_string_map[v.mode] = i;
    }

    ll purchase_cost = 0, daily_cost = 0, all_cost = 0;
    int T = ioEngine.read_int();
//    处理每一天的请求
    while (T--) {
//        每一天有R个请求
        int R = ioEngine.read_int();
        getchar();
        int add_op = 0;
        vector<pair<int, int> > deploy_plan;
        vector<int> purchase_plan(engine.N, 0);
        while (R--) {
            pair<string, int> r = ioEngine.read_request();
            string mode = r.first;
            int vitur_id = r.second;
//            删除
            if (mode == "") {
                int id = engine.viturs_map[vitur_id];
                _vitur _v = engine.viturs[id];
//                TODO: 释放_v所在服务器的资源
                int server_id = _v.server_id;
                if (_v.double_node) {
                    engine.servers[server_id].left_core += _v.core / 2;
                    engine.servers[server_id].left_mem += _v.mem / 2;
                    engine.servers[server_id].right_core += _v.core / 2;
                    engine.servers[server_id].right_mem += _v.mem / 2;
                } else {
                    int deploy_node = _v.deploy_node;
                    if (deploy_node == 1) {
                        engine.servers[server_id].left_core += _v.core;
                        engine.servers[server_id].left_mem += _v.mem;
                    } else {
                        engine.servers[server_id].right_core += _v.core;
                        engine.servers[server_id].right_mem += _v.mem;
                    }
                }
            } else {
                add_op += 1;
                engine.viturs_map[vitur_id] = engine.viturs.size();
                vitur v = engine.vitur_list[engine.vitur_string_map[mode]];
//                TODO: find a _server or purchase
//                policy: 找到第一个能用的服务器
                int server_id;
                auto& servers = engine.servers;
                bool flag = false;
                int server_index = 0;
                for (auto &_server: servers) {
                    if (v.double_node && _server.left_core >= v.core / 2 && _server.left_mem >= v.mem / 2
                        && _server.right_core >= v.core / 2 && _server.right_mem >= v.mem / 2) {
                        _server.left_core -= v.core / 2;
                        _server.right_core -= v.core / 2;
                        _server.left_mem -= v.mem / 2;
                        _server.right_mem -= v.mem / 2;
                        _vitur _v(mode, v.core, v.mem, v.double_node, vitur_id, server_index);
                        engine.viturs.push_back(_v);
                        deploy_plan.push_back({server_index, 0});
                        flag = true;
                        break;
                    } else if (!v.double_node && _server.left_core >= v.core && _server.left_mem >= v.mem) {
                        _server.left_core -= v.core;
                        _server.left_mem -= v.mem;
                        _vitur _v(mode, v.core, v.mem, v.double_node, vitur_id, server_index, 1);
                        engine.viturs.push_back(_v);
                        deploy_plan.push_back({server_index, 1});
                        flag = true;
                        break;
                    } else if (!v.double_node && _server.right_core >= v.core && _server.right_mem >= v.mem) {
                        _server.right_core -= v.core;
                        _server.right_mem -= v.mem;
                        _vitur _v(mode, v.core, v.mem, v.double_node, vitur_id, server_index, 2);
                        engine.viturs.push_back(_v);
                        deploy_plan.push_back({server_index, 2});
                        flag = true;
                        break;
                    }
                    server_index += 1;
                }
//                TODO: not find, should purchase now!
                if (!flag) {
                    for (int i = 0; i < N; i++) {
                        auto server = engine.server_list[i];
                        if (v.double_node && server.core >= v.core && server.mem >= v.mem) {
                            purchase_plan[i] += 1;
                            _server _s(server.mode, server.core, server.mem, server.hard_cost, server.soft_cost, engine.servers.size());
                            _s.left_core -= v.core / 2;
                            _s.left_mem -= v.mem / 2;
                            _s.right_core -= v.core / 2;
                            _s.right_mem -= v.mem / 2;
                            engine.servers.push_back(_s);
                            _vitur _v(mode, v.core, v.mem, v.double_node, vitur_id, engine.servers.size() - 1);
                            engine.viturs.push_back(_v);
                            deploy_plan.push_back({server_index, 0});
                            break;
                        } else if (!v.double_node && server.core / 2 >= v.core && server.mem / 2 >= v.mem) {
                            purchase_plan[i] += 1;
                            _server _s(server.mode, server.core, server.mem, server.hard_cost, server.soft_cost, engine.servers.size());
                            _s.left_core -= v.core;
                            _s.left_mem -= v.mem;
                            engine.servers.push_back(_s);
                            _vitur _v(mode, v.core, v.mem, v.double_node, vitur_id, engine.servers.size() - 1, 1);
                            engine.viturs.push_back(_v);
                            deploy_plan.push_back({server_index, 1});
                            break;
                        }
                    }
                }
            }
        }
//        TODO: reorder, 根据购买规则需要对新添加的服务器重新排序
//            修改内容包括添加的服务器，添加的虚拟机，部署计划
        int servers_size = engine.servers.size();
        int purchase_type = 0, purchase_size = 0;
        for (auto plan: purchase_plan) {
            if (plan) purchase_type += 1;
            purchase_size += plan;
        }
        vector<int> reorder_index(purchase_size);
        for (int i = 0; i < purchase_size; i++)reorder_index[i] = servers_size - 1 - i;
        sort(reorder_index.begin(), reorder_index.end(), [&](int x, int y) {
            return engine.server_string_map[engine.servers[x].mode] <
                   engine.server_string_map[engine.servers[y].mode];
        });
        unordered_map<int,int> reorder_map;
        for(int i=0;i<purchase_size;i++)
        {
            reorder_map[reorder_index[i]] = servers_size - purchase_size + i;
        }
        for(int i=0;i<add_op;i++)
        {
            auto& _v = engine.viturs[engine.viturs.size() - 1 - i];
            auto& plan = deploy_plan[deploy_plan.size() - 1 - i];
            assert(_v.server_id == plan.first);
            if(_v.server_id >= servers_size - purchase_size)
            {
                _v.server_id = reorder_map[_v.server_id];
            }
            if(plan.first >= servers_size - purchase_size)
            {
                plan.first = reorder_map[plan.first];
            }
//            assert(_v.server_id == plan.first);
        }
        sort(engine.servers.end() - purchase_size, engine.servers.end(),[&](_server x,_server y){
            return reorder_map[x.index] < reorder_map[y.index];
        });
        ioEngine.output_purchase(purchase_type);
        for(int i=0;i<N;i++)
        {
            if(purchase_plan[i])
            {
                ioEngine.output_buy_server(engine.server_list[i].mode, purchase_plan[i]);
//                TODO: 计算购买服务器成本
                purchase_cost += (ll)purchase_plan[i] * engine.server_list[i].hard_cost;
            }
        }
        ioEngine.output_migration(0);
        for(auto plan: deploy_plan)
        {
            ioEngine.output_deploy(plan.first, plan.second);
        }
//        TODO: 计算每日开销
        for(auto _server: engine.servers)
        {
            if(_server.left_core + _server.right_core < _server.core ||
                _server.left_mem + _server.right_mem < _server.mem)
            {
                daily_cost += _server.soft_cost;
            }
        }
    }
    all_cost = purchase_cost + daily_cost;
//    fprintf(stderr, "总开销 = %lld, 购买成本 = %lld, 日常开销 = %lld\n", all_cost, purchase_cost, daily_cost);
}

//#define DY
int main() {
    // TODO:read standard input
    // TODO:process
    // TODO:write standard output
    // TODO:fflush(stdout);
#ifdef DY
    freopen("training-2.in", "r", stdin);
    freopen("training_2.out", "w", stdout);
    freopen("training_2.err", "w", stderr);
#endif
    Main();
    return 0;
}
