#include "otr.h"

OTR::OTR()
{
}

void OTR::Init(vector<Point> input)
{
	points_input = input;
	default_random_engine engine(3);
	vector<bool> index(input.size(),false);
	uniform_int_distribution<int> ud(0, input.size() - 1);
	for (int i = 0; i < 0.3*input.size(); ++i)
	{
		int id = ud(engine);
		while (index[id])
		{
			id = ud(engine);
		}
		index[id]=true;
	}

	for (int i = 0; i < input.size(); ++i)
	{
		if(index[i])
		{
			delaunay_input.insert(points_input[i]);
		}
		else
		{
			assin_points.push_back(points_input[i]);
		}
	}

	for (auto vit=delaunay_input.finite_vertices_begin();vit!=delaunay_input.finite_vertices_end();++vit)
	{
		ms1.Vertexs.insert(vit->point());
	}

	for (auto eit = delaunay_input.finite_edges_begin(); eit != delaunay_input.finite_edges_end(); ++eit)
	{
		Segment s(source_vertex(*eit)->point(), target_vertex(*eit)->point());
		ms1.edges.insert(s);
	}

	pri_cost = 0;
	iter_times = 0;
	ms2 = ms1;

	
	
#ifdef METHED2
	CaculateAssinCost();

	for (int i = 0; i < 5; ++i)
	{
		for (auto pit = ms2.Vertexs.begin(); pit != ms2.Vertexs.end(); ++pit)
		{
			Point newp = Relocatev(*pit);
			Point ppit = *pit;
			applyRelocate(ppit, newp);
		}
		vertex_points_map_temp.clear();
		edge_points_map_temp.clear();
		CaculateAssinCost();
	}

	vertex_points_map_temp.clear();
	edge_points_map_temp.clear();
	CaculateAssinCost();

	GetVaild();

	vertex_points_map_temp.clear();
	edge_points_map_temp.clear();
#endif

}

void OTR::InitPriQueue()
{
	int kk = 0;
	for (auto eit=ms1.edges.begin();eit!=ms1.edges.end();++eit)
	{
		kk++;
		ms2 = ms1;
		Segment edge1 = *eit;



		int re=ms2.MakeCollaps(edge1.source(), edge1.target());
		if(re==1)
		{
			assin_points.push_back(edge1.target());
		}
		assin_points.push_back(edge1.source());
		double cost = CaculateAssinCost();

		pri_queue_item pi;
		pi.edge= edge1;
		pi.cost = cost - pri_cost;
		half_edge_queue.push(pi);
		vertex_points_map_temp.clear();
		edge_points_map_temp.clear();
		assin_points.pop_back();
		if(re==1)
		{
			assin_points.pop_back();
		}


		ms2 = ms1;
		Segment edge1t = twin_edge(edge1);

		int re1 = ms2.MakeCollaps(edge1t.source(), edge1t.target());
		if (re1 == 1)
		{
			assin_points.push_back(edge1t.target());
		}
		assin_points.push_back(edge1t.source());
		double cost1 = CaculateAssinCost();

		pri_queue_item pi1;
		pi1.edge = edge1t;
		pi1.cost = cost1- pri_cost;
		half_edge_queue.push(pi1);
		vertex_points_map_temp.clear();
		edge_points_map_temp.clear();
		assin_points.pop_back();
		if (re1 == 1)
		{
			assin_points.pop_back();
		}

	}
	ms2 = ms1;
	cout << kk << " ";
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
	iter_times++;
	CaculateAssinCost();


	for (int i = 0; i < 1; ++i)
	{
		for (auto pit = ms2.Vertexs.begin(); pit != ms2.Vertexs.end(); ++pit)
		{
			Point newp = Relocatev(*pit);
			Point ppit = *pit;
			applyRelocate(ppit, newp);
		}
		vertex_points_map_temp.clear();
		edge_points_map_temp.clear();
		CaculateAssinCost();
	}
	

	//vertex_points_map_temp.clear();
	//edge_points_map_temp.clear();
	//GetVaild();

	assin_points.clear();
	for (auto ipit = points_input.begin(); ipit != points_input.end(); ++ipit)
	{
		Point ip = *ipit;
		if (ms2.Vertexs.find(ip) == ms2.Vertexs.end())
		{
			assin_points.push_back(ip);
		}
	}

	ms1 = ms2;

	vertex_points_map = vertex_points_map_temp;
	edge_points_map = edge_points_map_temp;			//为了显示
	vertex_points_map_temp.clear();
	edge_points_map_temp.clear();
	pri_cost = CaculateAssinCost();

	vertex_points_map_temp.clear();
	edge_points_map_temp.clear();
	InitPriQueue();

	Segment fst_edge = half_edge_queue.top().edge;
	cout << half_edge_queue.top().cost << endl;

	int re=ms2.MakeCollaps(fst_edge.source(), fst_edge.target());
	if (re == 1)
	{
		assin_points.push_back(fst_edge.target());
	}
	assin_points.push_back(fst_edge.source());

	ms1 = ms2;

	while (!half_edge_queue.empty())		//把优先队列清空
	{
		half_edge_queue.pop();
	}
	
	cout << iter_times << " ";
}

