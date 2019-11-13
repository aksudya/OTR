#include "otr.h"

OTR::OTR()
{
}

void OTR::Init(vector<Point> input)
{
	points_input = input;
	delaunay_input.insert(input.begin(), input.end());
	bbox=bbox_2(input.begin(), input.end());
	double dl = (std::max)((bbox.xmax() - bbox.xmin()) / 2.,
		(bbox.ymax() - bbox.ymin()) / 2.);
	Point p1(bbox.xmin() - dl, bbox.ymin() - dl);
	Point p2(bbox.xmin() - dl, bbox.ymax() + dl);
	Point p3(bbox.xmax() + dl, bbox.ymax() + dl);
	Point p4(bbox.xmax() + dl, bbox.ymin() - dl);
	delaunay_input.insert(p1);
	delaunay_input.insert(p2);
	delaunay_input.insert(p3);
	delaunay_input.insert(p4);

	tgl1 = delaunay_input;
	isborder = false;
	//delaunay_temp = delaunay_input;
}

void OTR::InitPriQueue()
{
	//delaunay_temp = delaunay_input;
	int kk = 0;
	for (auto eiter = tgl1.finite_edges_begin(); eiter != tgl1.finite_edges_end(); eiter++)
	{
		kk++;

		tgl2 = tgl1;

		//Point s1 = source_vertex(*eiter)->point();
		//Point t1 = target_vertex(*eiter)->point();
		Edge t1_edge = *eiter;
		Edge t2_edge=FindEdgeInTgl2(t1_edge);

		if (!IsBoderEdge(t2_edge))
		{
			MakeCollap(t2_edge);

			//Edge e = t2_edge;

			double cost = CaculateAssinCost();
			pri_queue_item pi;
			pi.half_edge = t1_edge;
			pi.cost = cost;
			half_edge_queue.push(pi);
			vertex_points_map_temp.clear();
			edge_points_map_temp.clear();

			//caculate cost


			assin_points.pop_back();
		}



		tgl2 = tgl1;
		Edge t1_edge_twin = twin_edge(t1_edge);


		if (!IsBoderEdge(t1_edge_twin))
		{
			Edge t2_edge_twin = FindEdgeInTgl2(t1_edge_twin);
			MakeCollap(t2_edge_twin);


			double cost1 = CaculateAssinCost();
			pri_queue_item pi1;
			pi1.half_edge = t1_edge_twin;
			pi1.cost = cost1;
			half_edge_queue.push(pi1);
			vertex_points_map_temp.clear();
			edge_points_map_temp.clear();

			//caculate cost

			assin_points.pop_back();
			//isborder = false;
		}
	
	}
	tgl2 = tgl1;
	cout << kk<<" ";
	//delaunay_input = delaunay_temp;
}


Edge OTR::FindEdgeInTgl2(Edge e)
{
	Point s1 = source_vertex(e)->point();
	Point t1 = target_vertex(e)->point();
	Edge re;
	for (auto eiter2 = tgl2.finite_edges_begin(); eiter2 != tgl2.finite_edges_end(); eiter2++)
	{
		Point s2 = source_vertex(*eiter2)->point();
		Point t2 = target_vertex(*eiter2)->point();

		if (s1.x() == s2.x() && s1.y() == s2.y() && t1.x() == t2.x() && t1.y() == t2.y())
		{
			re=*eiter2;
		}
	}
	for (auto eiter2 = tgl2.finite_edges_begin(); eiter2 != tgl2.finite_edges_end(); eiter2++)
	{
		Point s2 = target_vertex(*eiter2)->point();
		Point t2 = source_vertex(*eiter2)->point();

		if (s1.x() == s2.x() && s1.y() == s2.y() && t1.x() == t2.x() && t1.y() == t2.y())
		{
			re = twin_edge(*eiter2);
		}
	}
	return re;
}


void OTR::PickAndCollap()
{
	//for (int i = 0; i <80; ++i)
	//{
		Edge fst_edge = half_edge_queue.top().half_edge;
		cout << half_edge_queue.top().cost<<endl;
		Edge fst_edge_tgl2 = FindEdgeInTgl2(fst_edge);
		half_edge_queue.pop();
		vertex_handle vs = source_vertex(fst_edge_tgl2);
		vector<vertex_handle> one_ring_s = GetOneRingVertex(vs);
		MakeCollap(fst_edge_tgl2);
		//if (i >= 35 - 17)
		//{
			
			CaculateAssinCost();
			ReLocate(one_ring_s, vs->point());
		//}
		

		vertex_points_map_temp.clear();
		edge_points_map_temp.clear();

		tgl1 = tgl2;
		while (!half_edge_queue.empty())
		{
			half_edge_queue.pop();
		}
		InitPriQueue();
		//cout << i << " ";
	//}
	
	
}

