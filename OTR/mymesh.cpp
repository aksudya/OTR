#include "mymesh.h"



mymesh::mymesh()
{
}


mymesh::~mymesh()
{

}

Point mymesh::FindNearestVertex(Point& p)
{

	Tree tree(Vertexs.begin(), Vertexs.end());

	//Point aa = p;
	Neighbor_search search(tree, p, 1);
	Point re;
	for (Neighbor_search::iterator it = search.begin(); it != search.end(); ++it)
	{
		re = it->first;
	}
	
	return re;
}

Segment mymesh::FindNearestEdge(Point& p)
{
	double min = DBL_MAX;
	Segment re;
	//vector<set<Segment, Segment_more>::iterator> eitstore;


	//for (auto eit = edges.begin(); eit != edges.end(); ++eit)
	//{
	//	eitstore.push_back(eit);
	//}

	//omp_lock_t lock;
	//omp_init_lock(&lock);
//#pragma omp parallel for
	for (auto eit = edges.begin(); eit != edges.end(); ++eit)
	{
		//auto eit = eitstore[i];
		Segment s = *eit;
		double dis = squared_distance(p,s);

		if(dis<min)
		{
			//omp_set_l ock(&lock);
			min = dis;
			re = s;
			//omp_unset_lock(&lock);
		}
	}
	//omp_destroy_lock(&lock);
	return re;
}

int mymesh::MakeCollaps(const Point& s, const Point& t)
{
	vector<Point> stars;
	vector<Point> starp;
	Vertexs.erase(s);
	for (auto eit=edges.begin();eit!=edges.end();++eit)
	{
		if(eit->source()==s)
		{
			if(eit->target()!=t)
			{
				stars.push_back(eit->target());
			}
		}
		if (eit->target() == s)
		{
			if (eit->source() != t)
			{
				stars.push_back(eit->source());
			}
		}
	}

	for (auto eit = edges.begin(); eit != edges.end(); ++eit)
	{
		if (eit->source() == t)
		{
			if (eit->target() != s)
			{
				starp.push_back(eit->target());
			}
		}
		if (eit->target() == t)
		{
			if (eit->source() != s)
			{
				starp.push_back(eit->source());
			}
		}
	}

	for (auto sit=stars.begin();sit!=stars.end();++sit)
	{
		Segment ss1(*sit, s);
		Segment ss2(s, *sit);

		
		edges.erase(ss1);
		edges.erase(ss2);


		Segment si1(*sit, t);
		Segment si2(t, *sit);
		if (edges.find(si1) == edges.end() && edges.find(si2) == edges.end())
		{
			edges.insert(si1);
		}
	}
	Segment ss1(s, t);
	Segment ss2(t, s);

	edges.erase(ss1);
	edges.erase(ss2);

	if(stars.empty()&&starp.empty())
	{
		Vertexs.erase(t);
		return 1;
	}
	else
	{
		return 0;
	}
}

void mymesh::Clear()
{
	Vertexs.clear();
	edges.clear();
	//IsoVertexs.clear();
}