void OTR::GetVaild()
{
	ms2.Clear();
	assin_points.clear();
	for (auto epmit=edge_points_map_temp.begin();epmit!= edge_points_map_temp.end();++epmit)
	{
		if(!epmit->second.assined_points.empty())
		{
			ms2.edges.insert(epmit->first);
			ms2.Vertexs.insert(epmit->first.source());
			ms2.Vertexs.insert(epmit->first.target());
		}
	}
	for (auto ipit=points_input.begin();ipit!=points_input.end();++ipit)
	{
		Point ip = *ipit;
		if(ms2.Vertexs.find(ip)==ms2.Vertexs.end())
		{
			assin_points.push_back(ip);
		}
	}
}

void OTR::GetVaild1()
{
	ms2.Clear();
	assin_points.clear();
	for (auto epmit = edge_points_map_temp.begin(); epmit != edge_points_map_temp.end(); ++epmit)
	{
		if (epmit->second.assined_points.size()>=10)
		{
			ms2.edges.insert(epmit->first);
			ms2.Vertexs.insert(epmit->first.source());
			ms2.Vertexs.insert(epmit->first.target());
		}
	}
	for (auto ipit = points_input.begin(); ipit != points_input.end(); ++ipit)
	{
		Point ip = *ipit;
		if (ms2.Vertexs.find(ip) == ms2.Vertexs.end())
		{
			assin_points.push_back(ip);
		}
	}
}

//void OTR::ReLocate(vector<vertex_handle> ring,Point sp)
//{
//	int rvid = 0;
//	vector<pair<vertex_handle,Point>> toMove;
//	for (auto rvit=ring.begin();rvit!=ring.end();rvit++)
//	{
//
//		if (IsBoderPoint((*rvit)->point()))			//跳过边界点
//		{
//			rvid++;
//			continue;
//		}
//
//		Point AfterRelocate = Relocatev(*rvit);
//		vector<vertex_handle> one_ring_v = GetOneRingVertex(*rvit);
//		if(PointIsInRing(one_ring_v, AfterRelocate))
//		{
//			(*rvit)->point() = AfterRelocate;		//移动点
//		}
//		rvid++;
//	}
//}
//
//bool OTR::PointIsInRing(vector<vertex_handle> ring, Point sp)
//{
//	bool isIn = true;
//	for (int i = 0; i < ring.size(); ++i)
//	{
//		Point p1 = ring[i]->point();
//		int nexti = (i + 1) % (int)ring.size();
//		Point p2 = ring[nexti]->point();
//		if(!compute_triangle_ccw_line(p1,p2,sp))
//		{
//			isIn = false;
//			break;
//		}
//	}
//	return isIn;
//}
//
Point OTR::Relocatev(Point v)
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
	//auto ceiter = tgl2.incident_edges(v);

	vector<Segment> onestar;
	for (auto eit = ms2.edges.begin(); eit != ms2.edges.end(); ++eit)
	{
		if (eit->source() == v)
		{

			onestar.push_back(*eit);

		}
		if (eit->target() == v)
		{

			onestar.push_back(*eit);
		}
	}


	for (int i = 0; i < onestar.size(); ++i)
	{
		Segment curedge = onestar[i];
		if(curedge.source()!=v)		//确保方向是正确的
		{
			curedge = twin_edge(curedge);
		}

		auto cemp_it = edge_points_map_temp.find(curedge);
		double bx = curedge.target().x();
		double by= curedge.target().y();
		if (cemp_it != edge_points_map_temp.end())
		{
			_Cost v_cost = cemp_it->second;					
			for (auto cepit = v_cost.assined_points.begin(); cepit != v_cost.assined_points.end(); cepit++)		//当前边的上的每个点
			{
				double lamuda = Getlamuda(curedge, *cepit);
				double pix = cepit->x();
				double piy = cepit->y();
				sumRingEx += (1 - lamuda) * (pix - lamuda * bx);
				sumRingEy += (1 - lamuda) * (piy - lamuda * by);
				sumRingElow += (1 - lamuda) * (1 - lamuda);
			}
		}

		Segment ceitwin = twin_edge(curedge);						//在Map里可能存的是twin，所以也要找一遍
		auto cemp_it1 = edge_points_map_temp.find(ceitwin);		
		double bx1 = ceitwin.source().x();		//反向的所以是source
		double by1 = ceitwin.source().y();
		if (cemp_it1 != edge_points_map_temp.end())
		{
			_Cost v_cost = cemp_it1->second;
			for (auto cepit = v_cost.assined_points.begin(); cepit != v_cost.assined_points.end(); cepit++)		//当前边的上的每个点
			{
				double lamuda = Getlamuda(curedge, *cepit);
				double pix = cepit->x();
				double piy = cepit->y();
				sumRingEx += (1 - lamuda) * (pix - lamuda * bx1);
				sumRingEy += (1 - lamuda) * (piy - lamuda * by1);
				sumRingElow += (1 - lamuda) * (1 - lamuda);
			}
		}


		
	}

	double resx = (sumVpx + sumRingEx) / (sumVp + sumRingElow);
	double resy = (sumVpy + sumRingEy) / (sumVp + sumRingElow);

	if(sumVp+sumRingElow==0)		//如果分母为0就代表没变，直接复制
	{
		resx = v.x();
		resy = v.y();
	}

	Point returnp(resx, resy);

	return returnp;
}

