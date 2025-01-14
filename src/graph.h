#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <map>
#include <set>
#include <utility> // pair
#include <time.h> // time
#include <stdlib.h> // srand, rand


// class that represents the graph
class Graph
{
public:
	int V; // number of vertices
	int total_edges; // total of edges
	int initial_vertex; // initial vertex
	std::map<std::pair<int, int>, double> map_edges; // map of the edges
	
	std::vector<std::vector<int>> conns;//conns
	std::vector<int> vertexs;//  vertexs set
public:
	Graph(int V, int initial_vertex, bool random_graph = false, char* data_path = ""); // constructor
	void addEdge(int v1, int v2, double weight); // adds a edge
	void showGraph(); // shows all the links of the graph
	void generatesGraph(); // generates a random graph
	void showInfoGraph(); // shows info of the graph
	double existsEdge(int src, int dest); // checks if exists a edge
	friend class Genetic; // to access private membres this class
	void addVertex(int v); // add a vertex
	void delVertex(int v); // delete a vertex
	void initConns(std::vector<std::vector<int>>& conns);
};



#endif