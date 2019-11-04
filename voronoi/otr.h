#pragma once
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/Optimal_transportation_reconstruction_2.h>
#include <GL/GLUT.h>
#include <iostream>
#include <cmath>
#include <random>
#include <ctime>

using namespace std;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2                                          Point;
typedef CGAL::Delaunay_triangulation_2<K>					Delaunay;
typedef Delaunay::Edge										Half_edge;
typedef Delaunay::Vertex_handle							vertex_handle;
typedef K::Segment_2 Segment;

class pri_queue_item
{
public:
	Half_edge half_edge;
	double cost;

	bool operator < (const pri_queue_item& a) const 
	{
		return cost > a.cost;//最小值优先 
	}
};

class OTR
{
public:
	vector<Point> points_input;
	Delaunay delaunay_input;

	Delaunay delaunay_temp;

	priority_queue<pri_queue_item> half_edge_queue;					//优先队列

	map<vertex_handle, vector<Point>> vertex_points_map;			//点分配到边
	map<Half_edge, vector<Point>> edge_points_map;					//点分配到顶点

	map<vertex_handle, vector<Point>> vertex_points_map_temp;			//点分配到边	 临时
	map<Half_edge, vector<Point>> edge_points_map_temp;					//点分配到顶点	临时


	OTR();


	void Init(vector<Point> input);

	void InitPriQueue();
	bool IsCollapsable(Half_edge e);		//判断边是否可以合并
};