double OTR::Getlamuda(Segment e, Point p)
{
	const Point ps = e.source();
	const Point pt = e.target();
	//Segment seg(ps, pt);
	Line le = e.supporting_line();
	Point project = le.projection(p);

	double x1 = ps.x();
	double x2 = pt.x();
	double xp = project.x();
	
	if(abs(x2-x1)<0.001)		//x坐标相差太小的话精度可能较低，换y坐标
	{
		x1 = ps.y();
		x2 = pt.y();
		xp = project.y();
	}

	double res = (xp - x1) / (x2 - x1);


	return res;
}

void OTR::applyRelocate(Point& s, Point& t)
{
	ms2.Vertexs.erase(s);
	ms2.Vertexs.insert(t);
	vector<Segment> eraseseg;
	vector<Segment> addseg;
	for (auto eit = ms2.edges.begin(); eit != ms2.edges.end(); ++eit)
	{
		if (eit->source() == s)
		{
			Segment newseg(t, eit->target());
			addseg.push_back(newseg);
			eraseseg.push_back(*eit);

		}
		if (eit->target() == s)
		{
			Segment newseg(eit->source(), t);
			addseg.push_back(newseg);
			eraseseg.push_back(*eit);
		}
	}

	for (auto eit=eraseseg.begin();eit!=eraseseg.end();eit++)
	{
		ms2.edges.erase(*eit);
	}
	for (auto ait = addseg.begin(); ait != addseg.end(); ait++)
	{
		ms2.edges.insert(*ait);
	}

}



double OTR::CaculateAssinCost()
{
	
	for (auto apit=assin_points.begin();apit!=assin_points.end();apit++)
	{
		Point pnow = *apit;
		Segment nearest_edge = ms2.FindNearestEdge(pnow);


		double dist2 = get_p_to_edge(pnow, nearest_edge);

		Segment twinn = twin_edge(nearest_edge);
		auto iter = edge_points_map_temp.find(nearest_edge);		//找最近的那条边e
		auto itert = edge_points_map_temp.find(twinn);				//找那条边的twine

		if (iter != edge_points_map_temp.end())		//如果e已经在map里了
		{
			auto find_it = find(iter->second.assined_points.begin(), iter->second.assined_points.end(), *apit);
			if (find_it == iter->second.assined_points.end())
			{
				iter->second.assined_points.push_back(*apit);
			}
		}
		else		//如果e没有在map里
		{
			if (itert != edge_points_map_temp.end())		//如果twine在map里
			{
				auto find_it = find(itert->second.assined_points.begin(), itert->second.assined_points.end(), *apit);
				if (find_it == itert->second.assined_points.end())
				{
					itert->second.assined_points.push_back(*apit);
				}
			}
			else			//如果都不在map里则插入一个新的项
			{
				_Cost cost;
				cost.assined_points.push_back(*apit);
				edge_points_map_temp.insert(pair<Segment, _Cost>(nearest_edge, cost));
			}
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
		Segment e = emit->first;
		_Cost cost = emit->second;
		CaculateNormalCost(e, cost);

		CaculateTangentialCost(e, cost);
		cost.to_edge_cost = cost.normal_cost + cost.tangential_cost;
		CaculateVertexCost(e, cost);
		if(cost.to_edge_cost<cost.to_vertex_cost)		//分配给边
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
			Delete_edge.push_back(e);         //先把要删的边存起来

		}
	}
	for (auto deit=Delete_edge.begin();deit!=Delete_edge.end();deit++)		//删除已经分配给顶点的那些边
	{
		edge_points_map_temp.erase(*deit);
	}
	Delete_edge.clear();
	return totalcost;
}



