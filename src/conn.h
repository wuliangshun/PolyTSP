#ifndef CONN_H
#define CONN_H

#include <vector>
#include <map>
#include <set>
#include <utility> // pair
#include <time.h> // time
#include <stdlib.h> // srand, rand
#include <math.h> // pow, sqrt
#include <stdio.h>
#include <numeric>   	//iota
#include <sstream>
#include <assert.h>
#include <iostream>
#include <algorithm>
#include <string.h>

using namespace std;


class CONN
{
public:
	
	static bool IsVertexInConns(int vertex, vector<vector<int>>& conns, vector<int>& out_conn); // judge a vertex belong to connections ?
	
	static void AddConnToPath(vector<int>& this_conn, vector<int>& path, bool is_head); // add a conn to a path
	
	static void UpdateConn(vector<vector<int>>& conns, vector<int>& old_conn, vector<int>& new_conn); // update conns
	
	static void PrintConns(vector<vector<int>>& conns); //print conns

	static void FlattenToFullPath(vector<int>& short_path,vector<vector<int>>& conns, vector<int>& full_path); //fatten a short path to full path
};

#endif