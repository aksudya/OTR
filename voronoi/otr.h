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
#include <fstream> 

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

class _Cost
{
public:
	vector<Point> assined_points;
	double to_edge_cost;
	double to_vertex_cost;
	double normal_cost;
	double tangential_cost;

	double total_cost;

	//int assin = 0;		//分配给边或是顶点 0 边 1 顶点
};


class pri_queue_item
{
public:
	Edge half_edge;
	double cost;

	bool operator < (const pri_queue_item& a) const 
	{
		return cost > a.cost;	//最小值
	}
};

class OTR
{
public:
	vector<Point> points_input;
	Delaunay delaunay_input;

	Triangulation tgl1;
	Triangulation tgl2;

	CGAL::Bbox_2 bbox;			//bounding box
	//Delaunay delaunay_temp;

	priority_queue<pri_queue_item> half_edge_queue;					//代价函数的优先队列

	map<vertex_handle, _Cost> vertex_points_map;			//点到顶点的分配
	map<Edge, _Cost> edge_points_map;					//点到边的分配

	map<vertex_handle, _Cost> vertex_points_map_temp;			//
	map<Edge, _Cost> edge_points_map_temp;					//

	//map<Edge, double>	edge_cost_map;							//每条边的代价 

	vector<vertex_pair> block_edge;				//

	vector<Point> assin_points;					//待分配的顶点

	vector<Edge> Delete_edge;				//分配到顶点后从边表中删除的边

	OTR();

	bool isborder;

	void Init(vector<Point> input);

	void InitPriQueue();
	bool IsCollapsable(Edge &e);		//
	void FlipEdge(Edge& e);
	void MakeCollap(Edge& e);	//合并边，合并后的结果在tgl2中
	void PickAndCollap();		//	选取队列中第一条边合并

	double CaculateAssinCost();		//计算当前分配方案总代价
	double CaculateEachEdgeCost();	//计算每条边的代价

	void CaculateNormalCost(Edge e, _Cost &c);	//计算法向代价
	void CaculateTangentialCost(Edge e, _Cost& c);	//计算切向代价
	void CaculateVertexCost(Edge e, _Cost& c);	//计算切向代价
	void AssinToVertex(Edge e, _Cost& c);		//重新分配给顶点

	Edge FindEdgeInTgl2(Edge e);

	bool face_has_point(Point p, Face_handle f);	//判断点p是不是在f内
	bool IsBoderEdge(Edge e);						//判断边e是否是边缘点
	bool PointEqual(Point a, Point b);				//判断点a与点b坐标是否相同

	Edge find_nearest_edge(Face_handle f, Point p);		//找到面f里离p最近的边


	double get_p_to_edge(Point p, Edge e);			//获取p到e的距离

	bool compute_triangle_ccw(Point pa,Point pb, Point pc)
	{

		auto ab = pb - pa;
		auto bc = pc - pb;
		double cross_product = ab.x() * bc.y() - bc.x() * ab.y();
		return cross_product >= -DBL_MIN;
	}

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