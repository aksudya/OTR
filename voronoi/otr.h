#pragma once
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/Optimal_transportation_reconstruction_2.h>

#include <CGAL/Polygon_2.h>
#include <CGAL/Periodic_2_triangulation_2.h>
#include <gl/GLUT.h>
#include <iostream>
#include <cmath>
#include <random>
#include <ctime>

using namespace std;



typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Optimal_transportation_reconstruction_2<K>  OPT;

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

class pri_queue_item
{
public:
	Edge half_edge;
	double cost;

	bool operator < (const pri_queue_item& a) const 
	{
		return cost > a.cost;	//最大值
	}
};

class OTR
{
public:
	vector<Point> points_input;
	Delaunay delaunay_input;

	Triangulation tgl1;
	Triangulation tgl2;

	//Delaunay delaunay_temp;

	priority_queue<pri_queue_item> half_edge_queue;					//代价函数的优先队列

	map<vertex_handle, vector<Point>> vertex_points_map;			//点到顶点的分配
	map<Edge, vector<Point>> edge_points_map;					//点到边的分配

	map<vertex_handle, vector<Point>> vertex_points_map_temp;			//
	map<Edge, vector<Point>> edge_points_map_temp;					//

	vector<vertex_pair> block_edge;				//

	vector<Point> assin_points;					//待分配的顶点

	OTR();

	bool isborder;

	void Init(vector<Point> input);

	void InitPriQueue();
	bool IsCollapsable(Edge &e);		//
	void FlipEdge(Edge& e);
	void MakeCollap(Edge& e);	//合并边，合并后的结果在tgl2中

	double CaculateAssinCost();		//计算当前分配方案总代价

	vertex_handle source_vertex(Edge& edge)
	{
		return edge.first->vertex(tgl2.ccw(edge.second));
	}

	vertex_handle target_vertex(Edge& edge)
	{
		return edge.first->vertex(tgl2.cw(edge.second));
	}

	Edge prev_edge(Edge& edge) 
	{
		Face_handle f = edge.first;
		int index = tgl2.cw(edge.second);
		return Edge(f, index);
	}

	Edge twin_edge(Edge& edge)
	{
		Face_handle f = edge.first;
		vertex_handle v = source_vertex(edge);
		Face_handle nf = f->neighbor(edge.second);
		return Edge(nf, tgl2.ccw(nf->index(v)));
	}

	vertex_handle opposite_vertex(Edge& edge)
	{
		return edge.first->vertex(edge.second);
	}

	bool is_flippable(Edge& edge);
};