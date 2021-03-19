#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <cassert>

using namespace std;
typedef long long ll;
#define migrate_max_consider 1000
#define consider_times 2.64

#define core_mem_eps 4.5
#define pick_weight 0.32
#define ACTIVATE_MIGRATE
//提交前务必确保DEBUG定义被注释
//#define DEBUG

//服务器
class server {
public:
    string mode;
    int core, mem;
    int hard_cost, soft_cost;
    double core_mem;
    int sum_cost;
    int average_cost;

    server() {}

    server(string mode, int core, int mem, int hard_cost, int soft_cost) :
            mode(mode), core(core), mem(mem), hard_cost(hard_cost), soft_cost(soft_cost) {
        core_mem = 1.0 * core / mem;
        sum_cost = hard_cost + soft_cost * 100;
        average_cost = (hard_cost + soft_cost * 100) / core;
    }
};

class _server : public server {
public:
    int index;
    int left_core, left_mem;
    int right_core, right_mem;
    unordered_set<int> vitur_ids;

    _server() {}

    _server(string mode, int core, int mem, int hard_cost, int soft_cost, int index) :
            server(mode, core, mem, hard_cost, soft_cost), index(index) {
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
    double core_mem;

    vitur() {}

    vitur(string mode, int core, int mem, bool double_node) :
            mode(mode), core(core), mem(mem), double_node(double_node) {
        core_mem = 1.0 * core / mem;
    }
};

class _vitur : public vitur {
public:
    int id;
    int server_id;
    int deploy_node;

    _vitur() {}

    _vitur(string mode, int core, int mem, bool double_node, int id, int server_id, int deploy_node = 0) :
            vitur(mode, core, mem, double_node), id(id), server_id(server_id), deploy_node(deploy_node) {}
};

//维护数据中心资源
class Engine {
public:
    int N, M;
    int total_viturs;
    vector<server> server_list;
    vector<vitur> vitur_list;
    vector<_server> servers;
    vector<_vitur> viturs;
    unordered_map<int, int> viturs_map;
    unordered_map<string, int> vitur_string_map, server_string_map;
//    trick member data
    vector<int> server_magic_ids;

    Engine() {
        total_viturs = 0;
    }
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
#ifdef DEBUG
        fprintf(stderr, "read number: %d\n", N);
#endif
        return N;
    }

    server read_server() {
        fgets(temp, 511, stdin);
        char *token = strtok(temp, sep);
        int cnt = 0;
        while (token != NULL) {
            if (cnt == 0)mode = token;
            else if (cnt == 1)core = atoi(token);
            else if (cnt == 2)mem = atoi(token);
            else if (cnt == 3)hard_cost = atoi(token);
            else soft_cost = atoi(token);
            token = strtok(NULL, sep);
            cnt += 1;
        }
        server s(mode, core, mem, hard_cost, soft_cost);
#ifdef DEBUG
        fprintf(stderr, "read server: mode = %s, core = %d, mem = %d, hard_cost = %d, soft_cost = %d, cpu_mem = %.3f\n",
                mode.c_str(), core, mem, hard_cost, soft_cost, s.core_mem);
#endif
        return s;
    }

    vitur read_vitur() {
        fgets(temp, 511, stdin);
        char *token = strtok(temp, sep);
        int cnt = 0;
        while (token != NULL) {
            if (cnt == 0)mode = token;
            else if (cnt == 1)core = atoi(token);
            else if (cnt == 2)mem = atoi(token);
            else double_node = atoi(token);
            token = strtok(NULL, sep);
            cnt += 1;
        }
        vitur v(mode, core, mem, double_node);
#ifdef DEBUG
        fprintf(stderr, "read vitur: mode = %s, core = %d, mem = %d, double_node = %d, cpu_mem = %.3f\n",
                mode.c_str(), core, mem, double_node, v.core_mem);
#endif
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
        char *token = strtok(temp, sep);
        int cnt = 0;
        while (token != NULL) {
            if (cnt == 1)mode = token;
            else if (cnt == 2)N = atoi(token);
            token = strtok(NULL, sep);
            cnt += 1;
        }
#ifdef DEBUG
        fprintf(stderr, "read add: mode = %s, id = %d\n", mode.c_str(), N);
#endif
        return {mode, N};
    }

