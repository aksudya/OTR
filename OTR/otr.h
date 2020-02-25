#pragma once
//#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
//#include <CGAL/point_generators_2.h>
//#include <CGAL/Optimal_transportation_reconstruction_2.h>
//
//#include <CGAL/Polygon_2.h>
//#include <CGAL/Periodic_2_triangulation_2.h>
//#include <GLUT.h>
//#include <iostream>
//#include <cmath>
//#include <random>
//#include <ctime>
//#include <fstream> 

#include "mymesh.h"

using namespace std;


//
//typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
//
//typedef K::Point_2                                          Point;
//typedef CGAL::Delaunay_triangulation_2<K>					Delaunay;
//typedef CGAL::Triangulation_2<K>							Triangulation;
//typedef Triangulation::Edge									Edge;
//typedef Triangulation::Face									Face;
//typedef Triangulation::Face_handle							Face_handle;
//typedef Triangulation::Vertex_handle						vertex_handle;
//typedef K::Segment_2										Segment;
//typedef CGAL::Polygon_2<K>									Polygon_2;
//typedef K::Line_2											Line;
//typedef pair<vertex_handle, vertex_handle>					vertex_pair;

class _Cost
{
public:
	vector<Point> assined_points;
	double to_edge_cost=0;
	double to_vertex_cost=0;
	double normal_cost=0;
	double tangential_cost=0;

	double total_cost=0;
};


class pri_queue_item
{
public:
	Segment edge;
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

	mymesh ms1;
	mymesh ms2;

	CGAL::Bbox_2 bbox;			//bounding box

	double pri_cost;		//	上一次的代价

	int iter_times;

	priority_queue<pri_queue_item> half_edge_queue;					//代价函数的优先队列

	map<Point, _Cost> vertex_points_map;			//点到顶点的分配
	map<Segment, _Cost,Segment_more> edge_points_map;					//点到边的分配

	map<Point, _Cost> vertex_points_map_temp;			//
	map<Segment, _Cost, Segment_more> edge_points_map_temp;					//

	vector<vertex_pair> block_edge;				//

	vector<Point> assin_points;					//待分配的顶点

	vector<Segment> Delete_edge;				//分配到顶点后从边表中删除的边

	set<Segment, Segment_more> to_be_Collaps;				//下次迭代中将要被计算合并的边

	vector<Point> OneRingPoint;

	OTR();

	bool isborder;

	void Init(vector<Point> input);

	void InitPriQueue();
	bool IsCollapsable(Edge &e);		
	void FlipEdge(Edge& e);
	void MakeCollap(Edge& e);	//合并边，合并后的结果在tgl2中
	void PickAndCollap();		//	选取队列中第一条边合并
	void GetVaild();			//获取有效边并更新ms2
	void GetVaild1();			//获取有效边并更新ms2
	void GetVaild2();
	void GetOneRingEdge(vector<Point> s);		//获取一圈的边

	double CaculateAssinCost();		//计算当前分配方案总代价
	double CaculateEachEdgeCost();	//计算每条边的代价
	double PointProjectToSource(Segment e, Point p);	//计算点p在e上的投影到e原点的距离

	void CaculateNormalCost(Segment e, _Cost &c);	//计算法向代价
	void CaculateTangentialCost(Segment e, _Cost& c);	//计算切向代价
	void CaculateVertexCost(Segment e, _Cost& c);	//计算切向代价
	void AssinToVertex(Segment e, _Cost& c);		//重新分配给顶点
	void ReLocate(vector<vertex_handle> ring,Point sp);	//relocate sp为合并边的源点

	Point Relocatev(Point v);		//relocate顶点v
	double Getlamuda(Segment e, Point p);		//获取点P在e上投影的重心坐标lamuda
	void applyRelocate(Point& s, Point& t);		//将s移到t

	Edge FindEdgeInTgl2(Edge e);

	bool PointIsInRing(vector<vertex_handle> ring, Point sp);		//判断点sp是否在ring内
	bool IsBoderEdge(Edge e);						//判断边e是否是边缘点
	bool IsBoderPoint(Point p);						//判断点p是否是边缘点
	bool PointEqual(Point a, Point b);				//判断点a与点b坐标是否相同


	vector<Point> GetOneRingVertex(Point v);	//获取tgl2中顶点v周围一圈的顶点

	double get_p_to_edge(Point p, Segment e);			//获取p到e的距离

	bool compute_triangle_ccw(Point pa,Point pb, Point pc)		//认为共线为逆时针
	{

		auto ab = pb - pa;
		auto bc = pc - pb;
		double cross_product = ab.x() * bc.y() - bc.x() * ab.y();
		return cross_product > -DBL_MIN;
	}

	bool compute_triangle_ccw_line(Point pa, Point pb, Point pc)	//认为共线不为逆时针
	{

		auto ab = pb - pa;
		auto bc = pc - pb;
		double cross_product = ab.x() * bc.y() - bc.x() * ab.y();
		return cross_product > DBL_MIN;
	}

	//template<typename T>
	//vertex_handle source_vertex(T edge)
	//{
	//	return edge.first->vertex(tgl2.ccw(edge.second));
	//}

	//template<typename T>
	//vertex_handle target_vertex(T edge)
	//{
	//	return edge.first->vertex(tgl2.cw(edge.second));
	//}

	

	Segment twin_edge(Segment& edge)
	{
		Segment re(edge.target(), edge.source());
		return re;
	}

	vertex_handle opposite_vertex(Edge& edge)
	{
		return edge.first->vertex(edge.second);
	}

	bool is_flippable(Edge& edge);
};