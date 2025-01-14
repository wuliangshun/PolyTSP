#include "conn.h"



// judge a vertex belong to connections ?
bool CONN::IsVertexInConns(int vertex, vector<vector<int>>& conns, vector<int>& out_conn)
{
	vector<vector<int>>::iterator it;
	for(it = conns.begin(); it !=  conns.end(); it++) //search all conn
	{
		vector<int>::iterator ret = std::find(it->begin(), it->end(), vertex);
		if (ret != it->end()) //find vertex
		{
			out_conn.assign(it->begin(), it->end());
			return true;
		}
	}
	return false;
}

// add a conn to a path
void CONN::AddConnToPath(vector<int>& this_conn, vector<int>& path, bool is_head)
{
	vector<int>::iterator it;
	if(is_head)
	{
		for(int i = 0; i <= this_conn.size() - 1; i++)
			path.push_back(this_conn[i]);
	}
	else
	{
		for(int i = this_conn.size() - 1; i >= 0; i--)
			path.push_back(this_conn[i]);
	}
}

// update conns
void CONN::UpdateConn(vector<vector<int>>& conns, vector<int>& old_conn, vector<int>& new_conn)
{
	vector<vector<int>>::iterator it;
	for (it = conns.begin(); it != conns.end(); ++it) {
        if (old_conn == *it) {
            it->assign(new_conn.begin(),new_conn.end());
        }
    }
}

//print conns
void CONN::PrintConns(vector<vector<int>>& conns)
{
	cout << "conns:" << conns.size() << endl;
	vector<vector<int>>::iterator it;
	for(it = conns.begin(); it != conns.end(); it++)
	{
		vector<int>::iterator iit;
		for(iit = it->begin(); iit != it->end(); iit++)
		{
			cout << *iit << ",";
		}
		cout<< "\n";
	}
}


//fatten a short path to full path
void CONN::FlattenToFullPath(vector<int>& short_path, vector<vector<int>>& conns, vector<int>& full_path)
{
	for(int i=0; i< short_path.size(); i++)
	{
		int v = short_path[i];
		
		vector<int> conn; 
		bool isIn = CONN::IsVertexInConns(v, conns, conn);
		if(isIn) // in conns
		{
			CONN::AddConnToPath(conn, full_path, conn[0] == v); //add conn
			if(i < short_path.size() - 1)
				i++;// shift step
		}
		else // not in conns
			full_path.push_back(v);
	}
}