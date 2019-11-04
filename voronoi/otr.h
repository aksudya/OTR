#pragma once
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/Optimal_transportation_reconstruction_2.h>

#include <CGAL/Polygon_2.h>
#include <CGAL/Periodic_2_triangulation_2.h>
#include <GLUT.h>
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
		return cost > a.cost;//��Сֵ���� 
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

	priority_queue<pri_queue_item> half_edge_queue;					//���ȶ���

	map<vertex_handle, vector<Point>> vertex_points_map;			//����䵽��
	map<Edge, vector<Point>> edge_points_map;					//����䵽����

	map<vertex_handle, vector<Point>> vertex_points_map_temp;			//����䵽��	 ��ʱ
	map<Edge, vector<Point>> edge_points_map_temp;					//����䵽����	��ʱ

	vector<vertex_pair> block_edge;				//

	OTR();


	void Init(vector<Point> input);

	void InitPriQueue();
	bool IsCollapsable(Edge &e);		//�жϱ��Ƿ���Ժϲ�
	void FlipEdge(Edge& e);

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