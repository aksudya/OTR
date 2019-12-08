#pragma once
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/Optimal_transportation_reconstruction_2.h>
#include <CGAL/Point_set_2.h>
#include <list>
#include <CGAL/Polygon_2.h>
#include <CGAL/Periodic_2_triangulation_2.h>
#include <GLUT.h>
#include <iostream>
#include <cmath>
#include <random>
#include <ctime>
#include <fstream> 

using namespace std;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef K::Point_2                                          Point;
typedef CGAL::Delaunay_triangulation_2<K>					Delaunay;
typedef CGAL::Triangulation_2<K>							Triangulation;
typedef Triangulation::Edge									Edge;
typedef Triangulation::Face									Face;
typedef Triangulation::Face_handle							Face_handle;
typedef Triangulation::Vertex_handle						vertex_handle;
typedef K::Segment_2										Segment;
typedef CGAL::Polygon_2<K>									Polygon_2;
typedef K::Line_2											Line;
typedef pair<vertex_handle, vertex_handle>					vertex_pair;

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