void OTR::ReLocate(vector<vertex_handle> ring,Point sp)
{
	int rvid = 0;
	vector<pair<vertex_handle,Point>> toMove;
	for (auto rvit=ring.begin();rvit!=ring.end();rvit++)
	{
		//todo
		double dl = (std::max)((bbox.xmax() - bbox.xmin()) / 2.,
			(bbox.ymax() - bbox.ymin()) / 2.);
		Point * p[4];

		p[0] = new Point(bbox.xmin() - dl, bbox.ymin() - dl);
		p[1] = new Point(bbox.xmin() - dl, bbox.ymax() + dl);
		p[2] = new Point(bbox.xmax() + dl, bbox.ymax() + dl);
		p[3] = new Point(bbox.xmax() + dl, bbox.ymin() - dl);

		bool isBox = false;
		for (int i = 0; i < 4; ++i)
		{
			if(PointEqual((*rvit)->point(), *p[i]))
			{
				isBox = true;
				break;
			}
		}
		if(isBox)
		{
			rvid++;
			continue;
		}

		Point AfterRelocate = Relocatev(*rvit);
		vector<vertex_handle> one_ring_v = GetOneRingVertex(*rvit);
		if(PointIsInRing(one_ring_v, AfterRelocate))
		{
			(*rvit)->point() = AfterRelocate;
			//pair<vertex_handle, Point> pp(*rvit, AfterRelocate);
			//toMove.push_back(pp);
		}
		rvid++;
	}
	//for (auto tmit=toMove.begin();tmit!=toMove.end();tmit++)
	//{
	//	vertex_handle vm = tmit->first;
	//	vm->point() = tmit->second;

	//	//tgl2.move_if_no_collision(tmit->first, tmit->second);
	//}
	
}

bool OTR::PointIsInRing(vector<vertex_handle> ring, Point sp)
{
	bool isIn = true;
	for (int i = 0; i < ring.size(); ++i)
	{
		Point p1 = ring[i]->point();
		
		int nexti = (i + 1) % ring.size();
		Point p2 = ring[nexti]->point();
		if(!compute_triangle_ccw_line(p1,p2,sp))
		{
			isIn = false;
			break;
		}
	}
	return isIn;
}

Point OTR::Relocatev(vertex_handle v)
{
	auto vcit=vertex_points_map_temp.find(v);

	double sumVpx = 0;
	double sumVpy = 0;
	int sumVp=0;
	if (vcit != vertex_points_map_temp.end())
	{
		_Cost v_cost = vcit->second;
		for (auto vcpit=v_cost.assined_points.begin();vcpit!=v_cost.assined_points.end();vcpit++)
		{
			sumVpx += vcpit->x();
			sumVpy += vcpit->y();
			sumVp++;
		}
	}

	double sumRingEx = 0;
	double sumRingEy = 0;
	double sumRingElow = 0;
	auto ceiter = tgl2.incident_edges(v);
	
	for (int i = 0; i < v->degree(); ++i)
	{
		auto cemp_it = edge_points_map_temp.find(*ceiter);		//curr_edge map item
		double bx = target_vertex(*ceiter)->point().x();
		double by= target_vertex(*ceiter)->point().y();
		if (cemp_it != edge_points_map_temp.end())
		{
			_Cost v_cost = cemp_it->second;					
			for (auto cepit = v_cost.assined_points.begin(); cepit != v_cost.assined_points.end(); cepit++)		//当前边的上的每个点
			{
				double lamuda = Getlamuda(*ceiter, *cepit);
				double pix = cepit->x();
				double piy = cepit->y();
				sumRingEx += (1 - lamuda) * (pix - lamuda * bx);
				sumRingEy += (1 - lamuda) * (piy - lamuda * by);
				sumRingElow += (1 - lamuda) * (1 - lamuda);
			}
		}

		++ceiter;
	}

	double resx = (sumVpx + sumRingEx) / (sumVp + sumRingElow);
	double resy = (sumVpy + sumRingEy) / (sumVp + sumRingElow);

	if(sumVp+sumRingElow==0)
	{
		resx = v->point().x();
		resy = v->point().y();
	}

	//todo
	Point returnp(resx, resy);

	return returnp;
}

