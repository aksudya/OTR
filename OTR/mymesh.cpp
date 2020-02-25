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
//	double min = DBL_MAX;
//	Segment re;
//	//vector<set<Segment, Segment_more>::iterator> eitstore;
//
//
//	//for (auto eit = edges.begin(); eit != edges.end(); ++eit)
//	//{
//	//	eitstore.push_back(eit);
//	//}
//
//	//omp_lock_t lock;
//	//omp_init_lock(&lock);
////#pragma omp parallel for
//	for (auto eit = edges.begin(); eit != edges.end(); ++eit)
//	{
//		//auto eit = eitstore[i];
//		Segment s = *eit;
//		double dis = squared_distance(p,s);
//
//		if(dis<min)
//		{
//			//omp_set_l ock(&lock);
//			min = dis;
//			re = s;
//			//omp_unset_lock(&lock);
//		}
//	}
	//omp_destroy_lock(&lock);

	Neighbor_search search(sampe_tree, p, 1);
	Point re;
	for (Neighbor_search::iterator it = search.begin(); it != search.end(); ++it)
	{
		re = it->first;
	}

	Segment res = sample_map.find(re)->second;

	return res;
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

void mymesh::BuildSampleKDtree()
{
	sample_map.clear();
	sampe_tree.clear();
	int n = 5;
	//set<Point> sample_ponts;

	for (auto eit = edges.begin(); eit != edges.end(); ++eit)
	{
		Segment e = *eit;
		int a = 0;
		int b = n;
		for (int i = 0; i < n - 1; ++i)
		{
			a++;
			b--;
			double xx = (b * e.source().x() + a * e.target().x()) / (double)n;
			double yy = (b * e.source().y() + a * e.target().y()) / (double)n;
			double zz = (b * e.source().z() + a * e.target().z()) / (double)n;
			Point pp(xx, yy,zz);
			sampe_tree.insert(pp);
			//sample_ponts.insert(pp);
			sample_map.insert(pair<Point, Segment>(pp, e));
		}
		//sampe_tree.insert(e.source());
		//sampe_tree.insert(e.target());
		//sample_map.insert(pair<Point, Segment>(e.source(), e));
		//sample_map.insert(pair<Point, Segment>(e.target(), e));
	}
	//sampe_tree.insert(sample_ponts.begin(), sample_ponts.end());
	
}

void mymesh::Clear()
{
	Vertexs.clear();
	edges.clear();
	//IsoVertexs.clear();
}
