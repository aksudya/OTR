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
typedef  Delaunay::Edge										Half_edge;
typedef  Delaunay::Vertex_handle							vertex_handle;

class pri_queue_item
{
public:
	Half_edge half_edge;
	double cost;

	bool operator < (const pri_queue_item& a) const 
	{
		return cost > a.cost;//��Сֵ���� 
		
	}
};

class OTR
{
public:
	vector<Point> points_input;
	Delaunay delaunay_input;

	priority_queue<pri_queue_item> half_edge_queue;

	map<vertex_handle, vector<Point>> vertex_points_map;
	map<Half_edge, vector<Point>> edge_points_map;



	OTR();


	void Init(vector<Point> input);



};