#include "otr.h"

OTR::OTR()
{
}

void OTR::Init(vector<Point> input)
{
	points_input = input;
	default_random_engine engine(2);
	vector<bool> index(input.size(),false);
	uniform_int_distribution<int> ud(0, input.size() - 1);
	for (int i = 0; i < 0.1*input.size(); ++i)
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
		
		Point p1=eit->first->vertex(eit->second)->point();
		Point p2 = eit->first->vertex(eit->third)->point();
		Segment s(p1,p2);
		ms1.edges.insert(s);
	}

	

	pri_cost = 0;
	iter_times = 0;
	ms2 = ms1;

	//for (auto eit = ms2.edges.begin(); eit != ms2.edges.end(); ++eit)
	//{
	//	Segment sss = *eit;
	//	to_be_Collaps.insert(sss);
	//	to_be_Collaps.insert(twin_edge(sss));
	//}
	
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
	for (auto eit= to_be_Collaps.begin();eit!= to_be_Collaps.end();++eit)
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


		/*ms2 = ms1;
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
		}*/

	}
	ms2 = ms1;
	cout << " queuesize:" << half_edge_queue.size()<<" "<<ms2.Vertexs.size() <<" "<< to_be_Collaps.size() << " ";
}





void OTR::PickAndCollap()
{
	iter_times++;
	//CaculateAssinCost();
	////for (auto eit = ms2.edges.begin(); eit != ms2.edges.end(); ++eit)
	////{
	////	to_be_Collaps.insert(*eit);
	////}

	//for (int i = 0; i < 1; ++i)
	//{
	//	vector<Point> newps;
	//	vector<Point> ppits;
	//	int nump = 0;
	//	for (auto pit = OneRingPoint.begin(); pit != OneRingPoint.end(); ++pit)
	//	{
	//		Point newp = Relocatev(*pit);
	//		Point ppit = *pit;
	//		nump++;
	//		newps.push_back(newp);
	//		ppits.push_back(ppit);
	//	}

	//	for (int i = 0; i < nump; ++i)
	//	{
	//		applyRelocate(ppits[i], newps[i]);
	//	}
	//	
	//	
	//	vertex_points_map_temp.clear();
	//	edge_points_map_temp.clear();
	//	CaculateAssinCost();
	//}
	//

	////vertex_points_map_temp.clear();
	////edge_points_map_temp.clear();
	////GetVaild();

	//assin_points.clear();
	//for (auto ipit = points_input.begin(); ipit != points_input.end(); ++ipit)
	//{
	//	Point ip = *ipit;
	//	if (ms2.Vertexs.find(ip) == ms2.Vertexs.end())
	//	{
	//		assin_points.push_back(ip);
	//	}
	//}

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

	OneRingPoint.clear();
	OneRingPoint = GetOneRingVertex(fst_edge.source());
	

	int re=ms2.MakeCollaps(fst_edge.source(), fst_edge.target());
	if (re == 1)
	{
		assin_points.push_back(fst_edge.target());
	}
	assin_points.push_back(fst_edge.source());


	CaculateAssinCost();


	//for (int i = 0; i < 1; ++i)
	//{
	//	vector<Point> newps;
	//	vector<Point> ppits;
	//	int nump = 0;
	//	for (auto pit = OneRingPoint.begin(); pit != OneRingPoint.end(); ++pit)
	//	{
	//		Point newp = Relocatev(*pit);
	//		Point ppit = *pit;
	//		nump++;
	//		newps.push_back(newp);
	//		ppits.push_back(ppit);
	//	}

	//	for (int i = 0; i < nump; ++i)
	//	{
	//		applyRelocate(ppits[i], newps[i]);
	//	}


	//	vertex_points_map_temp.clear();
	//	edge_points_map_temp.clear();
	//	CaculateAssinCost();
	//	OneRingPoint = newps;
	//}




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

	GetOneRingEdge(OneRingPoint);
	priority_queue<pri_queue_item> temp_queue;
	//cout << endl;
	while (!half_edge_queue.empty())		//把优先队列清空
	{
		//cout << half_edge_queue.top().cost << endl;
		Segment eedge = half_edge_queue.top().edge;
		/*if(eedge==fst_edge)
		{
			
		}*/

		if(to_be_Collaps.find(eedge)==to_be_Collaps.end() 
			&& (ms2.edges.find(eedge) != ms2.edges.end()
			|| ms2.edges.find(twin_edge(eedge)) != ms2.edges.end()))
		{
			pri_queue_item aa = half_edge_queue.top();
			temp_queue.push(aa);
		}
		half_edge_queue.pop();
	}
	half_edge_queue = temp_queue;
	cout << iter_times <<" ";
}

