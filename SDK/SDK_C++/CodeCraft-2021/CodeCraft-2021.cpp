#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <cassert>
#include <queue>

using namespace std;
typedef long long ll;

//初赛测试集参数：
//consider_times:3
//migrate_weight:0.7~0.8
#define consider_times_cpu 2.64     //采购时考虑cpu的倍数
#define consider_times_mem 2.54     //采购时考虑mem的倍数
#define best_fit_desc_mem 1.3       //离线部署时考虑资源排序的mem占比
#define povit_weight 0.65           //采购服务器的排序方案阈值，重要！
#define pick_weight 0.32            //部署时考虑碎片中mem占比
#define migrate_weight 0.8          //迁移时考虑碎片中mem占比
#define ACTIVATE_MIGRATE
//提交前务必确保DEBUG定义被注释
#define DEBUG

set<pair<int, int> > servers_left[513];
set<pair<int, int> > servers_right[513];
set<pair<int, int> > servers_double[513];

//服务器
class server {
public:
    string mode;
    int core, mem;
    int hard_cost, soft_cost;
    double core_mem;
    int sum_cost;
    int average_cost;
    bool is_choice;

    server() {}

    server(string mode, int core, int mem, int hard_cost, int soft_cost) :
            mode(mode), core(core), mem(mem), hard_cost(hard_cost), soft_cost(soft_cost) {
        core_mem = 1.0 * core / mem;
        sum_cost = hard_cost + soft_cost * 1000;
        average_cost = (hard_cost + soft_cost * 1000) / core;
        is_choice = true;
    }
};

class _server : public server {
public:
    int index;
    int left_core, left_mem;
    int right_core, right_mem;
    unordered_set<int> vitur_ids;
    bool can_move;

    _server() {}

