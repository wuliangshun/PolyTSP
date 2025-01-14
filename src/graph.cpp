#include <iostream>
#include <sstream>  
#include <algorithm> // sort, next_permutation
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "graph.h"
#include "log.h"
using namespace std;


Graph::Graph(int V, int initial_vertex, bool random_graph, char* data_path) // constructor of Graph
{
	if(V < 1) // checks if number of vertexes is less than 1
	{
		cout << "Error: number of vertexes <= 0\n";
		exit(1);
	}
	
	this->V = V; // assigns the number of vertices
	this->initial_vertex = initial_vertex; // assigns initial vertex
	this->total_edges = 0; // initially the total of edges is 0
	
	if(random_graph)
		generatesGraph();
	else
	{		
		//load default graph
		if(strcmp(data_path, "../data/default_graph.txt")==0)
		{
			cout << "load graph from: " << data_path << " ..." << endl;
			ifstream fin;
			fin.open(data_path, ios::in);
			vector<string> v;
			string tmp;
			while (getline(fin, tmp))
			{
				v.push_back(tmp);

				vector<int> nums;
				const char *sep = ","; //可按多个字符来分割
				char *p;
				p = strtok((char*)tmp.c_str(), sep);			
				while(p)
				{
					int num = atoi(p);
					nums.push_back(num);
					printf("%d ", num);
					p = strtok(NULL, sep);
				}
				if(nums.size() == 3)
				{
					addVertex(nums[0]);
					addEdge(nums[0], nums[1], nums[2]);
				}
			}
		}
		else
		{
			//load vertex coord
			cout << "load graph from: " << data_path << " ..." << endl;
			map<int, pair<double, double>> map_coord;
			ifstream fin;
			fin.open(data_path, ios::in);
			string line;
			while (getline(fin, line))
			{
				vector<double> nums;
				const char *sep = " "; 
				char *p;
				p = strtok((char*)line.c_str(), sep);			
				while(p)
				{
					double num = atof(p);
					nums.push_back(num);
					p = strtok(NULL, sep);
				}
				if(nums.size() == 3) 
				{
					int vertex = (int)nums[0] - 1;// vertex index start from 0
					double x = nums[1];
					double y = nums[2];
					if (vertex >= 0)
					{
						map_coord[vertex] = make_pair(x, y);
						cout << vertex << "," << x << ","  << y << endl;
					}
				}
			}	
			// compute vertex-to-vertex distance
			this->V = map_coord.size();
			this->initial_vertex = 0;
			for(int i=0; i < this->V; i++)
			{
				addVertex(i);
				for(int j=i+1; j < this->V; j++)
				{
					pair<double, double> src_coord = map_coord[i];
					pair<double, double> dest_coord = map_coord[j];
					double distance = sqrt(pow(src_coord.first - dest_coord.first,2) + pow(src_coord.second - dest_coord.second,2));					
					addEdge(i, j, distance);
					addEdge(j, i, distance);
				}
			}
			this->total_edges = map_edges.size();
		}		
	}
}


void Graph::addVertex(int v)
{
	this->vertexs.push_back(v);
}

void Graph::delVertex(int v)
{
	for(vector<int>::iterator it=this->vertexs.begin(); it!=this->vertexs.end(); )
    {
        if(*it == v)
        {
            it = this->vertexs.erase(it);
        }
        else
        {
            ++it;
        }
	}
}

void  Graph::initConns(std::vector<std::vector<int>>& conns)
{
	//init conns
	this->conns.assign(conns.begin(),conns.end());
	//del vertexs
	for(vector<vector<int>>::iterator it = this->conns.begin(); it != this->conns.end(); it++) //enumerate conns
	{
		vector<int> conn = *it;
		for(int i = 1; i <= conn.size()-2; i++)
		{
			delVertex(conn[i]); 
		}
	}
}

void Graph::generatesGraph()
{
	//Generate all connected graph
	for(int i=0; i<V; i++)
	{
		addVertex(i);
		for(int j=i+1; j<V; j++)
		{
			double weight = rand() % V + 1;
			addEdge(i, j, weight);
			addEdge(j, i, weight);
		}
	}
}


void Graph::showInfoGraph()
{
	cout << "Showing info of graph:\n\n";
	cout << "Number of vertices: " << V;
	cout << "\nNumber of edges: " << map_edges.size() << "\n";
}


void Graph::addEdge(int src, int dest, double weight) // add a edge
{
	map_edges[make_pair(src, dest)] = weight; // adds edge in the map
}


void Graph::showGraph() // shows all connections of the graph
{	
	map<pair<int, int>, double>::iterator it;
	stringstream ss;
	for(it = map_edges.begin(); it != map_edges.end(); ++it)
		ss << it->first.first << " linked to vertex " << it->first.second << " with weight " << it->second << endl;
	cout << ss.str() << endl;
	CLog(CLOG_INFO, (char*)ss.str().c_str());
}


double Graph::existsEdge(int src, int dest) // checks if exists a edge
{
	map<pair<int, int>, double>::iterator it = map_edges.find(make_pair(src, dest));	
	if(it != map_edges.end())
		return it->second; // returns cost
	return -1;
}