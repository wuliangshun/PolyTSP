#include "smart.h"
#include "log.h"
#include "conn.h"
#include <stdio.h>
#include <numeric>   	//iota
#include <sstream>
#include <assert.h>
#include <iostream>
#include <algorithm>
#include <string.h>
#include "thread.h"
using namespace std;


int g_cost_type = INTEGER_MODE;// global varible

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                           STL  tools
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/
// sort STL vector, return the indexs
template <typename T>
vector<size_t> sort_indexes_e(vector<T> &v)
{
	vector<size_t> idx(v.size());
	iota(idx.begin(), idx.end(), 0);
	sort(idx.begin(), idx.end(),
		[&v](size_t i1, size_t i2) {return v[i1] < v[i2]; });
	return idx;
}
// sort STL map
int cmp(const std::pair<std::pair<int,int>, double>& x, const std::pair<std::pair<int,int>, double>& y)
{
 	return x.second < y.second;
}
void sort_map(map<pair<int, int>, double>& map_t, vector<pair<pair<int, int>,double>>& vec_t)
{
	for (map<std::pair<int, int>, double>::iterator curr = map_t.begin(); curr != map_t.end(); curr++)
	{
	 	vec_t.push_back(std::make_pair(curr->first, curr->second));
	}

	sort(vec_t.begin(), vec_t.end(), cmp);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                            Constructor
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/
// Constructor
Smart::Smart(Graph* graph)
{
	this->graph = graph;
	this->coef = 0.5;
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                            Calculate Cost 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

// calculate the cost of a path
double Smart::CalcCost(vector<int>& short_path, int logType, char* logInfo)
{
	vector<int> path;
	if(path.size() != graph->V + 1) 
	{
		CONN::FlattenToFullPath(short_path, graph->conns, path);
	}
	else
		path.assign(short_path.begin(), short_path.end());
	stringstream ss;
	ss << logInfo << ":";
	ss << path[0] << "  ";
	double total_weight = 0;
	for(int i=0; i<path.size()-1; i++)
	{
		double weight;
		if(g_cost_type == INTEGER_MODE)
			weight = round(graph->existsEdge(path[i], path[i+1]));
		else
			weight = graph->existsEdge(path[i], path[i+1]);
		if(weight >= 0)
		{
			total_weight += weight;
			ss << path[i+1] << "  ";
		}
		else
			cout << "no edges between " << path[i] << " and " << path[i+1] << endl;
	}
	ss << " | Cost:" << total_weight << endl;
	if(logType == PRINT_AND_LOG || logType == PRINT)
		cout << ss.str() << endl;
	if(logType == PRINT_AND_LOG || logType == LOG)
		CLog(CLOG_INFO, (char*)ss.str().c_str());
	
	return total_weight;
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	    Find next vertexs alternatives
			    @FindRestVertexs:         find rest vertexs
				@FindRestNeighbors:       find tiers vertexs
			
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/
void Smart::FindRestVertexs(vector<int>& path, vector<int>& rest_vertexs)
{
	int curr_vertex = path[path.size() - 1];
	for(vector<int>::iterator it = graph->vertexs.begin(); it != graph->vertexs.end(); it++)
	{
		int v = *it;
		vector<int>::iterator fit = std::find(path.begin(), path.end(), v);
		if(fit == path.end()) //vertex not in path
			rest_vertexs.push_back(v);
	}
}

void Smart::FindRestNeighbors(vector<int>& path, vector<int>& rest_neighbors)
{
	int curr_vertex = path[path.size() - 1];
	// find tier0,tier1,tier2,tier3 vertexs
	vector<int> neighbors; 
	// tier0
	map<int, vector<int>>::iterator iit = this->map_tier0_vertexs.find(curr_vertex);
	if(iit != this->map_tier0_vertexs.end())
		neighbors.assign(iit->second.begin(), iit->second.end());
	else
	{
		// tier1
		iit = this->map_tier1_vertexs.find(curr_vertex);
		if(iit != this->map_tier1_vertexs.end())
			neighbors.assign(iit->second.begin(), iit->second.end());
		else
		{
			// tier2
			iit = this->map_tier2_vertexs.find(curr_vertex);
			if(iit != this->map_tier2_vertexs.end())
				neighbors.assign(iit->second.begin(), iit->second.end());
			else
			{
				// tier3
				iit = this->map_tier3_vertexs.find(curr_vertex);
				if(iit != this->map_tier3_vertexs.end())
					neighbors.assign(iit->second.begin(), iit->second.end());
			}
		}
	}
	
	// not in the path
	for(vector<int>::iterator it = neighbors.begin(); it != neighbors.end(); it++)
	{
		vector<int>::iterator f_it = std::find(path.begin(), path.end(), *it);
		if(f_it == path.end()) //vertex not in path
			rest_neighbors.push_back(*it);
	}
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   
	             --------      Provision Stage    -----------
	   
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/
void Smart::InitNeighbors(int tier1_num, int tier2_num, int tier3_num)
{
	for(vector<int>::iterator i_it = graph->vertexs.begin(); i_it != graph->vertexs.end(); i_it++)
	{
		int i = *i_it;
		int curr_vertex = i;
		
		// find zero-order vertexs :   head --> foot    foot --> head
 		if (graph->conns.size() > 0)
		{
			vector<vector<int>>::iterator it;
			for(it = graph->conns.begin(); it != graph->conns.end(); it++) //enumerate all conn
			{
				vector<int> conn = *it;
				
				vector<int> head;
				head.push_back(conn[0]);
				
				vector<int> foot;
				foot.push_back(conn[conn.size() - 1]);
				
				this->map_tier0_vertexs[conn[0]] = foot;
				this->map_tier0_vertexs[conn[conn.size() - 1]] = head;
			}
		}
		
		// find shorest neigbours
		map<pair<int, int>, double> map_weight;
		for(vector<int>::iterator j_it = graph->vertexs.begin(); j_it != graph->vertexs.end(); j_it++)
		{
			int j = *j_it;
			double weight = graph->existsEdge(curr_vertex, j);
			if(weight > 0 && i != j)
				map_weight[make_pair(curr_vertex, j)] = weight;
		}
		vector<pair<pair<int, int>, double>> vec_weight;
		sort_map(map_weight, vec_weight);
		
		vector<int> tier1_neighbors;
		vector<int> tier2_neighbors;
		vector<int> tier3_neighbors;
		for(int j = 0; j < tier1_num + tier2_num + tier3_num; j++)
		{
			int neighbor = vec_weight[j].first.second;
			//cout << i << ",   " << neighbor << ",   " << vec_weight[j].second << endl << endl;
			if (j < tier1_num)
				tier1_neighbors.push_back(neighbor);
			else if (j < tier2_num)
				tier2_neighbors.push_back(neighbor);
			else
				tier3_neighbors.push_back(neighbor);
		}
		this->map_tier1_vertexs[curr_vertex] = tier1_neighbors;
		this->map_tier2_vertexs[curr_vertex] = tier2_neighbors;
		this->map_tier3_vertexs[curr_vertex] = tier3_neighbors;
	}
}






/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   
	             Prob :
				 		@GetDistProb:            dist prob
						@GetTimesProb:           times prob
						@GetWeightedTimesProb    weighted times prob
	   
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/
void Smart::GetDistProb(int curr_vertex, vector<int>& rest_vertexs, map<std::pair<int, int>, double>& map_dist_prob)
{
	/*
	@paras:
		  curr_vertex:	    current vertex
		  rest_vertexs:     list of rest vertexs
		  map_dist_prob:    <int,int>: first: fixed to curr_vertex
		  					           second: for v in rest_vertexs
						    double:    prob			
	*/
	//calc C
	double total_weight = 0;
	for(vector<int>::iterator it = rest_vertexs.begin(); it != rest_vertexs.end(); it++)
	{
		double weight = graph->existsEdge(curr_vertex, *it);
		if(weight >= 0)
			total_weight += weight;	
	}
	double avg_weight = total_weight/rest_vertexs.size();	
	avg_weight = this->coef * avg_weight;
	double sum = 0;
	for(vector<int>::iterator it = rest_vertexs.begin(); it != rest_vertexs.end(); it++)
	{
		double weight = graph->existsEdge(curr_vertex, *it);
		if(weight >= 0)
			sum += 1.0/(1 + (weight / avg_weight)*(weight / avg_weight));
	}
	if(sum == 0)
		return;
	double C = 1.0/sum;
	//prob_i
	for(vector<int>::iterator it = rest_vertexs.begin(); it != rest_vertexs.end(); it++)
	{
		double weight = graph->existsEdge(curr_vertex, *it);
		if(weight > 0)
		{
			double prob_i = C/(1 + (weight/avg_weight)*(weight/avg_weight));
			map_dist_prob[make_pair(curr_vertex, *it)] = prob_i;
		}			
	}
}
void Smart::GetTimesProb(int curr_vertex, vector<int>& rest_vertexs, map<std::pair<int,int>,double>& map_times_prob)
{
	
	// get  total times of rest vertexs
	double total_times = 0;
	for(int i=0; i<rest_vertexs.size(); i++)
	{
		map<pair<int, int>,int>::iterator iit;
		iit = this->map_times.find(make_pair(curr_vertex, rest_vertexs[i]));
		if(iit != this->map_times.end())
			total_times += iit->second;
		iit = this->map_times.find(make_pair(rest_vertexs[i], curr_vertex));
		if(iit != this->map_times.end())
			total_times += iit->second;
	}
	// enumerate rest vertexs
	for(vector<int>::iterator rit = rest_vertexs.begin(); rit != rest_vertexs.end(); rit++)
	{
		double prob = 0;
		map<pair<int, int>,int>::iterator iit;
		iit = this->map_times.find(make_pair(curr_vertex, *rit));
		if(iit != this->map_times.end())
			prob += iit->second/total_times;
		iit = this->map_times.find(make_pair(*rit, curr_vertex));
		if(iit != this->map_times.end())
			prob += iit->second/total_times;
		map_times_prob[make_pair(curr_vertex, *rit)] = prob;
	}
}


void Smart::GetWeightedTimesProb(map<std::pair<int,int>,double>& map_dist_prob, map<std::pair<int,int>,double>& map_times_prob, map<std::pair<int,int>,double>& map_weighted_times_prob)
{
	//compute sum
	double sum = 0;
	for(map<pair<int, int>, double>::iterator dist_it = map_dist_prob.begin(); dist_it != map_dist_prob.end(); ++dist_it)
	{
		map<pair<int, int>, double>::iterator times_it = map_times_prob.find(make_pair(dist_it->first.first, dist_it->first.second));
		if(times_it != map_times_prob.end())		
			sum += (dist_it->second) * (times_it->second);
		// no pair between current vertex and this vertex, then let it be a very small prob
		else
			sum += (dist_it->second) * (1.0 / map_dist_prob.size());
	}
	if (sum == 0)
	{
		map_weighted_times_prob = map_dist_prob;
		//cout << "no pair occurs for rest vertexs. " << map_weighted_times_prob.size() << endl;
		return;
	}
		
	//compute prob
	for(map<pair<int, int>, double>::iterator dist_it = map_dist_prob.begin(); dist_it != map_dist_prob.end(); ++dist_it)
	{
		map<pair<int, int>, double>::iterator times_it = map_times_prob.find(make_pair(dist_it->first.first, dist_it->first.second));
		if(times_it != map_times_prob.end())				
			map_weighted_times_prob[make_pair(dist_it->first.first, dist_it->first.second)] = (dist_it->second) * (times_it->second) / sum;
		else
			map_weighted_times_prob[make_pair(dist_it->first.first, dist_it->first.second)] = (dist_it->second) * (1.0 /  map_dist_prob.size()) / sum;
	}																	
}


int Smart::RandomSelectNextVertex(map<std::pair<int, int>, double>& map_prob)
{
	/*
	@paras:
		  map_prob:    <int,int>: first: fixed to curr_vertex
		  					      second: for v in rest_vertexs
					   double:    prob			
	@return:
		  next vertex
	*/
	//check sum_prob==1 or not?
	//double sum_prob = 0;
	//for(map<pair<int, int>, double>::iterator it = map_prob.begin(); it != map_prob.end(); ++it)
	//	sum_prob += it->second; 
	//cout << "sum_prob:" << sum_prob << endl;
	//if (sum_prob != 1)
		//return -1;
	//rand
	double sum_prob = 0;
	double r = rand()%100/(double)101; //0~1 random number
	for(map<pair<int, int>, double>::iterator it = map_prob.begin(); it != map_prob.end(); ++it)
	{
		sum_prob += it->second;
		if(r <= sum_prob)
			return it->first.second;
	}
	return -1;
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   
	          --  sort paths
			  --  select paths
			  --  count pairs
					   
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/
void Smart::SortAndSelectPaths(int select_num, vector<vector<int>>& paths, vector<double>& costs, vector<vector<int>>& select_paths)
{
	/*
		Params:
			Input : select_num
			        paths
					costs
			Output: 
	*/
	
	// sort all paths
	vector<size_t> idx = sort_indexes_e(costs);
	if (idx.size() == 0) return;
	for(int i=0; i < costs.size(); i++)
	{
		//remove repeated paths
		if (i >=  1)
		{
			double last_cost =  costs[idx[i-1]];
			double this_cost =  costs[idx[i]];
			if( abs(this_cost - last_cost) < PRECISE )
				continue; 
		}
		//add path
		vector<int> curr_path = paths[idx[i]];
		select_paths.push_back(curr_path);
		if(select_paths.size() >= select_num)
			break;
	}
}

void Smart::CountPairs(int t, vector<vector<int>>& paths)
{
	/*
		@Paras:
		       t:                           iterations
			   map_times:                   Key:
			   			                        <int,int> : path[i],path[j] and path[j],path[i]
			                                Value:
											     int: pair occurance times
	*/
	
	//update pairs
	for(vector<vector<int>>::iterator it = paths.begin(); it != paths.end(); ++it)
	{
		vector<int> curr_path = *it;
		
		//log paths
		char info[50] = {0}; sprintf(info, "Shot Gun & Fuzzy Concatenation +%d", t+1);
		double cost = this->CalcCost(curr_path, LOG, info);
		
		for(int k=0; k < curr_path.size()-1; k++)
		{
			map<pair<int, int>,int>::iterator it;
			// i --> j			
			it = this->map_times.find(make_pair(curr_path[k], curr_path[k+1]));
			if(it != this->map_times.end())
				it->second = it->second + 1;
			else
				this->map_times[make_pair(curr_path[k], curr_path[k+1])] = 1; 
			// j --> i
			it = this->map_times.find(make_pair(curr_path[k+1], curr_path[k]));
			if(it != this->map_times.end())
				it->second = it->second + 1;
			else
				this->map_times[make_pair(curr_path[k+1], curr_path[k])] = 1; 
		}
	}
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   
	                             ----   Strategies ----
					   
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/
/* &&&&&&&&&&&&&&&&&&&&&&&&  Shot Gun  &&&&&&&&&&&&&&&&&&&&&  */
void Smart::ShotGun(vector<int>& path)
{
	int initial_vertex = graph->vertexs[rand() % graph->vertexs.size()];
	int curr_vertex = initial_vertex;
	path.push_back(curr_vertex);
	while(true)
	{	
		//find tier1, tier2 vertexs
		vector<int> rest_vertexs;
		this->FindRestNeighbors(path, rest_vertexs);
		if(rest_vertexs.size() == 0)
			this->FindRestVertexs(path, rest_vertexs);
	
		//last vertex
		if(path.size() == graph->vertexs.size())
		{
			path.push_back(initial_vertex); // return back to inital vertex
			break;
		}
		//determine next vertex
		int next_vertex = -1;
		if(rest_vertexs.size() == 1)
			next_vertex = rest_vertexs[0];
		else
		{
			//compute distance probabilities
			map<std::pair<int, int>, double> map_dist_prob;
			this->GetDistProb(curr_vertex, rest_vertexs, map_dist_prob);
			if(map_dist_prob.size() == 0) break;
			next_vertex = this->RandomSelectNextVertex(map_dist_prob);
		}	
		// add to path and update
		if(next_vertex != -1)
		{
			path.push_back(next_vertex);
			curr_vertex = next_vertex;
		}
	}
}
/* &&&&&&&&&&&&&&&&&&&&&&&&  Fuzzy Cat  &&&&&&&&&&&&&&&&&&&&&  */
void Smart::FuzzyConcat(vector<int>& path)
{
	int initial_vertex = graph->vertexs[rand() % graph->vertexs.size()];
	int curr_vertex = initial_vertex;
	path.push_back(curr_vertex);
	while(true)
	{
		//find tier1, tier2 vertexs
		vector<int> rest_vertexs;
		this->FindRestNeighbors(path, rest_vertexs);
		if(rest_vertexs.size() == 0)
			this->FindRestVertexs(path, rest_vertexs);
		
		//last vertex
		if(path.size() == graph->vertexs.size())
		{
			path.push_back(initial_vertex); // return back to inital vertex
			break;
		}
		
		//determine next vertex
		int next_vertex = -1;
		if(rest_vertexs.size() == 1)
			next_vertex = rest_vertexs[0];
		else
		{
			//compute times probabilities
			//dist
			map<std::pair<int,int>,double> map_dist_prob;
			this->GetDistProb(curr_vertex, rest_vertexs, map_dist_prob);
			if(map_dist_prob.size() == 0) break;
			//times
			map<std::pair<int,int>,double> map_times_prob;
			this->GetTimesProb(curr_vertex, rest_vertexs, map_times_prob);
			if(map_times_prob.size() == 0) break;
			//weighted times
			map<std::pair<int,int>,double> map_weighted_times_prob;
			this->GetWeightedTimesProb(map_dist_prob, map_times_prob, map_weighted_times_prob);
			if(map_weighted_times_prob.size() == 0) break;
			//determine next vertex
			next_vertex = this->RandomSelectNextVertex(map_weighted_times_prob);
		}	
		// add to path and update
		if(next_vertex != -1)
		{
			path.push_back(next_vertex);
			curr_vertex = next_vertex;
		}
	}	
}




/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	TEST
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/
void* threadShotGun(void* args)
{
	Smart* smartPtr = (Smart*) args;
	vector<int> *pathPtr = new vector<int>{};
	smartPtr->ShotGun(*pathPtr);
	pthread_exit(pathPtr);
}

void* threadFuzzyCat(void* args)
{
	Smart* smartPtr = (Smart*) args;
	vector<int> *pathPtr = new vector<int>{};
	smartPtr->FuzzyConcat(*pathPtr);
	pthread_exit(pathPtr);
}



void Smart::runStrategy(int max_iterations, int stop_iters, int tier1_num, int tier2_num, int tier3_num, 
						double stop_threshold, int stage_1_paths_num, int stage_2_paths_num, int stage_1_select_num,
						int stage_2_select_num, int decay_select_num, int log_type)
{
	/*
	@Paras:
		log_type:              NO, PRINT, LOG, PRINT_AND_LOG 
	*/
	
	InitNeighbors(tier1_num, tier2_num, tier3_num);
	
	vector<vector<int>> paths;
	vector<double> costs;
	double shortest_cost = 1000000000000;
	/*
	================================
	Stage 1: Shot Gun
	===============================
	*/	
	for(int i=0; i < stage_1_paths_num / NUM_THREADS; i++)
	{
		vector<void*> p_vec;
		thread_proc_ret(threadShotGun, this, p_vec);		
		vector<void*>::iterator it;
		for(it = p_vec.begin(); it != p_vec.end(); it++)
		{
			void* retPathPtr = *it;
			vector<int> path;
			path.assign((*(vector<int>*)retPathPtr).begin(), (*(vector<int>*)retPathPtr).end());
			delete retPathPtr;	retPathPtr = NULL;
			
			double cost = this->CalcCost(path, log_type, "Shot Gun");
			if(cost != -1) {
				paths.push_back(path);
				costs.push_back(cost);
			}
		}
	}
	
	//select paths
	vector<vector<int>> select_paths;
	this->SortAndSelectPaths(stage_1_select_num, paths, costs, select_paths);
	this->last_iter_paths.clear();
	this->last_iter_paths.assign(this->curr_iter_paths.begin(), this->curr_iter_paths.end());
	this->curr_iter_paths.clear();
	this->curr_iter_paths.assign(select_paths.begin(), select_paths.end());

	//update map_times
	this->map_times.clear();	
	this->CountPairs(-1, this->curr_iter_paths);
	
	/*
	=================================
	Stage 2: Fuzzy Concatenate
	=================================
	*/
	vector<double> recent_costs;
	for(int t=0; t < max_iterations; t++)
	{	
		// determine coefficient
		this->coef =  0.5 - 0.01 * t > 0.15 ? 0.5 - 0.01 * t : 0.15;
		
		// get $path1Num$ paths
		paths.clear();
		costs.clear();		
		int this_select_num = stage_2_select_num - decay_select_num * t > 100 ? stage_2_select_num - decay_select_num * t: 100;
		int this_paths_num = this_select_num * 100; 
		for(int i=0; i < this_paths_num  / NUM_THREADS; i++)
		{
			vector<void*> p_vec;
			thread_proc_ret(threadFuzzyCat, this, p_vec);	
			vector<void*>::iterator it;
			for(it = p_vec.begin(); it != p_vec.end(); it++)
			{
				void* retPathPtr = *it;
				vector<int> path;
				path.assign((*(vector<int>*)retPathPtr).begin(), (*(vector<int>*)retPathPtr).end());
				delete retPathPtr;	retPathPtr = NULL;

				char info[50] = {0}; sprintf(info, "Shot Gun & Fuzzy Concatenation +%d", t+1);
				double cost = this->CalcCost(path, log_type, info);
				if(cost != -1) {
					paths.push_back(path);
					costs.push_back(cost);
				}
			}
		}
		
		//select paths
		vector<vector<int>> select_paths;
		this->SortAndSelectPaths(this_select_num, paths, costs, select_paths);
		this->last_iter_paths.clear();
		this->last_iter_paths.assign(this->curr_iter_paths.begin(), this->curr_iter_paths.end());
		this->curr_iter_paths.clear();
		this->curr_iter_paths.assign(select_paths.begin(), select_paths.end());
		
		//merge paths
		vector<vector<int>> merge_iter_paths;
		merge_iter_paths.insert(merge_iter_paths.end(),this->last_iter_paths.begin(),this->last_iter_paths.end());
		merge_iter_paths.insert(merge_iter_paths.end(),this->curr_iter_paths.begin(),this->curr_iter_paths.end());
		
		//update map_times
		this->map_times.clear();	
		this->CountPairs(t, merge_iter_paths);
		
		// print iteration result
		if(curr_iter_paths.size() == 0)
			continue;
		double this_cost = this->CalcCost(curr_iter_paths[0], log_type, "Shortest path");
		if(this_cost < shortest_cost)
			shortest_cost = this_cost;
		printf("%d-th iteration, shortest cost: %.2f,  this iteration shorest cost: %.2f\n", t, shortest_cost, this_cost);
		
		// record recent shortest_cost
		recent_costs.push_back(shortest_cost);
		int size = recent_costs.size();
		
		// For consecutive 5 iterations, optimaliaztion less than $stop_threshold$, stop
		if(size > stop_iters)
		{	
			double before_cost = recent_costs[size-1-stop_iters];
			if( abs(shortest_cost - before_cost) < before_cost * stop_threshold)
			{
				cout << "compare to last " << stop_iters << " iterations,optimaliaztion less than "<< stop_threshold <<", stop." << endl;
				break;
			}
		}
	}
}

void Smart::buildConn(int pair_num, vector<vector<int>>& conns)
{	
	//get current iter costs
	vector<double> curr_iter_costs;
	for(int i=0; i < this->curr_iter_paths.size(); i++)
	{
		vector<int> path;
		path.assign(this->curr_iter_paths[i].begin(), this->curr_iter_paths[i].end());
		double cost = this->CalcCost(path, NO, "Build Conn");
		curr_iter_costs.push_back(cost);
	}
	
	//sort and select paths
	vector<vector<int>> select_paths;
	SortAndSelectPaths(this->curr_iter_paths.size(), this->curr_iter_paths, curr_iter_costs, select_paths);
		
	//update map_times
	this->map_times.clear();	
	int path_num = 1;
	for(int i=0; i < select_paths.size(); i++)
	{
		vector<vector<int>> temp_paths;
		temp_paths.push_back(select_paths[i]);
		
		this->CountPairs(1000000, temp_paths);
		if(this->map_times.size() >= pair_num*2)
		{
			path_num = i + 1;
			break;
		}
	}
	cout << "debug: pairs:" << this->map_times.size()  << ", select paths" << path_num << endl;
	
	//filter pairs
	map<pair<int, int>, int>::iterator it;
	for(it = this->map_times.begin(); it != this->map_times.end(); ++it)
	{
		if(it->second != path_num) //occured not in every path
			this->map_times.erase(it);
	}
	cout << "debug: remained pairs:" << this->map_times.size() << endl;
	
	//form connections
	for(it = this->map_times.begin(); it != this->map_times.end(); ++it)
	{
		vector<int> conn1;
		bool isHeadIn = CONN::IsVertexInConns(it->first.first, conns, conn1);
		vector<int> conn2;
		bool isFootIn = CONN::IsVertexInConns(it->first.second, conns, conn2);
		if(isHeadIn && !isFootIn)
		{
			vector<int> temp_conn;
			temp_conn.assign(conn1.begin(), conn1.end());
			//add foot
			if(it->first.first == temp_conn[temp_conn.size() - 1]) // [ conn --> head ] --> foot
				temp_conn.push_back(it->first.second);
			else if(it->first.first == temp_conn[0]) //   foot --> [ head --> conn ]
				temp_conn.insert(temp_conn.begin(), it->first.second);
			
			CONN::UpdateConn(conns, conn1, temp_conn);
		}
		else if(!isHeadIn && isFootIn)
		{
			vector<int> temp_conn;
			temp_conn.assign(conn2.begin(), conn2.end());
			//add head
			if(it->first.second == temp_conn[temp_conn.size() - 1]) // [ conn --> foot ] --> head
				temp_conn.push_back(it->first.first);
			else if(it->first.second == temp_conn[0]) //   head --> [ foot --> conn ] 
				temp_conn.insert(temp_conn.begin(), it->first.first);
			
			CONN::UpdateConn(conns, conn1, temp_conn);
		}
		else if(!isHeadIn && !isFootIn) //not in, add pair
		{
			vector<int> conn;
			conn.push_back(it->first.first);
			conn.push_back(it->first.second);
			conns.push_back(conn);
		}
	}
	
	//print conns
	CONN::PrintConns(conns);
}