    _server(string mode, int core, int mem, int hard_cost, int soft_cost, int index) :
            server(mode, core, mem, hard_cost, soft_cost), index(index) {
        left_core = right_core = core / 2;
        left_mem = right_mem = mem / 2;
        can_move = true;
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
    bool alive;
    bool can_move;

    _vitur() {}

    _vitur(string mode, int core, int mem, bool double_node, int id, int server_id, int deploy_node = 0) :
            vitur(mode, core, mem, double_node), id(id), server_id(server_id), deploy_node(deploy_node) {
        alive = true;
        can_move = true;
    }
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
        if (server.core >= v.core * consider_times_cpu && server.mem >= v.mem * consider_times_mem) {
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
void dynamic_purchase(vector<pair<int, int> > &purchase_list, vector<int> &purchase_plan, vector<server> &server_list,
                      vector<int> &server_ids, vector<pair<int, int> > &deploy_plan,
                      vector<_vitur> &viturs, vector<_server> &servers) {
    int purchase_size = purchase_list.size();
    vector<int> sorted_purchase_ids(purchase_size);
    for (int i = 0; i < purchase_size; i++)sorted_purchase_ids[i] = i;
//    按照需要资源从高到低进行排序
    sort(sorted_purchase_ids.begin(), sorted_purchase_ids.end(), [&](int x, int y) {
        return viturs[purchase_list[x].first].core + best_fit_desc_mem * viturs[purchase_list[x].first].mem
               > viturs[purchase_list[y].first].core + best_fit_desc_mem * viturs[purchase_list[y].first].mem;
    });
//    当前必须要购买，所以必须从服务器最后位置遍历
    int start_pos = 0;
    for (int i = 0; i < purchase_size; i++) {
        auto cur_purchase = purchase_list[sorted_purchase_ids[i]];
        auto &v = viturs[cur_purchase.first];
        auto &_deploy = deploy_plan[cur_purchase.second];
        int remain = 1e9;
        int server_index = -1;
        bool flag = false;
        bool is_left = true;
        for (int j = start_pos; j < servers.size(); j++) {
            auto &_server = servers[j];
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
                if (_server.left_core >= v.core / 2 && _server.left_mem >= v.mem / 2 &&
                    _server.right_core >= v.core / 2 &&
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
        if (!flag) {
            bool flag2 = false;
            int best_id;
            for (auto id: server_ids) {
                auto server = server_list[id];
                if (server.core >= v.core * consider_times_cpu && server.mem >= v.mem * consider_times_mem) {
                    flag2 = true;
                    best_id = id;
                    break;
                }
            }
            if (!flag2) {
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
            server_index = servers.size() - 1;
        }
        v.server_id = server_index;
        auto &_server = servers[server_index];
        _server.vitur_ids.insert(v.id);
        _deploy.first = server_index;
        if (v.double_node) {
            _server.left_core -= v.core / 2;
            _server.right_core -= v.core / 2;
            _server.left_mem -= v.mem / 2;
            _server.right_mem -= v.mem / 2;
            _deploy.second = 0;
            v.deploy_node = 0;
        } else if (is_left) {
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

//        trick: 迁移策略，先迁移，后采购部署
//                    Best Fit Desc
//                迁移目的服务器尽量选择剩余核心数少的
//                原虚拟机尽量选择使用所在服务器虚拟机少，若虚拟机数量一致，则优先考虑服务器核心数多的
void policy_migrate_server(vector<_server> &servers, vector<_vitur> &viturs,
                           vector<pair<int, pair<int, int> > > &migrate_details,
                           Engine &engine,
                           int &max_migrate, int &cur_migrate, int &migrate_times) {
#ifdef ACTIVATE_MIGRATE
    vector<int> viturs_ids(viturs.size());
    for (int i = 0; i < viturs.size(); i++)viturs_ids[i] = i;
//    TODO: 排序策略，优先考虑虚拟机所在服务器的虚拟机个数，在此基础上进行Best Fit Desc
//            因为每次迁移都是从虚拟机少的服务器迁往虚拟机多的服务器，避免重复迁移同一个虚拟机
    sort(viturs_ids.begin(), viturs_ids.end(), [&](int x, int y) {
        if (servers[viturs[x].server_id].vitur_ids.size() != servers[viturs[y].server_id].vitur_ids.size())
            return servers[viturs[x].server_id].vitur_ids.size() <
                   servers[viturs[y].server_id].vitur_ids.size();
        return viturs[x].core > viturs[y].core;
    });
    for (int i = 0; i < 513; i++) {
        servers_left[i].clear();
        servers_right[i].clear();
        servers_double[i].clear();
    }
    int count = 0;
    for (auto &_server: servers) {
        servers_left[_server.left_core].insert({_server.left_mem, count});
        servers_right[_server.right_core].insert({_server.right_mem, count});
        servers_double[min(_server.left_core, _server.right_core)].insert(
                {min(_server.left_mem, _server.right_mem), count});
        count += 1;
    }
    for (auto v_id:viturs_ids) {
        if (!max_migrate)break;
        auto &_vitur = viturs[v_id];
        if (_vitur.alive == 0)continue;
        auto &origin_server = servers[_vitur.server_id];
        int best_id = _vitur.server_id;
        int deploy_node = _vitur.deploy_node;
        double remain;
        if (_vitur.deploy_node == 0)
            remain = (origin_server.left_core + origin_server.right_core) +
                     migrate_weight * (origin_server.left_mem + origin_server.right_mem);
        else if (_vitur.deploy_node == 1)
            remain = origin_server.left_core + migrate_weight * origin_server.left_mem;
        else
            remain = origin_server.right_core + migrate_weight * origin_server.right_mem;
        bool migrate_flag = false;
        if (_vitur.double_node) {
            for (int i = _vitur.core / 2; i < 513; i++) {
                auto iter = servers_double[i].lower_bound({_vitur.mem / 2, 0});
                while (iter != servers_double[i].end()) {
                    if (iter->second == _vitur.server_id) {
                        iter++;
                        continue;
                    }
                    if (remain < 2 * i - _vitur.core + migrate_weight * (2 * iter->first - _vitur.mem))
                        break;
                    int idx = iter->second;
                    double cur_remain = (servers[idx].left_core + servers[idx].right_core - _vitur.core) +
                                        migrate_weight * (servers[idx].left_mem + servers[idx].right_mem - _vitur.mem);
                    if (cur_remain < remain) {
                        remain = cur_remain;
                        best_id = idx;
                        deploy_node = 0;
                        migrate_flag = true;
                    }
                    iter++;
                }
            }
        } else {
            for (int i = _vitur.core; i < 513; i++) {
                auto iter = servers_left[i].lower_bound({_vitur.mem, 0});
                if (iter != servers_left[i].end() && iter->second == _vitur.server_id && 1 == _vitur.deploy_node)
                    iter++;
                if (iter != servers_left[i].end()) {
                    double cur_remain = (i - _vitur.core) + migrate_weight * (iter->first - _vitur.mem);
                    if (cur_remain < remain) {
                        remain = cur_remain;
                        best_id = iter->second;
                        deploy_node = 1;
                        migrate_flag = true;
                    }
                }
            }
            for (int i = _vitur.core; i < 513; i++) {
                auto iter = servers_right[i].lower_bound({_vitur.mem, 0});
                if (iter != servers_right[i].end() && iter->second == _vitur.server_id && 2 == _vitur.deploy_node)
                    iter++;
                if (iter != servers_right[i].end()) {
                    double cur_remain = (i - _vitur.core) + migrate_weight * (iter->first - _vitur.mem);
                    if (cur_remain < remain) {
                        remain = cur_remain;
                        best_id = iter->second;
                        deploy_node = 2;
                        migrate_flag = true;
                    }
                }
            }
        }
        if (migrate_flag == true) {
            servers_left[origin_server.left_core].erase({origin_server.left_mem, _vitur.server_id});
            servers_right[origin_server.right_core].erase({origin_server.right_mem, _vitur.server_id});
            servers_double[min(origin_server.left_core, origin_server.right_core)].erase(
                    {min(origin_server.left_mem, origin_server.right_mem), _vitur.server_id});
            if (_vitur.double_node) {
                origin_server.left_core += _vitur.core / 2;
                origin_server.left_mem += _vitur.mem / 2;
                origin_server.right_core += _vitur.core / 2;
                origin_server.right_mem += _vitur.mem / 2;
            } else if (_vitur.deploy_node == 1) {
                origin_server.left_core += _vitur.core;
                origin_server.left_mem += _vitur.mem;
            } else {
                origin_server.right_core += _vitur.core;
                origin_server.right_mem += _vitur.mem;
            }
            origin_server.vitur_ids.erase(_vitur.id);
            servers_left[origin_server.left_core].insert({origin_server.left_mem, _vitur.server_id});
            servers_right[origin_server.right_core].insert({origin_server.right_mem, _vitur.server_id});
            servers_double[min(origin_server.left_core, origin_server.right_core)].insert(
                    {min(origin_server.left_mem, origin_server.right_mem), _vitur.server_id});
            auto &new_server = servers[best_id];
            servers_left[new_server.left_core].erase({new_server.left_mem, best_id});
            servers_right[new_server.right_core].erase({new_server.right_mem, best_id});
            servers_double[min(new_server.left_core, new_server.right_core)].erase(
                    {min(new_server.left_mem, new_server.right_mem), best_id});
            if (deploy_node == 0) {
                new_server.left_core -= _vitur.core / 2;
                new_server.left_mem -= _vitur.mem / 2;
                new_server.right_core -= _vitur.core / 2;
                new_server.right_mem -= _vitur.mem / 2;
            } else if (deploy_node == 1) {
                new_server.left_core -= _vitur.core;
                new_server.left_mem -= _vitur.mem;
            } else {
                new_server.right_core -= _vitur.core;
                new_server.right_mem -= _vitur.mem;
            }
            new_server.vitur_ids.insert(_vitur.id);
            servers_left[new_server.left_core].insert({new_server.left_mem, best_id});
            servers_right[new_server.right_core].insert({new_server.right_mem, best_id});
            servers_double[min(new_server.left_core, new_server.right_core)].insert(
                    {min(new_server.left_mem, new_server.right_mem), best_id});
            _vitur.server_id = best_id;
            _vitur.deploy_node = deploy_node;
            migrate_details.push_back({_vitur.id, {best_id, deploy_node}});
            max_migrate -= 1;
            migrate_times += 1;
            cur_migrate += 1;
        }
    }
#endif
}

ll Main() {
    double left_source = 0;
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
    int K = ioEngine.read_int();
    auto &server_ids = engine.server_magic_ids;
    auto server_list = engine.server_list;
//    处理每一天的请求
    int povit_day = T * povit_weight;
    while (T--) {
        if (T > povit_day) {
            sort(server_ids.begin(), server_ids.end(), [&](int x, int y) {
                return server_list[x].soft_cost < server_list[y].soft_cost;
            });
        } else {
            sort(server_ids.begin(), server_ids.end(), [&](int x, int y) {
                return server_list[x].hard_cost < server_list[y].hard_cost;
            });
        }
//        每一天有R个请求
        int R = ioEngine.read_int();
        int _R = R;
        getchar();
        int add_op = 0;
        vector<pair<int, int> > deploy_plan;
        vector<int> purchase_plan(engine.N, 0);
        vector<pair<int, int> > purchase_list;
//        trick: 迁移策略，先迁移，后采购部署
//                迁移目的服务器尽量选择剩余核心数少的
//                原服务器尽量选择使用虚拟机少，若虚拟机数量一致，则优先考虑服务器核心数多的
        vector<pair<int, pair<int, int> > > migrate_details;
        int cur_migrate = 0;
        int max_migrate = engine.total_viturs * 3 / 100;
#ifdef ACTIVATE_MIGRATE
        auto &servers = engine.servers;
        auto &viturs = engine.viturs;
        policy_migrate_server(servers, viturs, migrate_details, engine, max_migrate,
                              cur_migrate, migrate_times);
#endif
        vector<pair<string, int> > day_requests;
        for (int i = 0; i < R; i++) day_requests.push_back(ioEngine.read_request());
        int idx = 0;
        while (R--) {
            pair<string, int> r = day_requests[idx++];
            string mode = r.first;
            int vitur_id = r.second;
//            删除
            if (mode == "") {
                continue;
            } else {
                add_op += 1;
                engine.viturs_map[vitur_id] = engine.viturs.size();
                vitur v = engine.vitur_list[engine.vitur_string_map[mode]];
                deploy_plan.push_back({0, 0});
                _vitur _v(mode, v.core, v.mem, v.double_node, vitur_id, -1, -1);
                engine.viturs.push_back(_v);
                purchase_list.push_back({engine.viturs.size() - 1, deploy_plan.size() - 1});
            }
        }
        dynamic_purchase(purchase_list, purchase_plan, engine.server_list, server_ids,
                         deploy_plan, engine.viturs, engine.servers);
        for (auto r: day_requests) {
            if (r.first != "")continue;
            int vitur_id = r.second;
            int id = engine.viturs_map[vitur_id];
            _vitur &_v = engine.viturs[id];
//                TODO: 释放_v所在服务器的资源
            int server_id = _v.server_id;
            engine.servers[server_id].vitur_ids.erase(vitur_id);
            _v.alive = false;
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
        ll left_source_cpu = 0;
        ll left_souce_mem = 0;
        ll all_source_cpu = 0;
        ll all_source_mem = 0;
        for (auto &_server: engine.servers) {
            if (_server.left_core + _server.right_core < _server.core ||
                _server.left_mem + _server.right_mem < _server.mem) {
                daily_cost += _server.soft_cost;
            }
            _server.can_move = true;
            left_source_cpu += _server.left_core + _server.right_core;
            left_souce_mem += _server.left_mem + _server.right_mem;
            all_source_cpu += _server.core;
            all_source_mem += _server.mem;
        }
        left_source = 1. * (left_source_cpu) / (all_source_cpu);
        for (auto &_vitur:viturs) {
            _vitur.can_move = true;
        }
        engine.total_viturs += add_op - (_R - add_op);
//        fflush(stdout);
    }
    all_cost = purchase_cost + daily_cost;
#ifdef DEBUG
    fprintf(stderr, "总开销 = %lld, 购买成本 = %lld, 日常开销 = %lld, 总迁移次数 = %d\n", all_cost, purchase_cost, daily_cost,
            migrate_times);
#endif
    return all_cost;
}

//training_1 and training_2 cannot define at the same time!
//提交前必须注释这几行 以及 所有DEBUG定义语句!
//#define training_1
//#define training_2
//#define sample
#define joint-debug
#define CLOCK

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
#ifdef joint-debug
    freopen("training-1.in", "r", stdin);
    freopen("training_1.out", "w", stdout);
    freopen("training_1.err", "w", stderr);
    ll cost1 = Main();
    freopen("training-2.in", "r", stdin);
    freopen("training_2.out", "w", stdout);
    freopen("training_2.err", "w", stderr);
    ll cost2 = Main();
    fprintf(stderr, "All cost = %lld\n", cost1 + cost2);
#endif
    ll cost = Main();
#ifdef CLOCK
    clock_t ends = clock();
    fprintf(stderr, "Running Time : %f\n", (double) (ends - start) / CLOCKS_PER_SEC);
#endif CLOCK
    return 0;
}

