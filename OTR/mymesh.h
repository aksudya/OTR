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
#include <CGAL/Optimal_transportation_reconstruction_2.h>
//#include <gl/GLUT.h>
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
#include <Windows.h>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>
#include <osgUtil/Optimizer>
#include <osg/Point>
#include <osg/LineWidth>
#include <osgDB\ReadFile>
#include <osg\Node>
#include <omp.h>
#include "nanoflann.hpp"
//#define METHED2

using namespace std;
using namespace nanoflann;

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

struct apcitem
{
	Point p;
	Segment s;
};

struct annPointCloud
{
	vector<apcitem>  pts;
	void clear() { pts.clear(); }
};

struct PointCloudAdaptor
{

	const annPointCloud& obj; //!< A const ref to the data set origin

	/// The constructor that sets the data set source
	PointCloudAdaptor(const annPointCloud& obj_) : obj(obj_) { }

	/// CRTP helper method
	inline const annPointCloud& derived() const { return obj; }

	// Must return the number of data points
	inline size_t kdtree_get_point_count() const { return derived().pts.size(); }

	// Returns the dim'th component of the idx'th point in the class:
	// Since this is inlined and the "dim" argument is typically an immediate value, the
	//  "if/else's" are actually solved at compile time.
	inline double kdtree_get_pt(const size_t idx, const size_t dim) const
	{
		if (dim == 0) return derived().pts[idx].p.x();
		else if (dim == 1) return derived().pts[idx].p.y();
		else return derived().pts[idx].p.z();
	}

	template <class BBOX>
	bool kdtree_get_bbox(BBOX& /*bb*/) const { return false; }
};

typedef KDTreeSingleIndexAdaptor<
	L2_Simple_Adaptor<double, PointCloudAdaptor >,
	PointCloudAdaptor,
	3 /* dim */
> my_kd_tree_t;

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

	//Tree sampe_tree;
	map<Point,Segment> sample_map;

	annPointCloud cloud;



	mymesh();
	~mymesh();

	Point FindNearestVertex(Point &p);
	Segment FindNearestEdge(Point& p, my_kd_tree_t index1);
	int MakeCollaps(const Point& s, const Point& t);
	void BuildSampleKDtree();
	void Clear();

	mymesh& operator = (const mymesh& t) 
	{
		Vertexs = t.Vertexs;
		edges = t.edges;
		//IsoVertexs = t.IsoVertexs;
		return *this;
	}
};

