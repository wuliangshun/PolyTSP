#ifndef BFS_H
#define BFS_H

#include <iostream>
#include <vector>
#include "graph.h"
using namespace std;

#define maxx 9999
#define NO             0
#define PRINT          1
#define PRINT_AND_LOG  2
#define LOG            3

class BFS
{
private:
	Graph* graph; // the graph
	int logType;
	char logInfo[100];
	
	int l[maxx][maxx];//存储两个城市之间的距离
	int n;//城市数量
	int min_l = maxx;//最短路径
	int sum[maxx];//标记每条路线的路程总长度
	int go_city;//标记从第go_city个城市出发
	int visited[maxx]; //第i个城市已经去过：visited[i]=1;反之则为visited[i]=0;
	int path_index = 1; //已经去过的城市数目。
	int path[maxx][maxx];//存储经过城市的路线
	int route = 0;//存储第几条路线
public:
	BFS(Graph* graph);
	int recursion(int index);
	void bfs();
};




#endif