#include <iostream>
#include <vector>
using namespace std;

class server
{
    string mode;
    int core, mem;
    int hard_cost, soft_cost;
};

class vitur
{
    string mode;
    int core, mem;
    bool double_node;
};

//维护数据中心资源
class Engine
{
    vector<server> servers;
    vector<vitur> viturs;
};

int main()
{
	// TODO:read standard input
	// TODO:process
	// TODO:write standard output
	// TODO:fflush(stdout);

	return 0;
}