    pair<string, int> read_del() {
        char *token = strtok(temp, sep);
        int cnt = 0;
        while (token != NULL) {
            if (cnt == 1)N = atoi(token);
            token = strtok(NULL, sep);
            cnt += 1;
        }
#ifdef DEBUG
        fprintf(stderr, "read del: id = %d\n", N);
#endif
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

const char *IoEngine::sep = "\n (),";

struct Node {
    int id;
    Node *next;

    Node() {}

    Node(int id) : id(id) { next = NULL; }
};

/*
    policy: 最佳适应算法
    对于CPU需求大的，找剩余CPU>MEM的，并且放置之后保证剩余碎片资源最小化
    剩余碎片资源定义： REMAIN = alpha * CPU + beta * MEM , version 1.0 时采用alpha = beta = 1
    等价于  REMAIN = weight * CPU + MEM
    */
void policy_pick_server(vitur v, vector<_server> &servers, int &server_index, bool &flag) {
    int count = -1;
    int remain = 99999999;
    // 标记单节点放置的时候左还是右
    bool is_left = true;
    for (auto &_server: servers) {
        count += 1;
        // 单节点部署
        if (!v.double_node) {
            // 判断左节点是否可放
            if (_server.left_core >= v.core && _server.left_mem >= v.mem) {
                int cur_remain = (_server.left_core - v.core) + pick_weight * (_server.left_mem - v.mem);
                if (cur_remain < remain) {
                    remain = cur_remain;
                    server_index = count;
                    flag = true;
                    is_left = true;
                }
            }
            // 判断右节点
            if (_server.right_core >= v.core && _server.right_mem >= v.mem) {
                int cur_remain = (_server.right_core - v.core) + pick_weight * (_server.right_mem - v.mem);
                if (cur_remain < remain) {
                    remain = cur_remain;
                    server_index = count;
                    flag = true;
                    is_left = false;
                }
            }
        }
            // 双节点部署
        else {
            // 判断双节点资源是否都满足要求
            if (_server.left_core >= v.core / 2 && _server.left_mem >= v.mem / 2 && _server.right_core >= v.core / 2 &&
                _server.right_mem >= v.mem / 2) {
                int cur_remain = (_server.left_core - v.core / 2) + pick_weight * (_server.left_mem - v.mem / 2) +
                                 (_server.right_core - v.core / 2) + pick_weight * (_server.right_mem - v.mem / 2);
                if (cur_remain < remain) {
                    remain = cur_remain;
                    server_index = count;
                    flag = true;
                }
            }
        }
    }
}

//trick:购买服务器策略
//        按照预计总消费从前往后找第一个符合要求的服务器购买
//      当前考虑优先购买虚拟机容量consider_times倍数的服务器
int policy_purchase_server(vitur v, vector<int> server_ids, vector<server> server_list) {
    for (auto id: server_ids) {
        auto server = server_list[id];
        if (server.core >= v.core * consider_times && server.mem >= v.mem * consider_times) {
            return id;
        }
    }
    for (auto id: server_ids) {
        auto server = server_list[id];
        if (v.double_node && server.core >= v.core && server.mem >= v.mem) {
            return id;
        } else if (!v.double_node && server.core / 2 >= v.core && server.mem / 2 >= v.mem) {
            return id;
        }
    }
    return -1;
}

//trick:离线购买服务器策略
//       最佳适应下降算法
void dynamic_purchase(vector<pair<int,int> >& purchase_list, vector<int>& purchase_plan, vector<server>& server_list,
                      vector<int>& server_ids,vector<pair<int,int> >& deploy_plan,
                      vector<_vitur>& viturs, vector<_server>& servers)
{
    int purchase_size = purchase_list.size();
    vector<int> sorted_purchase_ids(purchase_size);
    for(int i=0;i < purchase_size; i++)sorted_purchase_ids[i] = i;
//    按照需要资源从高到低进行排序
    sort(sorted_purchase_ids.begin(), sorted_purchase_ids.end(),[&](int x,int y){
        return viturs[purchase_list[x].first].core + viturs[purchase_list[x].first].mem
            > viturs[purchase_list[y].first].core + viturs[purchase_list[y].first].mem;
    });
//    当前必须要购买，所以必须从服务器最后位置遍历
    int start_pos = servers.size();
    for(int i=0;i<purchase_size;i++)
    {
        auto cur_purchase = purchase_list[sorted_purchase_ids[i]];
        auto& v = viturs[cur_purchase.first];
        auto& _deploy = deploy_plan[cur_purchase.second];
        int remain = 1e9;
        int server_index = -1;
        bool flag = false;
        bool is_left = true;
        for(int j = start_pos;j < servers.size();j++)
        {
            auto& _server = servers[j];
            // 单节点部署
            if (!v.double_node) {
                // 判断左节点是否可放
                if (_server.left_core >= v.core && _server.left_mem >= v.mem) {
                    int cur_remain = (_server.left_core - v.core) + pick_weight * (_server.left_mem - v.mem);
                    if (cur_remain < remain) {
                        remain = cur_remain;
                        server_index = j;
                        flag = true;
                        is_left = true;
                    }
                }
                // 判断右节点
                if (_server.right_core >= v.core && _server.right_mem >= v.mem) {
                    int cur_remain = (_server.right_core - v.core) + pick_weight * (_server.right_mem - v.mem);
                    if (cur_remain < remain) {
                        remain = cur_remain;
                        server_index = j;
                        flag = true;
                        is_left = false;
                    }
                }
            }
                // 双节点部署
            else {
                // 判断双节点资源是否都满足要求
                if (_server.left_core >= v.core / 2 && _server.left_mem >= v.mem / 2 && _server.right_core >= v.core / 2 &&
                    _server.right_mem >= v.mem / 2) {
                    int cur_remain = (_server.left_core - v.core / 2) + pick_weight * (_server.left_mem - v.mem / 2) +
                                     (_server.right_core - v.core / 2) + pick_weight * (_server.right_mem - v.mem / 2);
                    if (cur_remain < remain) {
                        remain = cur_remain;
                        server_index = j;
                        flag = true;
                    }
                }
            }
        }
        if(!flag)
        {
            bool flag2 = false;
            int best_id;
            for (auto id: server_ids) {
                auto server = server_list[id];
                if (server.core >= v.core * consider_times && server.mem >= v.mem * consider_times) {
                    flag2 = true;
                    best_id = id;
                    break;
                }
            }
            if(!flag2) {
                for (auto id: server_ids) {
                    auto server = server_list[id];
                    if (v.double_node && server.core >= v.core && server.mem >= v.mem) {
                        best_id = id;
                        break;
                    } else if (!v.double_node && server.core / 2 >= v.core && server.mem / 2 >= v.mem) {
                        best_id = id;
                        break;
                    }
                }
            }
            auto server = server_list[best_id];
            purchase_plan[best_id] += 1;
            _server _s(server.mode, server.core, server.mem, server.hard_cost, server.soft_cost,
                       servers.size());
            servers.push_back(_s);
            server_index = servers.size() -1;
        }
        v.server_id = server_index;
        auto& _server = servers[server_index];
        _server.vitur_ids.insert(v.id);
        _deploy.first = server_index;
        if(v.double_node)
        {
            _server.left_core -= v.core /2;
            _server.right_core -= v.core /2;
            _server.left_mem -= v.mem/2;
            _server.right_mem -= v.mem/2;
            _deploy.second = 0;
            v.deploy_node = 0;
        } else if(is_left) {
            _server.left_core -= v.core;
            _server.left_mem -= v.mem;
            _deploy.second = 1;
            v.deploy_node = 1;
        } else {
            _server.right_core -= v.core;
            _server.right_mem -= v.mem;
            _deploy.second = 2;
            v.deploy_node = 2;
        }
    }
}

void Main() {
    Engine engine;
    IoEngine ioEngine;

    int N = ioEngine.read_int();
    getchar();
    engine.N = N;
    engine.server_list.resize(N);
    engine.server_magic_ids.resize(N);
    for (int i = 0; i < N; i++) {
        server s = ioEngine.read_server();
        engine.server_list[i] = s;
        engine.server_string_map[s.mode] = i;
        engine.server_magic_ids[i] = i;
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
    int migrate_times = 0;
    int T = ioEngine.read_int();
    auto &server_ids = engine.server_magic_ids;
    auto server_list = engine.server_list;
    sort(server_ids.begin(), server_ids.end(), [&](int x, int y) {
        return server_list[x].core_mem + 1 / server_list[x].core_mem <
               server_list[y].core_mem + 1 / server_list[y].core_mem;
    });
    for (int i = 0; i < server_ids.size(); i++) {
        int j;
        for (j = i; j < server_ids.size() &&
                    server_list[j].core_mem + 1 / server_list[j].core_mem - server_list[i].core_mem -
                    1 / server_list[i].core_mem <= core_mem_eps; j++);
        sort(server_ids.begin() + i, server_ids.begin() + j, [&](int x, int y) {
            return server_list[x].sum_cost < server_list[y].sum_cost;
        });
        i = j - 1;
    }
//    处理每一天的请求
    while (T--) {
//        每一天有R个请求
        int R = ioEngine.read_int();
        int _R = R;
        getchar();
        int add_op = 0;
        vector<pair<int, int> > deploy_plan;
        vector<int> purchase_plan(engine.N, 0);
        vector<pair<int,int> > purchase_list;
//        trick: 迁移策略，先迁移，后采购部署
//                迁移目的服务器尽量选择剩余核心数少的
//                原服务器尽量选择使用虚拟机少，若虚拟机数量一致，则优先考虑服务器核心数多的
        vector<pair<int, pair<int, int> > > migrate_details;
        int cur_migrate = 0;
        int max_migrate = engine.total_viturs * 5 / 1000;
#ifdef ACTIVATE_MIGRATE
        auto &servers = engine.servers;
        auto &viturs = engine.viturs;
        vector<int> servers_ids(servers.size());
        for (int i = 0; i < servers_ids.size(); i++) {
            servers_ids[i] = i;
        }
//        sort(servers_ids.begin(), servers_ids.end(),[&](int x,int y){
//            return servers[x].core / 2 - servers[x].left_core < servers[y].core / 2 - servers[y].left_core;
//        });
        sort(servers_ids.begin(), servers_ids.end(), [&](int x, int y) {
            if (servers[x].vitur_ids.size() == servers[y].vitur_ids.size()) {
                return servers[x].core > servers[y].core;
            }
            return servers[x].vitur_ids.size() < servers[y].vitur_ids.size();
        });
        Node *head = NULL;
        Node *cur = NULL;
        for (auto id: servers_ids) {
            if (head == NULL) {
                head = new Node(id);
                cur = head;
            } else {
                Node *temp = new Node(id);
                cur->next = temp;
                cur = temp;
            }
        }
        sort(servers_ids.begin(), servers_ids.end(), [&](int x, int y) {
            return servers[x].left_core + servers[x].right_core <
                   servers[y].left_core + servers[y].right_core;
        });
        for (auto id:servers_ids) {
            auto &right_server = servers[id];
            int &right_server_left_core_left = right_server.left_core;
            int &right_server_right_core_left = right_server.right_core;
            int &right_server_left_mem_left = right_server.left_mem;
            int &right_server_right_mem_left = right_server.right_mem;
            Node *key = head;
            Node *last_key = NULL;
            for (int i = 0; i < migrate_max_consider; i++) {
                if (key == NULL)break;
                if (key->id == id) {
                    last_key = key;
                    key = key->next;
                    continue;
                }
                auto &left_server = servers[key->id];
                if (left_server.vitur_ids.size() > max_migrate) {
                    last_key = key;
                    key = key->next;
                    continue;
                }
                int left_server_left_core_used = left_server.core / 2 - left_server.left_core;
                int left_server_right_core_used = left_server.core / 2 - left_server.right_core;
                int left_server_left_mem_used = left_server.mem / 2 - left_server.left_mem;
                int left_server_right_mem_used = left_server.mem / 2 - left_server.right_mem;
                bool delete_flag = false;
                if (left_server_left_core_used <= right_server_left_core_left &&
                    left_server_right_core_used <= right_server_right_core_left &&
                    left_server_left_mem_used <= right_server_left_mem_left &&
                    left_server_right_mem_used <= right_server_right_mem_left) {
                    for (auto vitur_id: left_server.vitur_ids) {
                        auto &vitur = engine.viturs[engine.viturs_map[vitur_id]];
                        vitur.server_id = id;
                        migrate_details.push_back({vitur_id, {id, vitur.deploy_node}});
                        max_migrate -= 1;
                        migrate_times += 1;
                        cur_migrate += 1;
                        right_server.vitur_ids.insert(vitur_id);
                    }
                    left_server.vitur_ids.clear();
                    left_server.left_core = left_server.core / 2;
                    left_server.right_core = left_server.core / 2;
                    left_server.left_mem = left_server.mem / 2;
                    left_server.right_mem = left_server.mem / 2;
                    right_server.left_core -= left_server_left_core_used;
                    right_server.right_core -= left_server_right_core_used;
                    right_server.left_mem -= left_server_left_mem_used;
                    right_server.right_mem -= left_server_right_mem_used;
                    if (key == head) {
                        Node *temp = head;
                        head = head->next;
                        key = head;
                        free(temp);
                        delete_flag = true;
                    } else {
                        Node *temp = key;
                        last_key->next = key->next;
                        key = key->next;
                        free(temp);
                        delete_flag = true;
                    }
                }
                if (!delete_flag) {
                    last_key = key;
                    key = key->next;
                }
            }
        }
        cur = head;
        while (cur) {
            Node *nt = cur->next;
            free(cur);
            cur = nt;
        }
#endif
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
                engine.servers[server_id].vitur_ids.erase(vitur_id);
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
                auto &servers = engine.servers;
                bool flag = false;
                int server_index = 0;
                policy_pick_server(v, servers, server_index, flag);
                if (flag == true) {
                    auto &_server = servers[server_index];
                    if (v.double_node && _server.left_core >= v.core / 2 && _server.left_mem >= v.mem / 2
                        && _server.right_core >= v.core / 2 && _server.right_mem >= v.mem / 2) {
                        _server.left_core -= v.core / 2;
                        _server.right_core -= v.core / 2;
                        _server.left_mem -= v.mem / 2;
                        _server.right_mem -= v.mem / 2;
                        _server.vitur_ids.insert(vitur_id);
                        _vitur _v(mode, v.core, v.mem, v.double_node, vitur_id, server_index);
                        engine.viturs.push_back(_v);
                        deploy_plan.push_back({server_index, 0});
                    } else if (_server.left_core >= _server.right_core) {
                        if (!v.double_node && _server.left_core >= v.core && _server.left_mem >= v.mem) {
                            _server.left_core -= v.core;
                            _server.left_mem -= v.mem;
                            _server.vitur_ids.insert(vitur_id);
                            _vitur _v(mode, v.core, v.mem, v.double_node, vitur_id, server_index, 1);
                            engine.viturs.push_back(_v);
                            deploy_plan.push_back({server_index, 1});
                        } else if (!v.double_node && _server.right_core >= v.core && _server.right_mem >= v.mem) {
                            _server.right_core -= v.core;
                            _server.right_mem -= v.mem;
                            _server.vitur_ids.insert(vitur_id);
                            _vitur _v(mode, v.core, v.mem, v.double_node, vitur_id, server_index, 2);
                            engine.viturs.push_back(_v);
                            deploy_plan.push_back({server_index, 2});
                        }
                    } else {
                        if (!v.double_node && _server.right_core >= v.core && _server.right_mem >= v.mem) {
                            _server.right_core -= v.core;
                            _server.right_mem -= v.mem;
                            _server.vitur_ids.insert(vitur_id);
                            _vitur _v(mode, v.core, v.mem, v.double_node, vitur_id, server_index, 2);
                            engine.viturs.push_back(_v);
                            deploy_plan.push_back({server_index, 2});
                        } else if (!v.double_node && _server.left_core >= v.core && _server.left_mem >= v.mem) {
                            _server.left_core -= v.core;
                            _server.left_mem -= v.mem;
                            _server.vitur_ids.insert(vitur_id);
                            _vitur _v(mode, v.core, v.mem, v.double_node, vitur_id, server_index, 1);
                            engine.viturs.push_back(_v);
                            deploy_plan.push_back({server_index, 1});
                        }
                    }
                }
//                TODO: not find, should purc/hase now!
                if (!flag) {
                    deploy_plan.push_back({0, 0});
                    _vitur _v(mode, v.core, v.mem, v.double_node, vitur_id, -1, -1);
                    engine.viturs.push_back(_v);
                    purchase_list.push_back({engine.viturs.size() - 1, deploy_plan.size() - 1});
                }
            }
        }
        dynamic_purchase(purchase_list, purchase_plan, engine.server_list, server_ids,
                         deploy_plan, engine.viturs, engine.servers);
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
        unordered_map<int, int> reorder_map;
        for (int i = 0; i < purchase_size; i++) {
            reorder_map[reorder_index[i]] = servers_size - purchase_size + i;
        }
        for (int i = 0; i < add_op; i++) {
            auto &_v = engine.viturs[engine.viturs.size() - 1 - i];
            auto &plan = deploy_plan[deploy_plan.size() - 1 - i];
            if (_v.server_id >= servers_size - purchase_size) {
                _v.server_id = reorder_map[_v.server_id];
            }
            if (plan.first >= servers_size - purchase_size) {
                plan.first = reorder_map[plan.first];
            }
        }
        sort(engine.servers.end() - purchase_size, engine.servers.end(), [&](_server x, _server y) {
            return reorder_map[x.index] < reorder_map[y.index];
        });
        ioEngine.output_purchase(purchase_type);
        for (int i = 0; i < N; i++) {
            if (purchase_plan[i]) {
                ioEngine.output_buy_server(engine.server_list[i].mode, purchase_plan[i]);
//                TODO: 计算购买服务器成本
                purchase_cost += (ll) purchase_plan[i] * engine.server_list[i].hard_cost;
            }
        }
        ioEngine.output_migration(cur_migrate);
        for (auto op: migrate_details) {
            int vitur_id = op.first;
            int target_server = op.second.first;
            int deploy_node = op.second.second;
            ioEngine.output_migrate(vitur_id, target_server, deploy_node);
        }
        for (auto plan: deploy_plan) {
            ioEngine.output_deploy(plan.first, plan.second);
        }
//        TODO: 计算每日开销
        for (auto _server: engine.servers) {
            if (_server.left_core + _server.right_core < _server.core ||
                _server.left_mem + _server.right_mem < _server.mem) {
                daily_cost += _server.soft_cost;
            }
        }
        engine.total_viturs += add_op - (_R - add_op);
    }
    all_cost = purchase_cost + daily_cost;
#ifdef DEBUG
    fprintf(stderr, "总开销 = %lld, 购买成本 = %lld, 日常开销 = %lld, 总迁移次数 = %d\n", all_cost, purchase_cost, daily_cost,
            migrate_times);
#endif
}

//training_1 and training_2 cannot define at the same time!
//提交前必须注释这几行 以及 所有DEBUG定义语句!
//#define training_1
//#define training_2
//#define sample
//#define CLOCK

int main() {
    // TODO:read standard input
    // TODO:process
    // TODO:write standard output
    // TODO:fflush(stdout);
#ifdef training_1
    freopen("training-1.in", "r", stdin);
    freopen("training_1.out", "w", stdout);
    freopen("training_1.err", "w", stderr);
#endif
#ifdef training_2
    freopen("training-2.in", "r", stdin);
    freopen("training_2.out", "w", stdout);
    freopen("training_2.err", "w", stderr);
#endif
#ifdef sample
    freopen("sample.in", "r", stdin);
    freopen("sample.out", "w", stdout);
    freopen("sample.err", "w", stderr);
#endif
#ifdef CLOCK
    clock_t start = clock();
#endif CLOCK
    Main();
#ifdef CLOCK
    clock_t ends = clock();
    fprintf(stderr, "Running Time : %f\n", (double) (ends - start) / CLOCKS_PER_SEC);
#endif CLOCK
    return 0;
}

