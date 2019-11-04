#include "otr.h"

OTR::OTR()
{
}

void OTR::Init(vector<Point> input)
{
	points_input = input;
	delaunay_input.insert(input.begin(), input.end());
	delaunay_temp = delaunay_input;
}

void OTR::InitPriQueue()
{
	//delaunay_temp = delaunay_input;
	int kk = 0;
	//delaunay_input.clear();
	for (auto eiter=delaunay_temp.finite_edges_begin();eiter!=delaunay_temp.finite_edges_end();eiter++)
	{

		
		kk++;
		if(kk==5)
		{
			delaunay_temp;
			//break;
		}

		/*Segment s = delaunay_input.segment(eiter);
		
		Point a = s.point(0);
		Point b = s.point(1);*/
	}

	//delaunay_input = delaunay_temp;
}
