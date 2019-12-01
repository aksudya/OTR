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

	pri_cost = 0;
	iter_times = 0;


	tgl2 = tgl1;
	//int kkk = 0;
	CGAL::Random rng(99);
	for (int i = 0; i < 600; ++i)
	{
		
		int s = rng.get_int(0,tgl2.tds().number_of_edges()-5);
		for (auto eiter = tgl2.finite_edges_begin(); eiter != tgl2.finite_edges_end(); eiter++)
		{
			
			if(s!=0)
			{
				s--;
				continue;;
			}

			if (!IsBoderEdge(*eiter))
			{
				MakeCollap(*eiter);
				break;
			}
		}
	}

	//for (auto eiter = tgl2.finite_edges_begin(); eiter != tgl2.finite_edges_end(); eiter++)
	//{
	//	if (!IsBoderEdge(*eiter))
	//	{
	//		MakeCollap(*eiter);
	//		//kkk++;
	//		if (kkk >= 50)
	//		{
	//			break;
	//		}
	//	}
	//}

	tgl1 = tgl2;
}

void OTR::InitPriQueue()
{
	int kk = 0;
	for (auto eiter = tgl1.finite_edges_begin(); eiter != tgl1.finite_edges_end(); eiter++)
	{
		kk++;

		tgl2 = tgl1;

		Edge t1_edge = *eiter;
		Edge t2_edge=FindEdgeInTgl2(t1_edge);

		if (!IsBoderEdge(t2_edge))
		{
			MakeCollap(t2_edge);

			double cost = CaculateAssinCost();
			pri_queue_item pi;
			pi.half_edge = t1_edge;
			pi.cost = cost-pri_cost;
			half_edge_queue.push(pi);
			vertex_points_map_temp.clear();
			edge_points_map_temp.clear();

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
			pi1.cost = cost1- pri_cost;
			half_edge_queue.push(pi1);
			vertex_points_map_temp.clear();
			edge_points_map_temp.clear();


			assin_points.pop_back();
		}
	
	}
	tgl2 = tgl1;
	cout << kk<<" ";
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
	Edge fst_edge = half_edge_queue.top().half_edge;
	cout << half_edge_queue.top().cost << endl;
	Edge fst_edge_tgl2 = FindEdgeInTgl2(fst_edge);
	half_edge_queue.pop();
	vertex_handle vs = source_vertex(fst_edge_tgl2);
	vector<vertex_handle> one_ring_s = GetOneRingVertex(vs);

	//int kkk = 0;
	//for (auto eiter = tgl2.finite_edges_begin(); eiter != tgl2.finite_edges_end(); eiter++)
	//{
	//	if (!IsBoderEdge(*eiter))
	//	{
	//		MakeCollap(*eiter);
	//		kkk++;
	//		if(kkk>=80)
	//		{
	//			break;
	//		}
	//	}
	//}

	MakeCollap(fst_edge_tgl2);


	CaculateAssinCost();		//为了填充分配Map

	if (iter_times >= 2)		//90为第90次迭代之后使用relocate
	{
		ReLocate(one_ring_s, vs->point());


		for (int i = 0; i < 0; ++i)
		{
			vertex_points_map_temp.clear();
			edge_points_map_temp.clear();

			CaculateAssinCost();

			ReLocate(one_ring_s, vs->point());		//relocate 2次,效果会好一点
		}
		
	}

	vertex_points_map = vertex_points_map_temp;
	edge_points_map = edge_points_map_temp;			//为了显示
	vertex_points_map_temp.clear();
	edge_points_map_temp.clear();


	pri_cost = CaculateAssinCost();	


	vertex_points_map_temp.clear();
	edge_points_map_temp.clear();

	tgl1 = tgl2;
	while (!half_edge_queue.empty())		//把优先队列清空
	{
		half_edge_queue.pop();
	}
	InitPriQueue();
	cout << iter_times << " ";


}

void OTR::ReLocate(vector<vertex_handle> ring,Point sp)
{
	int rvid = 0;
	vector<pair<vertex_handle,Point>> toMove;
	for (auto rvit=ring.begin();rvit!=ring.end();rvit++)
	{

		if (IsBoderPoint((*rvit)->point()))			//跳过边界点
		{
			rvid++;
			continue;
		}

		Point AfterRelocate = Relocatev(*rvit);
		vector<vertex_handle> one_ring_v = GetOneRingVertex(*rvit);
		if(PointIsInRing(one_ring_v, AfterRelocate))
		{
			(*rvit)->point() = AfterRelocate;		//移动点
		}
		rvid++;
	}
}

