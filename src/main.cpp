#include <iostream>
#include <sys/time.h> // timer
#include "graph.h"
#include "smart.h"
#include "log.h"
#include "bfs.h"
using namespace std;






int main()
{
	InitCLog("./Logs");
	srand(time(NULL)); // random numbers

	
	// creates random graph, parameter true is for generate the graph
	Graph * graph = new Graph(50, 0, false, (char*)"../data/eil76.tsp"); //"../data/berlin52.tsp" 
	graph->showInfoGraph();
	graph->showGraph();
	
	// BFS 
	//BFS * bfs = new BFS(graph);
	//bfs->bfs();
	
	// our solution
	time_t t_start, t_end;
  	t_start = time(NULL); 
	cout << "\n\n ############### Rough Screening. ##################\n" << endl;
	Smart * smart = new Smart(graph);
	/*
		MEMO: 
			provision stage:      find nearest neighbors
			stage 1 :             Shot Gun
			stage 2 :             Fuzzy Cat 
			
	    @paragrams
		
			stage_1_paths_num : stage 1 paths nums, default 300*N.
			stage_2_paths_num : stage 2 paths nums, default 200*N.
			stage_1_select_num: stage 1 select paths num, which are the shorest batch of paths. 
			stage_2_select_num: stage 2 select paths num, which are the shorest batch of paths. 
			decay_select_num  : in stage 2, selected paths num decay at N/10 per iteration, but no less than 100. 
								       i.e.  max( stage_2_select_num - t * decay_select_num, 100)
		 	max_iterations    : max iterations, default 150.
			tier1_num         : provision stage: first layer neighbors num
			tier2_num         : provision stage: second layer neighbors num
			tier3_num         : provision stage: third layer neighbors num
			stop_threshold    : stop condition.  default 0.0001
			stop_iters        : stop condition.  default 10.
								i.e.  (cost_i-stop_iters  -  cost_min) / cost_min < stop_threshold
			log type          : default NO, that means do not log every path
								
		GLOBAL VAR:			
			g_cost_type       : default INTEGER_MODE, that means the distance between two vertexs is integer (round).
	
	*/
	g_cost_type = INTEGER_MODE;
	int stage_1_paths_num = 300 * graph->V;	
	int stage_2_paths_num = 200 * graph->V;
	int stage_1_select_num = 3 * graph->V;
	int stage_2_select_num = 2 * graph->V;	
	int decay_select_num = graph->V / 10;
	int max_iterations = 150;
	int tier1_num = 5;
	int tier2_num = 15;
	int tier3_num = 1.0/3 * graph->V;
	double stop_threshold = 0.0001;
	int stop_iters = 10;
	smart->runStrategy(max_iterations, stop_iters, tier1_num, tier2_num, tier3_num, stop_threshold, 
						stage_1_paths_num, stage_2_paths_num, stage_1_select_num, stage_2_select_num, decay_select_num, 
					    NO); 
	
	t_end = time(NULL);
	cout << "\n\nTime : " << difftime(t_end,t_start) << " seconds." << endl; // shows time in seconds
	
	cout << "\n\n ############### Build Connections. ##################\n" << endl;
	int pair_num = 1.5 * graph->V;
	vector<vector<int>> conns;
	smart->buildConn(pair_num, conns);	
	graph->initConns(conns);
	t_end = time(NULL);
	cout << "\n\nTime : " << difftime(t_end,t_start) << " seconds." << endl; // shows time in seconds
	
	cout << "\n\n ############### Fine Screening. ##################\n" << endl;
	smart->runStrategy(max_iterations, stop_iters, tier1_num, tier2_num, tier3_num, stop_threshold, 
						stage_1_paths_num, stage_2_paths_num, stage_1_select_num, stage_2_select_num, decay_select_num, 
					    NO); 
	t_end = time(NULL);
	cout << "\n\nTime : " << difftime(t_end,t_start) << " seconds." << endl; // shows time in seconds	
	
	delete graph;
	delete smart;
	return 0;
}
