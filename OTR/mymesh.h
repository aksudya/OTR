#pragma once
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/point_generators_2.h>
//#include <CGAL/Optimal_transportation_reconstruction_2.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <queue>
#include <CGAL/Point_set_3.h>
#include <list>
#include <CGAL/Polygon_2.h>
#include <CGAL/Periodic_3_triangulation_3.h>
#include <CGAL/point_generators_3.h>
#include <gl/GLUT.h>
#include <iostream>
#include <cmath>
#include <random>
#include <ctime>
#include <fstream> 
#include <CGAL/Simple_cartesian.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits_3.h>
#include <list>
#include <cmath>
//#define METHED2

using namespace std;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef K::Point_3	                                        Point;
typedef CGAL::Delaunay_triangulation_3<K>					Delaunay;
typedef CGAL::Triangulation_3<K>							Triangulation;
typedef Triangulation::Edge									Edge;

typedef Triangulation::Vertex_handle						vertex_handle;
typedef K::Segment_3										Segment;
typedef CGAL::Polygon_2<K>									Polygon_2;
typedef K::Line_3											Line;
typedef pair<vertex_handle, vertex_handle>					vertex_pair;

typedef CGAL::Search_traits_3<K> TreeTraits;
typedef CGAL::Orthogonal_k_neighbor_search<TreeTraits> Neighbor_search;
typedef Neighbor_search::Tree Tree;


struct Segment_more {
	bool operator()(const Segment& s1, const Segment& s2) const
	{
		if(s1.source()==s2.source())
		{
			return s1.target() < s2.target();
		}
		else
		{
			return s1.source() < s2.source();
		}
		
	}
};

class mymesh
{
public:
	set<Point> Vertexs;
	set<Segment, Segment_more> edges;
	//set<Point> IsoVertexs;

	mymesh();
	~mymesh();

	Point FindNearestVertex(Point &p);
	Segment FindNearestEdge(Point& p);
	int MakeCollaps(const Point& s, const Point& t);
	void Clear();

	mymesh& operator = (const mymesh& t) 
	{
		Vertexs = t.Vertexs;
		edges = t.edges;
		//IsoVertexs = t.IsoVertexs;
		return *this;
	}
};