void OTR::GetOneRingEdge(vector<Point> s)
{
	to_be_Collaps.clear();
	set<Point> onePoint;
	for (auto pit = s.begin(); pit != s.end(); ++pit)
	{
		for (auto eit = ms2.edges.begin(); eit != ms2.edges.end(); ++eit)
		{
			if (eit->source() == *pit)
			{

				onePoint.insert(eit->target());

			}
			if (eit->target() == *pit)
			{

				onePoint.insert(eit->source());

			}
		}
	}
	//onePoint.push_back(e.source());

	for (auto pit = onePoint.begin(); pit != onePoint.end(); ++pit)
	{
		for (auto eit = ms2.edges.begin(); eit != ms2.edges.end(); ++eit)
		{
			if (eit->source() == *pit)
			{
				Segment edge1 = *eit;
				to_be_Collaps.insert(edge1);
				to_be_Collaps.insert(twin_edge(edge1));
				//onePoint.push_back(eit->target());

			}
			if (eit->target() == *pit)
			{
				Segment edge2 = *eit;
				to_be_Collaps.insert(edge2);
				to_be_Collaps.insert(twin_edge(edge2));
				//onePoint.push_back(eit->source());

			}
		}
	}

	//return res;
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

void OTR::GetVaild2()
{
	ms2.Clear();
	assin_points.clear();
	for (auto epmit = edge_points_map_temp.begin(); epmit != edge_points_map_temp.end(); ++epmit)
	{
		Segment s = epmit->first;
		double len = sqrt(s.squared_length());
		if (!epmit->second.assined_points.empty()|| len<0.07)
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

void OTR::GetVaild1()
{
	ms2.Clear();
	assin_points.clear();
	for (auto epmit = edge_points_map_temp.begin(); epmit != edge_points_map_temp.end(); ++epmit)
	{
		if (epmit->second.assined_points.size()>=5)
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
	double sumVpz = 0;

	int sumVp=0;
	if (vcit != vertex_points_map_temp.end())
	{
		_Cost v_cost = vcit->second;
		for (auto vcpit=v_cost.assined_points.begin();vcpit!=v_cost.assined_points.end();vcpit++)
		{
			sumVpx += vcpit->x();
			sumVpy += vcpit->y();
			sumVpz += vcpit->z();
			sumVp++;
		}
	}

	double sumRingEx = 0;
	double sumRingEy = 0;
	double sumRingEz = 0;
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
		double bz = curedge.target().z();
		if (cemp_it != edge_points_map_temp.end())
		{
			_Cost v_cost = cemp_it->second;					
			for (auto cepit = v_cost.assined_points.begin(); cepit != v_cost.assined_points.end(); cepit++)		//当前边的上的每个点
			{
				double lamuda = Getlamuda(curedge, *cepit);
				double pix = cepit->x();
				double piy = cepit->y();
				double piz = cepit->z();
				sumRingEx += (1 - lamuda) * (pix - lamuda * bx);
				sumRingEy += (1 - lamuda) * (piy - lamuda * by);
				sumRingEz += (1 - lamuda) * (piz - lamuda * bz);
				sumRingElow += (1 - lamuda) * (1 - lamuda);
			}
		}

		Segment ceitwin = twin_edge(curedge);						//在Map里可能存的是twin，所以也要找一遍
		auto cemp_it1 = edge_points_map_temp.find(ceitwin);		
		double bx1 = ceitwin.source().x();		//反向的所以是source
		double by1 = ceitwin.source().y();
		double bz1 = ceitwin.source().z();
		if (cemp_it1 != edge_points_map_temp.end())
		{
			_Cost v_cost = cemp_it1->second;
			for (auto cepit = v_cost.assined_points.begin(); cepit != v_cost.assined_points.end(); cepit++)		//当前边的上的每个点
			{
				double lamuda = Getlamuda(curedge, *cepit);
				double pix = cepit->x();
				double piy = cepit->y();
				double piz = cepit->z();
				sumRingEx += (1 - lamuda) * (pix - lamuda * bx1);
				sumRingEy += (1 - lamuda) * (piy - lamuda * by1);
				sumRingEz += (1 - lamuda) * (piz - lamuda * bz1);
				sumRingElow += (1 - lamuda) * (1 - lamuda);
			}
		}


		
	}

	double resx = (sumVpx + sumRingEx) / (sumVp + sumRingElow);
	double resy = (sumVpy + sumRingEy) / (sumVp + sumRingElow);
	double resz = (sumVpz + sumRingEz) / (sumVp + sumRingElow);
	if(sumVp+sumRingElow==0)		//如果分母为0就代表没变，直接复制
	{
		resx = v.x();
		resy = v.y();
		resz = v.z();
	}

	Point returnp(resx, resy,resz);

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
		if(abs(x2 - x1) < 0.001)
		{
			x1 = ps.z();
			x2 = pt.z();
			xp = project.z();
		}
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
	
	//vector<vector<Point>::iterator> eitstore;


	//for (auto eit = assin_points.begin(); eit != assin_points.end(); ++eit)
	//{
	//	eitstore.push_back(eit);
	//}
	ms2.BuildSampleKDtree();
	const PointCloudAdaptor  pc2kd1(ms2.cloud);
	my_kd_tree_t   index1(3 /*dim*/, pc2kd1, KDTreeSingleIndexAdaptorParams(10/* max leaf */));
	index1.buildIndex();

	const size_t num_results = 1;
	size_t ret_index;
	double out_dist_sqr;
	nanoflann::KNNResultSet<double> resultSet(num_results);

	for (auto apit = assin_points.begin(); apit != assin_points.end(); apit++)
	{

		//auto apit = eitstore[i];
		Point pnow = *apit;

		double query_pt[3] = { pnow.x(), pnow.y(), pnow.z() };

		//const size_t num_results = 1;
		//size_t ret_index;
		//double out_dist_sqr;
		//nanoflann::KNNResultSet<double> resultSet(num_results);
		resultSet.init(&ret_index, &out_dist_sqr);
		index1.findNeighbors(resultSet, &query_pt[0], nanoflann::SearchParams(10));

		Segment nearest_edge = ms2.cloud.pts[ret_index].s;
		//double dist2 = out_dist_sqr;
		//Segment nearest_edge = ms2.FindNearestEdge(pnow,index1);


		//double dist2 = get_p_to_edge(pnow, nearest_edge);

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
	for (auto eeit=ms2.edges.begin();eeit!=ms2.edges.end();eeit++)
	{
		Segment ee = *eeit;
		Segment tee= twin_edge(ee);
		if(edge_points_map_temp.find(ee)==edge_points_map_temp.end()&& edge_points_map_temp.find(tee) == edge_points_map_temp.end())
		{
			_Cost cost;
			cost.assined_points.clear();
			edge_points_map_temp.insert(pair<Segment, _Cost>(ee, cost));
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
		edge_points_map_temp.find(*deit)->second.assined_points.clear();
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



