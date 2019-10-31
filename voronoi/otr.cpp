#include "otr.h"

OTR::OTR()
{
}

void OTR::Init(vector<Point> input)
{
	points_input = input;
	delaunay_input.insert(input.begin(), input.end());
	
}

void OTR::InitPriQueue()
{
	delaunay_temp = delaunay_input;
	int kk = 0;
	//delaunay_input.clear();
	for (auto eiter=delaunay_input.finite_edges_begin();eiter!=delaunay_input.finite_edges_end();eiter++)
	{

		//delaunay_temp.collapse_edge(*eiter);
		
		
		Segment s = delaunay_input.segment(eiter);
		
		Point a = s.point(0);
		Point b = s.point(1);

		
	}
	
}