bool OTR::PointIsInRing(vector<vertex_handle> ring, Point sp)
{
	bool isIn = true;
	for (int i = 0; i < ring.size(); ++i)
	{
		Point p1 = ring[i]->point();
		int nexti = (i + 1) % (int)ring.size();
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
		Edge curedge = *ceiter;
		if(source_vertex(curedge)!=v)		//确保方向是正确的
		{
			curedge = twin_edge(curedge);
		}

		auto cemp_it = edge_points_map_temp.find(curedge);
		double bx = target_vertex(curedge)->point().x();
		double by= target_vertex(curedge)->point().y();
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

		Edge ceitwin = twin_edge(curedge);						//在Map里可能存的是twin，所以也要找一遍
		auto cemp_it1 = edge_points_map_temp.find(ceitwin);		
		double bx1 = source_vertex(ceitwin)->point().x();		//反向的所以是source
		double by1 = source_vertex(ceitwin)->point().y();
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


		++ceiter;
	}

	double resx = (sumVpx + sumRingEx) / (sumVp + sumRingElow);
	double resy = (sumVpy + sumRingEy) / (sumVp + sumRingElow);

	if(sumVp+sumRingElow==0)		//如果分母为0就代表没变，直接复制
	{
		resx = v->point().x();
		resy = v->point().y();
	}

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

void OTR::MakeCollap(Edge& e)
{
	assin_points.push_back(source_vertex(e)->point());
	while (!IsCollapsable(e))
	{
		FlipEdge(e);
		block_edge.clear();
	}
	Edge t_edge = twin_edge(e);
	tgl2.tds().join_vertices(t_edge.first, t_edge.second);

	block_edge.clear();

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

		double dist2 = get_p_to_edge(*apit, nearest_edge);

		Edge twinn = twin_edge(nearest_edge);
		auto iter = edge_points_map_temp.find(nearest_edge);		//找最近的那条边e
		auto itert= edge_points_map_temp.find(twinn);				//找那条边的twine

		if (iter != edge_points_map_temp.end())		//如果e已经在map里了
		{
			auto find_it = find(iter->second.assined_points.begin(), iter->second.assined_points.end(), *apit);
			if(find_it== iter->second.assined_points.end())
			{
				iter->second.assined_points.push_back(*apit);
			}
		}	
		else		//如果e没有在map里
		{
			if(itert!=edge_points_map_temp.end())		//如果twine在map里
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
				edge_points_map_temp.insert(pair<Edge, _Cost>(nearest_edge,cost));
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



void OTR::AssinToVertex(Edge e, _Cost& c)
{
	const Point ps = source_vertex(e)->point();
	const Point pt = target_vertex(e)->point();
	for (auto pit = c.assined_points.begin(); pit != c.assined_points.end(); pit++)
	{
		double dists = squared_distance(*pit, ps);
		double distt = squared_distance(*pit, pt);

		if (IsBoderPoint(ps))		//特判边界点
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
				
			}
			continue;
		}
		else if (IsBoderPoint(pt))
		{
			auto iter = vertex_points_map_temp.find(source_vertex(e));

			if (iter != vertex_points_map_temp.end())
			{

				iter->second.assined_points.push_back(*pit);			
			}
			else
			{
				_Cost cost;
				cost.assined_points.push_back(*pit);
				vertex_points_map_temp.insert(pair<vertex_handle, _Cost>(source_vertex(e), cost));
			}
			continue;
		}

		if (dists < distt)	//分配给s
		{
			auto iter = vertex_points_map_temp.find(source_vertex(e));

			if (iter != vertex_points_map_temp.end())
			{
					iter->second.assined_points.push_back(*pit);			
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

double OTR::PointProjectToSource(Edge e, Point p)
{
	const Point ps = source_vertex(e)->point();
	const Point pt = target_vertex(e)->point();
	Segment seg(ps, pt);
	Line le = seg.supporting_line();
	Point project = le.projection(p);
	double dist = sqrt(squared_distance(project, ps));
	return dist;
}

void OTR::CaculateTangentialCost(Edge e, _Cost& c)
{
	double tangential_cost = 0;
	double centerCord;		//每段中点的坐标
	const Point ps = source_vertex(e)->point();
	const Point pt = target_vertex(e)->point();
	
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

		if(IsBoderPoint(ps))		//边界点
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



Edge OTR::find_nearest_edge(Face_handle f, Point p)
{
	Edge nearest(f, 0);
	double min_dist2 =DBL_MAX;
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