void OTR::AssinToVertex(Segment e, _Cost& c)
{
	const Point ps = e.source();
	const Point pt = e.target();
	for (auto pit = c.assined_points.begin(); pit != c.assined_points.end(); pit++)
	{
		double dists = squared_distance(*pit, ps);
		double distt = squared_distance(*pit, pt);


		if (dists < distt)	//分配给s
		{
			auto iter = vertex_points_map_temp.find(ps);

			if (iter != vertex_points_map_temp.end())
			{
					iter->second.assined_points.push_back(*pit);			
			}
			else
			{
				_Cost cost;
				cost.assined_points.push_back(*pit);
				vertex_points_map_temp.insert(pair<Point, _Cost>(ps, cost));
				//vertex_points_map_temp.insert(pair<vertex_handle, _Cost>(source_vertex(e), c));
			}
		}
		else
		{
			auto iter = vertex_points_map_temp.find(pt);

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
				vertex_points_map_temp.insert(pair<Point, _Cost>(pt, cost));
				//vertex_points_map_temp.insert(pair<vertex_handle, _Cost>(target_vertex(e), c));
			}
		}
	}
}




void OTR::CaculateNormalCost(Segment e, _Cost& c)
{
	double normal_cost = 0;
	for (auto pit=c.assined_points.begin();pit!=c.assined_points.end();pit++)
	{
		normal_cost += get_p_to_edge(*pit, e)* get_p_to_edge(*pit, e);
	}
	c.normal_cost = normal_cost;
}

double OTR::PointProjectToSource(Segment e, Point p)
{
	const Point ps = e.source();
	//const Point pt = target_vertex(e)->point();
	Line le = e.supporting_line();
	Point project = le.projection(p);
	double dist = sqrt(squared_distance(project, ps));
	return dist;
}

void OTR::CaculateTangentialCost(Segment e, _Cost& c)
{
	double tangential_cost = 0;
	double centerCord;		//每段中点的坐标
	const Point ps = e.source();
	const Point pt = e.target();
	
	vector<double> sort_tos;

	for (auto pit = c.assined_points.begin(); pit != c.assined_points.end(); pit++)
	{
		double dist = PointProjectToSource(e, *pit);
		sort_tos.push_back(dist);
	}
	sort(sort_tos.begin(), sort_tos.end());


	double l = sqrt( squared_distance(ps, pt))/ c.assined_points.size();	//每段的长度
	double sql =l*l;
	centerCord = l / 2;

	for (auto pit = sort_tos.begin(); pit != sort_tos.end(); pit++)
	{
		double dist = *pit;
		double sqci = (dist - centerCord) * (dist - centerCord);
		tangential_cost += sql / 12 + sqci;
		centerCord += l;
	}

	c.tangential_cost = tangential_cost;
}

void OTR::CaculateVertexCost(Segment e, _Cost& c)
{
	const Point ps = e.source();
	const Point pt = e.target();
	double CostS = 0;
	double Costt = 0;
	for (auto pit = c.assined_points.begin(); pit != c.assined_points.end(); pit++)
	{
		double dists = squared_distance(*pit, ps);
		double distt = squared_distance(*pit, pt);

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
		return false;
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





vector<Point> OTR::GetOneRingVertex(Point v)
{
	vector<Point> res;
	for (auto eit = ms2.edges.begin(); eit != ms2.edges.end(); ++eit)
	{
		if (eit->source() == v)
		{

			res.push_back(eit->target());

		}
		if (eit->target() == v)
		{
			
			res.push_back(eit->source());
			
		}
	}
	return res;
}

double OTR::get_p_to_edge(Point p, Segment e)
{
	double dist =sqrt( squared_distance(p, e));
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

		auto xj_a = cviter->point() - end_p->point();
		auto a_b = temp->point() - cviter->point();

		double cross_product = xj_a.x() * a_b.y() - xj_a.y() * a_b.x();

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