double OTR::Getlamuda(Edge e, Point p)
{
	const Point ps = source_vertex(e)->point();
	const Point pt = target_vertex(e)->point();
	Segment seg(ps, pt);
	Line le = seg.supporting_line();
	Point project = le.projection(p);

	double x1 = ps.x();
	double x2 = pt.x();
	double xp = p.x();
	
	if(abs(x2-x1)<0.00001)
	{
		x1 = ps.y();
		x2 = pt.y();
		xp = p.y();
	}

	double res = (xp - x1) / (x2 - x1);


	return res;
}

void OTR::MakeCollap(Edge& e)
{
	assin_points.push_back(source_vertex(e)->point());
	while (!IsCollapsable(e))
	{
		FlipEdge(e);
		block_edge.clear();
	}
	//if (!isborder)
	//{
		Edge t_edge = twin_edge(e);
		tgl2.tds().join_vertices(t_edge.first, t_edge.second);
		//block_edge.clear();
	//}
	block_edge.clear();
	//isborder = false;
}



double OTR::CaculateAssinCost()
{
	
	for (auto apit=assin_points.begin();apit!=assin_points.end();apit++)
	{
		Face_handle face_now;
		for (auto fit=tgl2.all_faces_begin();fit!=tgl2.all_faces_end();fit++)
		{
			if(face_has_point(*apit,fit))
			{
				face_now = fit;
				break;
			}
		}

		Edge nearest_edge = find_nearest_edge(face_now, *apit);
		auto iter = edge_points_map_temp.find(nearest_edge);

		if (iter != edge_points_map_temp.end())
		{
			//_Cost cost = iter->second;
			iter->second.assined_points.push_back(*apit);
			//iter->second = cost;
		}	
		else
		{
			_Cost cost;
			cost.assined_points.push_back(*apit);
			edge_points_map_temp.insert(pair<Edge, _Cost>(nearest_edge,cost));
		}
	}

	double cost=CaculateEachEdgeCost();


	return cost;
}

double OTR::CaculateEachEdgeCost()
{
	double totalcost = 0;
	for (auto emit=edge_points_map_temp.begin();emit!=edge_points_map_temp.end();emit++)
	{
		Edge e = emit->first;
		_Cost cost = emit->second;

		CaculateNormalCost(e, cost);
		CaculateTangentialCost(e, cost);
		cost.to_edge_cost = cost.normal_cost + cost.tangential_cost;
		CaculateVertexCost(e, cost);
		if(cost.to_edge_cost<cost.to_vertex_cost && !IsBoderEdge(e))		//分配给边
		{
			cost.total_cost = cost.to_edge_cost;
			totalcost += cost.total_cost;
			emit->second = cost;
		}
		else									//重新分配给顶点
		{
			cost.total_cost = cost.to_vertex_cost;
			totalcost += cost.total_cost;
			AssinToVertex(e, cost);
			Delete_edge.push_back(e);
			//emit->second = cost;
		}
	}
	for (auto deit=Delete_edge.begin();deit!=Delete_edge.end();deit++)
	{
		edge_points_map_temp.erase(*deit);
		//Edge e = *deit;
	}
	Delete_edge.clear();
	return totalcost;
}

