#include "bfs.h"
#include "log.h"
#include <string.h>
#include <sstream>

BFS::BFS(Graph* graph)
{
	this->graph = graph;
	this->logType = PRINT_AND_LOG;
	strcpy(this->logInfo,  "BFS");
	
	//Paragrams
	this->n = graph->V;	
	for(int i=1;i<=n;i++)
    {
        for(int j=i+1;j<=n;j++)
        {
			l[i][j] = graph->existsEdge(i-1, j-1);
            l[j][i] = l[i][j];
        }
    }
}
void BFS::bfs()
{
	for(int i=1; i<=n; i++)
	{
		go_city = i;
		visited[go_city] = 1;
		recursion(go_city);
	}	
	
}

int BFS::recursion(int index)
{
    if(path_index != n)
    {
        for(int i=1;i <= n;i++)
        {
            if(visited[i] == 0)
            {
                visited[i] = 1;
                path[route][path_index] = index;
                path_index++;
                recursion(i);
                //回溯
                path_index--;
                visited[i] = 0;
            }
        }
    }
    else
    {
        //路线中加上最后一个城市和第一个城市（需要返回到最初的城市）
        path[route][path_index] = index;
        path[route][path_index + 1] = go_city;
		
        //计算每条路线的路程总长度,并输出路线
		
		
		
		
        cout << "路线" << route+1 << "为：" << endl;
		stringstream ss;
		ss << logInfo << ":";
        sum[route] = 0;
        for(int i=1; i<=n; i++)
        {
            sum[route] += l[ path[route][i] ][ path[route][i+1] ];
            ss << path[route][i] << "  ";
            //当route+1后，path[route][i]的前面需要保持，后面变化。
            path[route+1][i] = path[route][i];
        }
        if(min_l > sum[route])
        {
            min_l = sum[route];
        }
        //cout << path[route][n+1] << endl;
        ss << " | Cost:" << sum[route] << endl;
		if(logType == PRINT_AND_LOG || logType == PRINT)
			cout << ss.str() << endl;
		if(logType == PRINT_AND_LOG || logType == LOG)
			CLog(CLOG_INFO, (char*)ss.str().c_str());
        route++;
		
    }
    return 0;
	
	
}