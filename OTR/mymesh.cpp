#include "mymesh.h"



mymesh::mymesh()
{
}


mymesh::~mymesh()
{

}

Point mymesh::FindNearestVertex(Point& p)
{
	CGAL::Point_set_2<K> PSet;
	PSet.insert(Vertexs.begin(), Vertexs.end());
	Point re = PSet.nearest_neighbor(p)->point();
	return re;
}

Segment mymesh::FindNearestEdge(Point& p)
{
	double min = DBL_MAX;
	Segment re;
	for (auto eit=edges.begin();eit!=edges.end();++eit)
	{
		Segment s = *eit;
		double dis = squared_distance(p,s);
		if(dis<min)
		{
			min = dis;
			re = s;
		}
	}
	return re;
}

void mymesh::MakeCollaps(Point& s, Point& t)
{
	vector<Point> stars;
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
}