void OTR::AssinToVertex(Edge e, _Cost& c)
{
	const Point ps = source_vertex(e)->point();
	const Point pt = target_vertex(e)->point();
	for (auto pit = c.assined_points.begin(); pit != c.assined_points.end(); pit++)
	{
		double dists = squared_distance(*pit, ps);
		double distt = squared_distance(*pit, pt);

		if (IsBoderPoint(ps))
		{
			auto iter = vertex_points_map_temp.find(target_vertex(e));

			if (iter != vertex_points_map_temp.end())
			{
				iter->second.assined_points.push_back(*pit);
			}
			else
			{
				_Cost cost;
				cost.assined_points.push_back(*pit);
				vertex_points_map_temp.insert(pair<vertex_handle, _Cost>(target_vertex(e), cost));
				//vertex_points_map_temp.insert(pair<vertex_handle, _Cost>(target_vertex(e), c));
			}
			continue;
		}
		else if (IsBoderPoint(pt))
		{
			auto iter = vertex_points_map_temp.find(source_vertex(e));

			if (iter != vertex_points_map_temp.end())
			{
				//for (auto apit=c.assined_points.begin();apit!=c.assined_points.end();apit++)
				//{
				iter->second.assined_points.push_back(*pit);
				//}				
			}
			else
			{
				_Cost cost;
				cost.assined_points.push_back(*pit);
				vertex_points_map_temp.insert(pair<vertex_handle, _Cost>(source_vertex(e), cost));
				//vertex_points_map_temp.insert(pair<vertex_handle, _Cost>(source_vertex(e), c));
			}
			continue;
		}

		if (dists < distt)	//分配给s
		{
			auto iter = vertex_points_map_temp.find(source_vertex(e));

			if (iter != vertex_points_map_temp.end())
			{
				//for (auto apit=c.assined_points.begin();apit!=c.assined_points.end();apit++)
				//{
					iter->second.assined_points.push_back(*pit);
				//}				
			}
			else
			{
				_Cost cost;
				cost.assined_points.push_back(*pit);
				vertex_points_map_temp.insert(pair<vertex_handle, _Cost>(source_vertex(e), cost));
				//vertex_points_map_temp.insert(pair<vertex_handle, _Cost>(source_vertex(e), c));
			}
		}
		else
		{
			auto iter = vertex_points_map_temp.find(target_vertex(e));

			if (iter != vertex_points_map_temp.end())
			{
				//for (auto apit = c.assined_points.begin(); apit != c.assined_points.end(); apit++)
				//{
					iter->second.assined_points.push_back(*pit);
				//}
			}
			else
			{
				_Cost cost;
				cost.assined_points.push_back(*pit);
				vertex_points_map_temp.insert(pair<vertex_handle, _Cost>(target_vertex(e), cost));
				//vertex_points_map_temp.insert(pair<vertex_handle, _Cost>(target_vertex(e), c));
			}
		}
	}
}




void OTR::CaculateNormalCost(Edge e, _Cost& c)
{
	double normal_cost = 0;
	for (auto pit=c.assined_points.begin();pit!=c.assined_points.end();pit++)
	{
		normal_cost += get_p_to_edge(*pit, e)* get_p_to_edge(*pit, e);
	}
	c.normal_cost = normal_cost;
}

void OTR::CaculateTangentialCost(Edge e, _Cost& c)
{
	double tangential_cost = 0;
	double centerCord;		//每段中点的坐标
	const Point ps = source_vertex(e)->point();
	const Point pt = target_vertex(e)->point();
	double l = sqrt( squared_distance(ps, pt))/c.assined_points.size();	//每段的长度
	double sql = l * l;
	centerCord = l / 2;
	for (auto pit = c.assined_points.begin(); pit != c.assined_points.end(); pit++)
	{
		Segment seg(ps, pt);
		Line le = seg.supporting_line();
		Point project = le.projection(*pit);
		double dist = sqrt( squared_distance(project,ps));
		double sqci = (dist - centerCord)* (dist - centerCord);
		tangential_cost += sql / 12 + sqci;
		centerCord += l;
	}
	c.tangential_cost = tangential_cost;
}

void OTR::CaculateVertexCost(Edge e, _Cost& c)
{
	const Point ps = source_vertex(e)->point();
	const Point pt = target_vertex(e)->point();
	double CostS = 0;
	double Costt = 0;
	for (auto pit = c.assined_points.begin(); pit != c.assined_points.end(); pit++)
	{
		double dists = squared_distance(*pit, ps);
		double distt = squared_distance(*pit, pt);

		if(IsBoderPoint(ps))
		{
			Costt += distt;
			continue;
		}
		else if(IsBoderPoint(pt))
		{
			CostS += dists;
			continue;
		}

		if(dists<distt)
		{
			CostS += dists;
		}
		else
		{
			Costt += distt;
		}
	}
	c.to_vertex_cost = CostS + Costt;

}



bool OTR::face_has_point(Point p, Face_handle f)
{
	if(tgl2.is_infinite(f))
	{
		return true;
	}
	for (int i = 0; i < 3; ++i) 
	{
		
		Edge edge(f, i);
		const Point ps = source_vertex(edge)->point();
		const Point pt = target_vertex(edge)->point();
		if (!compute_triangle_ccw(ps, pt, p))
			return false;
	}
	return true;
}

