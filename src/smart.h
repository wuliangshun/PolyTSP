#ifndef SMART_H
#define SMART_H

#include <vector>
#include <map>
#include <set>
#include <utility> // pair
#include <time.h> // time
#include <stdlib.h> // srand, rand
#include <math.h> // pow, sqrt
#include "graph.h"
using namespace std;

#define NO             0
#define PRINT          1
#define PRINT_AND_LOG  2
#define LOG            3

#define DOUBLE_MODE    1
#define INTEGER_MODE   0

#define PRECISE 0.000001

extern int g_cost_type; // global varible

class Smart
{
private:
	Graph* graph; // the graph
	map<std::pair<int,int>, int>  map_times; //k:pairs, v:count
	vector<vector<int>> last_iter_paths; //last iteration select paths
	vector<vector<int>> curr_iter_paths; //this iteration select paths
	double coef;// dist coefficient
	map<int, vector<int>> map_tier0_vertexs; //tter0 vertexs
	map<int, vector<int>> map_tier1_vertexs; //tier1 neighbors
	map<int, vector<int>> map_tier2_vertexs; //tier2 neighbors
	map<int, vector<int>> map_tier3_vertexs; //tier3 neighbors
	
	
	// tool functions
	void InitNeighbors(int tier1_num, int tier2_num, int tier3_num);
	void FindRestNeighbors(vector<int>& path, vector<int>& rest_neighbors);
	void FindRestVertexs(vector<int>& path, vector<int>& rest_vertexs);
	void GetDistProb(int curr_vertex, vector<int>& rest_vertexs, map<std::pair<int, int>, double>& map_dist_prob);
	void GetTimesProb(int curr_vertex, vector<int>& rest_vertexs, map<std::pair<int,int>, double>& map_times_prob);
	void GetWeightedTimesProb(map<std::pair<int,int>,double>& map_dist_prob, map<std::pair<int,int>,double>& map_times_prob, map<std::pair<int,int>,double>& map_weighted_times_prob);
	int RandomSelectNextVertex(map<std::pair<int, int>, double>& map_prob);
	void SortAndSelectPaths(int select_num, vector<vector<int>>& paths, vector<double>& costs, vector<vector<int>>& select_paths);
	void CountPairs(int t, vector<vector<int>>& paths);
	
	
public:
	Smart(Graph *graph);// Constructor
	double CalcCost(vector<int>& path, int logType, char* logInfo);
	
	
	
	/*Stragies:
		stage 1: shot gun stage
		stage 2: fuzzy concatenation stage
	*/
	void ShotGun(vector<int>& path);
	void FuzzyConcat(vector<int>& final_path);
	
	/*
	Test
	*/
	void runStrategy(int max_iterations, int stop_iters, int tier1_num, int tier2_num, int tier3_num, 
						double stop_threshold, int stage_1_paths_num, int stage_2_paths_num, int stage_1_select_num, int stage_2_select_num, 
						int decay_select_num, int log_type);
	void buildConn(int pair_num, vector<vector<int>>& conns);
};
#endif