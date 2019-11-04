#include "otr.h"

OTR::OTR()
{
}

void OTR::Init(vector<Point> input)
{
	points_input = input;
	delaunay_input.insert(input.begin(), input.end());
	tgl1 = delaunay_input;
	//delaunay_temp = delaunay_input;
}

void OTR::InitPriQueue()
{
	//delaunay_temp = delaunay_input;
	int kk = 0;
	for (auto eiter = tgl1.finite_edges_begin(); eiter != tgl1.finite_edges_end(); eiter++)
	{
		kk++;


		//vertex_handle start_p = eiter->first->vertex(tgl1.cw(eiter->second));
		//vertex_handle end_p = eiter->first->vertex(tgl1.ccw(eiter->second));

		//if(kk==15)
		//{
		//IsCollapsable(*eiter, 0);
		//IsCollapsable(*eiter, 1);
			
		//}
		tgl2 = tgl1;

		Point s1 = source_vertex(*eiter)->point();
		Point t1 = target_vertex(*eiter)->point();

		Edge t2_edge;

		for (auto eiter2 = tgl2.finite_edges_begin(); eiter2 != tgl2.finite_edges_end(); eiter2++)
		{
			Point s2= source_vertex(*eiter2)->point();
			Point t2 = target_vertex(*eiter2)->point();

			if(s1.x()==s2.x()&&t1.x()==t2.x() && t1.y() == t2.y() && t1.y() == t2.y())
			{
				t2_edge = *eiter2;
			}
		}

		if (kk == 15)
		{
			//tgl2 = tgl1;
			//Edge t_edge = twin_edge(*eiter);
			while (!IsCollapsable(t2_edge))
			{
				FlipEdge(t2_edge);
				block_edge.clear();
			}
			Edge t_edge = twin_edge(t2_edge);
			tgl2.tds().join_vertices(t_edge.first, t_edge.second);
			break;
		}
		//



		//tgl2 = tgl1;
		//Edge t_edge = twin_edge(*eiter);
		//while (!IsCollapsable(t_edge))
		//{

		//	FlipEdge(t_edge);
		//	block_edge.clear();
		//}

		//tgl2.tds().join_vertices(t_edge);
		//


		/*Segment s = delaunay_input.segment(eiter);

		Point a = s.point(0);
		Point b = s.point(1);*/
	}

	//delaunay_input = delaunay_temp;
}


bool OTR::IsCollapsable(Edge &e)
{

	vertex_handle start_p = source_vertex(e);
	vertex_handle end_p = target_vertex(e);

	auto cviter = tgl2.incident_vertices(start_p);
	bool flag_collapsable = true;
	for (int i = 0; i < start_p->degree(); ++i)
	{
		auto temp = cviter;
		temp++;

		auto xj_a = cviter->point() - end_p->point();
		auto a_b = temp->point() - cviter->point();

		

		double cross_product = xj_a.x() * a_b.y() - xj_a.y() * a_b.x();

		if(cross_product<-DBL_MIN)		//˳ʱ��		todo
		{
			vertex_pair p(cviter, temp);
			block_edge.push_back(p);
			//Face_handle fe;
			//int fi;
			//tgl1.is_edge(cviter, temp, fe, fi);

			//Edge block_e(fe, fi);
			//block_edge.push_back(block_e);
			flag_collapsable = false;
		}

		cviter++;
	}



	return flag_collapsable;
}

void OTR::FlipEdge(Edge& e)
{
	Segment st(source_vertex(e)->point(), target_vertex(e)->point());

	double max_dist = DBL_MIN;
	vertex_pair blocked_edge;
	for (auto bit=block_edge.begin();bit!=block_edge.end();bit++)
	{
		Point ps = bit->first->point();
		Point pt = bit->second->point();
		Segment seg(ps, pt);
		Line lst = st.supporting_line();
		Line lab = seg.supporting_line();

		auto result= intersection(lst, lab);
		Point *ints_point;
		if (result)
		{
			ints_point = boost::get<Point>(&*result);		//����
		}
		
		double dist = squared_distance(*ints_point, target_vertex(e)->point());

		if (dist > max_dist)
		{
			max_dist = dist;
			blocked_edge = *bit;
		}
	}

	vertex_handle bs = blocked_edge.first;
	vertex_handle bt = blocked_edge.second;

	auto evit= tgl2.incident_edges(source_vertex(e));

	for (int i = 0; i < source_vertex(e)->degree(); ++i)
	{
		Edge ei = *evit;
		if (source_vertex(ei) != source_vertex(e))
		{
			ei = twin_edge(ei);
		}

		if (target_vertex(ei) == bs)
		{
			
			if (is_flippable(ei))
			{
				tgl2.tds().flip(ei.first, ei.second);
				break;
			}
		}
		if (target_vertex(ei) == bt)
		{
			if (is_flippable(ei))
			{
				tgl2.tds().flip(ei.first, ei.second);
				break;
			}		
		}

		evit++;
	}


	/*Edge ei = *tgl2.incident_edges(source_vertex(e));

	if (source_vertex(ei) != source_vertex(e))
		ei = twin_edge(ei);        

	int flag = 0;

	while (flag != 2) 
	{
		if (target_vertex(ei) == bs)
		{
			flag++;
			if (is_flippable(ei))
				tgl2.flip(ei.first, ei.second);
			break;
		}
		if (target_vertex(ei) == bt)
		{
			flag++;
			if (is_flippable(ei))
				tgl2.flip(ei.first, ei.second);
			break;
		}
		Edge pre_edge = prev_edge(ei);
		ei = twin_edge(pre_edge);
	}*/
}

bool OTR::is_flippable(Edge& edge)
{
	vertex_handle vt = target_vertex(edge);
	vertex_handle c = opposite_vertex(edge);
	Edge tw_edge = twin_edge(edge);
	vertex_handle a = opposite_vertex(tw_edge);



	double xa = a->point().x();
	double ya = a->point().y();

	double xt = vt->point().x();
	double yt = vt->point().y();

	double xc = c->point().x();
	double yc = c->point().y();

	double atx, tcx,aty, tcy;

	atx = xt - xa;
	aty = yt - ya;
	tcx = xc - xt;
	tcy = yc - yt;



	double cross_product = atx* tcy - aty * tcx;
	

	return cross_product>DBL_MIN;

}