bool OTR::IsBoderEdge(Edge e)
{
	double dl = (std::max)((bbox.xmax() - bbox.xmin()) / 2.,
		(bbox.ymax() - bbox.ymin()) / 2.);
	Point* p[4];

	p[0]=new Point(bbox.xmin() - dl, bbox.ymin() - dl);
	p[1]=new Point(bbox.xmin() - dl, bbox.ymax() + dl);
	p[2]=new Point(bbox.xmax() + dl, bbox.ymax() + dl);
	p[3]=new Point(bbox.xmax() + dl, bbox.ymin() - dl);

	Point s = source_vertex(e)->point();
	Point t = target_vertex(e)->point();

	bool match = false;

	for (int i = 0; i < 4; ++i)
	{
		if(PointEqual(s,*p[i])||PointEqual(t,*p[i]))
		{
			match = true;
			break;
		}
	}

	return match;
}

bool OTR::IsBoderPoint(Point point)
{
	double dl = (std::max)((bbox.xmax() - bbox.xmin()) / 2.,
		(bbox.ymax() - bbox.ymin()) / 2.);
	Point * p[4];

	p[0] = new Point(bbox.xmin() - dl, bbox.ymin() - dl);
	p[1] = new Point(bbox.xmin() - dl, bbox.ymax() + dl);
	p[2] = new Point(bbox.xmax() + dl, bbox.ymax() + dl);
	p[3] = new Point(bbox.xmax() + dl, bbox.ymin() - dl);

	bool isBox = false;
	for (int i = 0; i < 4; ++i)
	{
		if (PointEqual(point, *p[i]))
		{
			isBox = true;
			break;
		}
	}
	return isBox;
}

bool OTR::PointEqual(Point a, Point b)
{
	return abs(a.x()- b.x())<1e-9 && abs(a.y() - b.y())<1e-9;
}



Edge OTR::find_nearest_edge(Face_handle f, Point p)
{
	Edge nearest(f, 0);
	double min_dist2 =-DBL_MAX;
	for (int i = 0; i < 3; ++i)
	{
		Edge edge(f, i);
		double dist2 = get_p_to_edge(p, edge);
		if (dist2 < min_dist2) {
			min_dist2 = dist2;
			nearest = edge;
		}
	}

	return nearest;
}

vector<vertex_handle> OTR::GetOneRingVertex(vertex_handle v)
{
	auto cviter = tgl2.incident_vertices(v);
	vector<vertex_handle> res;
	for (int i = 0; i < v->degree(); ++i)
	{
		res.push_back(cviter);
		++cviter;
	}


	return res;
}

double OTR::get_p_to_edge(Point p, Edge e)
{
	const Point ps = source_vertex(e)->point();
	const Point pt = target_vertex(e)->point();
	Segment seg(ps, pt);

	Line lst = seg.supporting_line();

	double dist =sqrt( squared_distance(p, lst));
	return dist;
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
		//if(tgl2.is_infinite(cviter)||tgl2.is_infinite(temp))
		//{
			//isborder = true;
			//tgl2.remove(start_p);
			//return true;
		//}
		//if((i==start_p->degree()-1)&&temp!= tgl2.incident_vertices(start_p))
		//{
		//	//tgl2.remove(start_p);
		//	break;
		//}

		double cvx = 0;
		double cvy = 0;
		double endx = 0;
		double endy = 0;
		double tempx = 0;
		double tempy = 0;

		cvx = cviter->point().x();
		cvy = cviter->point().y();

		endx = end_p->point().x();
		endy = end_p->point().y();

		tempx = temp->point().x();
		tempy = temp->point().y();

		//auto xj_a = cviter->point() - end_p->point();
		//auto a_b = temp->point() - cviter->point();

		double xjax = cvx - endx;
		double xjay = cvy - endy;
		double abx = tempx - cvx;
		double aby = tempy - cvy;

		//double eps = 1e-9;
		//bool kkkkkk=abs(a_b.x()) < eps;

		double cross_product = xjax * aby - xjay * abx;

		if(cross_product<-DBL_MIN)		//		顺时针
		{
			vertex_pair p(cviter, temp);
			block_edge.push_back(p);
			flag_collapsable = false;
		}

		cviter++;
	}



	return flag_collapsable;
}

void OTR::FlipEdge(Edge& e)
{
	Segment st(source_vertex(e)->point(), target_vertex(e)->point());

	double max_dist = -1;
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
			ints_point = boost::get<Point>(&*result);		//获取距交点长度
		}
		
		double dist = sqrt(squared_distance(*ints_point, target_vertex(e)->point()));

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
	

	return cross_product>-DBL_MIN;

}
