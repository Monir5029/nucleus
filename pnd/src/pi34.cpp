#include "main.h"

inline void print_Ks (vector<triangle_id>& tlist, volatile vertex* F, const char* vfile) {
	string st (vfile);
	st += "_FINAL_H_values";
	FILE* pp = fopen (st.c_str(), "w");
	for (int i = 0; i < tlist.size(); i++)
		fprintf (pp, "%d %d %d   %d\n", get<0>(tlist[i].triple), get<1>(tlist[i].triple), get<2>(tlist[i].triple), F[i]);
	fclose (pp);
}

inline void print_Hs (vector<triangle_id>& tlist, volatile vertex* F, const char* vfile, int oc) {
	string st (vfile);
	st += "_H_values_" + to_string (oc);
	FILE* pp = fopen (st.c_str(), "w");
	for (int i = 0; i < tlist.size(); i++)
		fprintf (pp, "%d\n", F[i]);
	fclose (pp);
}

inline int minof (int a, int b, int c) {
	int tm = (a < b) ? a : b;
	return (tm < c) ? tm : c;
}

inline void reverse_intersect (Graph& graph, Graph& ordered_graph,vertex u, vertex v, vector<vertex>& intersection) {
	size_t i = 0, j = 0;
	while (i < graph[u].size() && j < graph[v].size()) {
		if (graph[u][i] < graph[v][j])
			i++;
		else if (graph[v][j] < graph[u][i])
			j++;
		else {
			int w = graph[u][i];
			if (w == smaller (graph, w, u) && w == smaller (graph, w, v))
				intersection.push_back(w);
			i++;
			j++;
		}
	}
}

void superTriangleList (Graph& graph, Graph& ordered_graph, vector<int>& xel, vector<int>& xtl, EdgeList2& el, vector<triangle_id>& tl, Graph& TF) {

	int co = 0;
#pragma omp parallel for
	for (size_t i = 0; i < el.size(); i++) { // u < v
		int u = get<0>(el[i]);
		int v = get<1>(el[i]);
		vector<vertex> is;
		intersect_2 (ordered_graph, u, v, is);
		for (size_t j = 0; j < is.size(); j++) { // u < v < w
			int wcand = is[j];
			for (size_t k = j+1; k < is.size(); k++) { // u < v < x
				int xcand = is[k];
				int w = wcand, x = xcand;
				if (x == smaller (graph, w, x)) {
					int temp = w;
					w = x;
					x = temp;
				}
				if (find_ind (graph, ordered_graph, w, x) != -1) {
					co++;
					vertex in1 = fast_getTriId (u, v, w, xel, xtl, tl, ordered_graph, graph);
					vertex in2 = fast_getTriId (u, v, x, xel, xtl, tl, ordered_graph, graph);
					vertex in3 = fast_getTriId (u, w, x, xel, xtl, tl, ordered_graph, graph);
					vertex in4 = fast_getTriId (v, w, x, xel, xtl, tl, ordered_graph, graph);
					TF[in1].push_back (in2); TF[in1].push_back (in3); TF[in1].push_back (in4);
					TF[in2].push_back (in1); TF[in2].push_back (in3); TF[in2].push_back (in4);
				}
			}
		}
	}

#pragma omp parallel for
	for (size_t i = 0; i < el.size(); i++) { // x < w
		int x = get<0>(el[i]);
		int w = get<1>(el[i]);
		vector<vertex> is;
		reverse_intersect (graph, ordered_graph, x, w, is);
		for (size_t j = 0; j < is.size(); j++) { // u < x < w
			int ucand = is[j];
			for (size_t k = j+1; k < is.size(); k++) { // v < x < w
				int vcand = is[k];
				int u = ucand, v = vcand;
				if (v == smaller (graph, u, v)) {
					int temp = u;
					u = v;
					v = temp;
				}
				if (find_ind (graph, ordered_graph, u, v) != -1) {
					co++;
					vertex in1 = fast_getTriId (u, x, w, xel, xtl, tl, ordered_graph, graph);
					vertex in2 = fast_getTriId (v, x, w, xel, xtl, tl, ordered_graph, graph);
					vertex in3 = fast_getTriId (u, v, w, xel, xtl, tl, ordered_graph, graph);
					vertex in4 = fast_getTriId (u, v, x, xel, xtl, tl, ordered_graph, graph);
					TF[in1].push_back (in2); TF[in1].push_back (in3); TF[in1].push_back (in4);
					TF[in2].push_back (in1); TF[in2].push_back (in3); TF[in2].push_back (in4);
				}
			}
		}
	}

	return;
}

bool herfunction (int i, int j) {
	return i > j;
}

inline void compute_KT_EDA (Graph& graph, vector<triangle_id>& tlist, vector<vertex>& Reals, vertex* F, int oc, vertex* Nxel, vertex* Nxtl, triangle_id* Ntlist, vertex** Nordered_graph, vertex* Nsizeordered_graph, vertex** Ngraph, vertex* Nsizegraph) {

	int count = 0, score = 0;
	for (vertex i = 0; i < tlist.size(); i++) {
		int a = get<0>(tlist[i].triple);
		int b = get<1>(tlist[i].triple);
		int c = get<2>(tlist[i].triple);

		vector<int> is;
		intersect3_new (graph, a, b, c, is);

		for (vertex j = 0; j < is.size(); j++) {
			int d = is[j];
			int tusubasa = i;// 		MyNgetTriId (a, b, c, Nxel, Nxtl, Ntlist, Nordered_graph, Nsizeordered_graph, Ngraph, Nsizegraph);
			int misaki = 		MyNgetTriId (a, b, d, Nxel, Nxtl, Ntlist, Nordered_graph, Nsizeordered_graph, Ngraph, Nsizegraph);
			int wakabayashi = 	MyNgetTriId (a, c, d, Nxel, Nxtl, Ntlist, Nordered_graph, Nsizeordered_graph, Ngraph, Nsizegraph);
			int erdem = 		MyNgetTriId (b, c, d, Nxel, Nxtl, Ntlist, Nordered_graph, Nsizeordered_graph, Ngraph, Nsizegraph);
			FourCliqueKendallTau (Reals, F, tusubasa, misaki ,wakabayashi, erdem, &count, &score);
		}
	}

	printf ("H %d , KendallTau: %lf\t", oc, kt (count, score));
}

inline void simple_distance (vector<vertex>& Reals, vertex* F, int oc) {
	double score = 0;
	int count = 0;
	for (vertex i = 0; i < Reals.size(); i++)
		if (F[i] > 0 && Reals[i] > 0) {
//			printf ("sc: %lf\n", (double) Reals[i] / F[i]);
//			score += (double) Reals[i] / F[i];
			if (Reals[i] == F[i])
				score++;

			count++;
		}

	score /= count;

	printf ("H %d , similarity: %lf\n", oc, score);
}

// Kendall-Tau -- each 4clique is counted 4 times but that's fine for final KT
inline void compute_KT (vector<triangle_id>& tlist, vector<vertex>& Reals, Graph& TF, vertex* F, int oc) {

	int count = 0, score = 0;
	for (vertex i = 0; i < tlist.size(); i++) {
		int a = get<0>(tlist[i].triple);
		int b = get<1>(tlist[i].triple);
		int c = get<2>(tlist[i].triple);

		for (vertex j = 0; j < TF[i].size(); j+=3) {
			int u = TF[i][j];
			int d = get<0>(tlist[u].triple);
			int e = get<1>(tlist[u].triple);
			int f = get<2>(tlist[u].triple);

			int che;
			if (d != a && d != b && d != c)
				che = d;
			else if (e != a && e != b && e != c)
				che = e;
			else if (f != a && f != b && f != c)
				che = f;

			FourCliqueKendallTau (Reals, F, a, b, c, che, &count, &score);
		}
	}

	printf ("H %d , KendallTau: %lf\t", oc, kt (count, score));
}


// stores triangle - 4clique graph
void base_pi34 (Graph& graph, int nEdge, vertex* F, vector<vertex>& Reals, util::timestamp& totaltime, vertex* threefour_number, const char* vfile, FILE* fp) {
	util::timestamp t_begin;
	char *p = (char*) &totaltime;
	util::timestamp *pout = (util::timestamp*) p;

	// Ordered (directed) graph creation
	EdgeList2 el;
	vector<vertex> xel;
	Graph ordered_graph;
	create_ordered_graph (graph, el, xel, ordered_graph);

	util::timestamp t_cog;
	cout << "creating ordered graph: " << t_cog - t_begin << endl;

	vector<triangle_id> tlist;
	vector<vertex> xtl;
	create_classic_triangleList (ordered_graph, el, tlist, xtl); // not parallel, like ordered_graph creation in pi23

	util::timestamp ttl;
	cout << "create triangle list: " << ttl - t_begin << endl;

	// 4-clique counting & enumerating for each triangle
	F = (vertex *) malloc (sizeof(vertex) * tlist.size());
#ifndef ASYNC
	vertex* G = (vertex *) malloc (sizeof(vertex) * tlist.size());
#endif

	Graph TF (tlist.size()); // triangle's neigs in each 4-clique, final size is 4 * #4-cliques
	superTriangleList (graph, ordered_graph, xel, xtl, el, tlist, TF);

#pragma omp parallel for
	for (int i = 0; i < TF.size(); i++)
		F[i] = TF[i].size() / 3;

	util::timestamp t_4c;
	cout << "4c count, TF construction: " << t_4c - t_begin << endl;


	util::timestamp td (0, 0);
	int oc = 0;
	bool flag = true;

	while (flag) {
		flag = false;
		util::timestamp td1;
#ifdef DEBUG_00
		// H values for each triangle
		print_Hs (tlist, F, vfile, oc);
#endif

#ifdef DEBUG_0
		compute_KT (tlist, Reals, TF, F, oc);
#endif

		util::timestamp td2;
		td += td2 - td1;
		util::timestamp t_end;
		cout << "H_" << oc << " time: " << t_end - t_begin - td << endl;

#pragma omp parallel for default (shared)
		for (vertex ind = 0; ind < TF.size(); ind++) {
			if (oc == 0) {
				vector<int> counts;
				for (vertex j = 0; j < TF[ind].size(); j+=3) {
					int u = TF[ind][j];
					int v = TF[ind][j+1];
					int w = TF[ind][j+2];
					int sm = minof (F[u], F[v], F[w]);
					counts.push_back (sm);
				}
				sort (counts.begin(), counts.end(), herfunction);
				int j;
				for (j = 0; j < counts.size(); j++)
					if (counts[j] < j+1)
						break;
#ifdef ASYNC
				if (F[ind] != j) {
					F[ind] = j;
					flag = true;
				}
#else
				if (G[ind] != j) {
					G[ind] = j;
					flag = true;
				}
#endif
			}
			else {
				if (F[ind] == 0)
					continue;
				int previous_F = F[ind];
				int greaterequals = 0;
				vector<int> smallers (previous_F, 0);
				bool yep_set = false;

				for (int j = 0; j < TF[ind].size(); j+=3) {
					int u = TF[ind][j];
					int v = TF[ind][j+1];
					int w = TF[ind][j+2];
					int sm = minof (F[u], F[v], F[w]);
					if (sm >= previous_F)
						greaterequals++;
					else
						smallers[sm]++;
					if (greaterequals == previous_F) {
						yep_set = true;
						break;
					}
				}

				if (!yep_set) {
					int newF = 0;
					for (int j = previous_F - 1; j > 0; j--) {
						greaterequals += smallers[j];
						if (greaterequals >= j) {
							newF = j;
							break;
						}
					}
#ifdef ASYNC
					if (F[ind] != newF) {
						F[ind] = newF;
						flag = true;
					}
#else
					if (G[ind] != newF) {
						G[ind] = newF;
						flag = true;
					}
#endif
				}
			}
		}
#ifndef ASYNC
		memcpy (F, G, sizeof(vertex) * tlist.size());
#endif
		oc++;
	}

	util::timestamp t_end;
	cout << "H_" << oc << " time: " << t_end - t_begin - td << endl;
	cout << "Converges at " <<  oc << endl;

#ifdef DEBUG_00
	// H values for each triangle
	print_Hs (tlist, F, vfile, oc);
#endif

#ifdef DEBUG_0
	compute_KT (tlist, Reals, TF, F, oc);
	cout << endl;
#endif

//	print_Ks (tlist, F, vfile);
	free (F);
	for (vertex i = 0; i < ordered_graph.size(); ++i)
		ordered_graph[i].clear();
	ordered_graph.clear();
	el.clear();
	xel.clear();
	for (vertex i = 0; i < TF.size(); ++i)
		TF[i].clear();
	TF.clear();
	return;
}



inline void updateAndNotify (int ind, volatile vertex* P, int newP, vector<vertex>& neigs, bool* changed) {
	P[ind] = newP;
	changed[ind] = true; // *THIS*
	for (int k = 0; k < neigs.size(); k++) {
		if (P[neigs[k]] >= P[ind]) {
			changed[neigs[k]] = true;
		}
	}
}


inline int quick_h_index (int ind, int* Nsizegraph, int** Ngraph, triangle_id* Ntlist, int* Nxel, vertex* Nxtl, int** Nordered_graph, int* Nsizeordered_graph, volatile vertex* P, bool* changed) {

	int nP, sm;
	vector<vertex> neigs;
	int previous_P = P[ind];
	int greaterequals = 0;
	vector<int> smallers (previous_P, 0);
	bool yep_set = false;
	vertex u = get<0> (Ntlist[ind].triple);
	vertex v = get<1> (Ntlist[ind].triple);
	vertex w = get<2> (Ntlist[ind].triple);

	size_t ii = 0, jj = 0, kk = 0;
	vertex u_size = Nsizegraph[u];
	vertex v_size = Nsizegraph[v];
	vertex w_size = Nsizegraph[w];

	bool has4Clique = false;
	while (ii < u_size && jj < v_size && kk < w_size) {
		vertex a = Ngraph[u][ii];
		vertex b = Ngraph[v][jj];
		vertex c = Ngraph[w][kk];

		if (a == b && a == c) {
			has4Clique = true;
			vertex x = a;
			vertex id1 = NgetTriId (u, v, x, Nxel, Nxtl, Ntlist, Nordered_graph, Nsizeordered_graph, Ngraph, Nsizegraph);
			vertex id2 = NgetTriId (u, w, x, Nxel, Nxtl, Ntlist, Nordered_graph, Nsizeordered_graph, Ngraph, Nsizegraph);
			vertex id3 = NgetTriId (v, w, x, Nxel, Nxtl, Ntlist, Nordered_graph, Nsizeordered_graph, Ngraph, Nsizegraph);

			int p1 = P[id1];
			int p2 = P[id2];
			int p3 = P[id3];
			nP = sm = minof (p1, p2, p3);

//			if (nP <= previous_P) { // *OLD THIS*
//				neigs.push_back (id1);
//				neigs.push_back (id2);
//				neigs.push_back (id3);
//			}
//

			if (p1 <= previous_P)
				neigs.push_back (id1);
			if (p2 <= previous_P)
				neigs.push_back (id2);
			if (p3 <= previous_P)
				neigs.push_back (id3);

			if (sm >= previous_P)
				greaterequals++;
			else
				smallers[sm]++;
			if (greaterequals == previous_P) {
				yep_set = true;
				break;
			}
			ii++; jj++; kk++;
		}
		else {
			vertex m = max3_old(a, b, c);
			if (a != m)
				ii++;
			if (b != m)
				jj++;
			if (c != m)
				kk++;
		}
	}

	if (!yep_set && has4Clique) {
		int H = 0;
		for (int j = previous_P - 1; j > 0; j--) {
			greaterequals += smallers[j];
			if (greaterequals >= j) {
				H = j;
				break;
			}
		}
		updateAndNotify (ind, P, H, neigs, changed);
		return 1;
	}
	return 0;
}

inline int initial_h_index (int ind, int* Nsizegraph, int** Ngraph, triangle_id* Ntlist, int* Nxel, vertex* Nxtl, int** Nordered_graph, int* Nsizeordered_graph, volatile vertex* P) {

	HashMap<int> gmap (INT_MIN);
	int greaters = 0;
	int equals = 0;
	int H = 0;
	vertex u = get<0> (Ntlist[ind].triple);
	vertex v = get<1> (Ntlist[ind].triple);
	vertex w = get<2> (Ntlist[ind].triple);

	int ii = 0, jj = 0, kk = 0;
	vertex u_size = Nsizegraph[u];
	vertex v_size = Nsizegraph[v];
	vertex w_size = Nsizegraph[w];

	while (ii < u_size && jj < v_size && kk < w_size) {
		vertex a = Ngraph[u][ii];
		vertex b = Ngraph[v][jj];
		vertex c = Ngraph[w][kk];
		if (a == b && a == c) {
			vertex x = a;
			vertex id1 = NgetTriId (u, v, x, Nxel, Nxtl, Ntlist, Nordered_graph, Nsizeordered_graph, Ngraph, Nsizegraph);
			vertex id2 = NgetTriId (u, w, x, Nxel, Nxtl, Ntlist, Nordered_graph, Nsizeordered_graph, Ngraph, Nsizegraph);
			vertex id3 = NgetTriId (v, w, x, Nxel, Nxtl, Ntlist, Nordered_graph, Nsizeordered_graph, Ngraph, Nsizegraph);
			int sm = minof (P[id1], P[id2], P[id3]);

			if (sm == H + 1) {
				if (equals > 0) {
					equals--;
					greaters++;
					gmap[sm]++;
				}
				else { // equals = 0
					H++;
					int gH = 0;
					if (!gmap.hasDefaultValue (H))
						gH = gmap[H];
					equals = gH + 1;
					greaters -= gH;
					gmap.erase (H);
				}
			}
			else if (sm > H + 1) {
				if  (equals > 0) {
					equals--;
					greaters++;
					gmap[sm]++;
				}
				else { // equals = 0
					greaters++;
					H++;
					int gH = 0;
					if (!gmap.hasDefaultValue (H))
						gH = gmap[H];
					equals = gH;
					greaters -= gH;
					gmap[sm]++;
					gmap.erase (H);
				}
			}
			ii++;
			jj++;
			kk++;
		}
		else {
			vertex m = max3_old(a, b, c);
			if (a != m)
				ii++;
			if (b != m)
				jj++;
			if (c != m)
				kk++;
		}
	}

	int oP = P[ind];
	P[ind] = H;
	if (H < oP) {
		return 1;
	}
	else
		return 0;
}






inline int regular_h_index (int ind, int* Nsizegraph, int** Ngraph, triangle_id* Ntlist, int* Nxel, vertex* Nxtl, int** Nordered_graph, int* Nsizeordered_graph, volatile vertex* P) {

	int previous_P = P[ind];
	int nP, sm;
	int greaterequals = 0;
	vector<int> smallers (previous_P, 0);
	bool yep_set = false;

	vertex u = get<0> (Ntlist[ind].triple);
	vertex v = get<1> (Ntlist[ind].triple);
	vertex w = get<2> (Ntlist[ind].triple);

	size_t ii = 0, jj = 0, kk = 0;
	vertex u_size = Nsizegraph[u];
	vertex v_size = Nsizegraph[v];
	vertex w_size = Nsizegraph[w];

	bool has4Clique = false;
	while (ii < u_size && jj < v_size && kk < w_size) {
		vertex a = Ngraph[u][ii];
		vertex b = Ngraph[v][jj];
		vertex c = Ngraph[w][kk];

		if (a == b && a == c) {
			has4Clique = true;
			vertex x = a;
			vertex id1 = NgetTriId (u, v, x, Nxel, Nxtl, Ntlist, Nordered_graph, Nsizeordered_graph, Ngraph, Nsizegraph);
			vertex id2 = NgetTriId (u, w, x, Nxel, Nxtl, Ntlist, Nordered_graph, Nsizeordered_graph, Ngraph, Nsizegraph);
			vertex id3 = NgetTriId (v, w, x, Nxel, Nxtl, Ntlist, Nordered_graph, Nsizeordered_graph, Ngraph, Nsizegraph);

			int p1 = P[id1];
			int p2 = P[id2];
			int p3 = P[id3];
			nP = sm = minof (p1, p2, p3);

			if (sm >= previous_P)
				greaterequals++;
			else
				smallers[sm]++;
			if (greaterequals == previous_P) {
				yep_set = true;
				break;
			}
			ii++; jj++; kk++;
		}
		else {
			vertex m = max3_old(a, b, c);
			if (a != m)
				ii++;
			if (b != m)
				jj++;
			if (c != m)
				kk++;
		}
	}

	if (!yep_set && has4Clique) {
		int H = 0;
		for (int j = previous_P - 1; j > 0; j--) {
			greaterequals += smallers[j];
			if (greaterequals >= j) {
				H = j;
				break;
			}
		}

		if (P[ind] != H) {
			P[ind] = H;
			return 1;
		}
		return 1;
	}
	return 0;
}





void TryFasterPi34 (Graph& graph, int nEdge, volatile vertex* P, vector<vertex>& Reals, util::timestamp& totaltime, vertex* threefour_number, const char* vfile, FILE* fp, string fname) {

	timestamp ts0minus;

	int ngraphsize = graph.size();
	int* Nsizegraph = (int *) malloc (sizeof (int) * graph.size());
	int** Ngraph = (int **) malloc (sizeof (int*) * graph.size());
	for (int i = 0; i < graph.size(); i++) {
		Ngraph[i] = (int *) malloc (sizeof (int) * graph[i].size());
		for (int j = 0; j < graph[i].size(); j++)
			Ngraph[i][j] = graph[i][j];
		Nsizegraph[i] = graph[i].size();
	}

	timestamp ts0minus_end;

	EdgeList2 el;
	vector<vertex> xel;
	Graph ordered_graph;
	create_ordered_graph (graph, el, xel, ordered_graph);


	timestamp ts1minus;
	tuple<int, int>* Nel = (tuple<int, int> *) malloc (sizeof(tuple<int, int>) * el.size());
	int Nelsize = el.size();
	for (int i = 0; i < el.size(); i++)
		Nel[i] = el[i];

	int* Nxel = (int *) malloc (sizeof(int) * xel.size());
	int Nxelsize = xel.size();
	for (int i = 0; i < xel.size(); i++)
		Nxel[i] = xel[i];


	int nordered_graphsize = ordered_graph.size();
	int* Nsizeordered_graph = (int *) malloc (sizeof (int) * ordered_graph.size());
	int** Nordered_graph = (int **) malloc (sizeof (int*) * ordered_graph.size());
	for (int i = 0; i < ordered_graph.size(); i++) {
		Nordered_graph[i] = (int *) malloc (sizeof (int) * ordered_graph[i].size());
		for (int j = 0; j < ordered_graph[i].size(); j++)
			Nordered_graph[i][j] = ordered_graph[i][j];
		Nsizeordered_graph[i] = ordered_graph[i].size();
	}

	timestamp ts1minus_end;

	timestamp ts0;
	cout << "before iterations: " << ts0 - ts0minus << endl;

	vector<triangle_id> tlist;
	vector<vertex> xtl;
	create_classic_triangleList (ordered_graph, el, tlist, xtl); // not parallel, like ordered_graph creation in pi23

	int Nxtlsize = xtl.size();
	vertex* Nxtl = (vertex *) malloc (sizeof (vertex) * xtl.size());
	for (int i = 0; i < xtl.size(); i++)
		Nxtl[i] = xtl[i];

	timestamp ts01;
	cout << "create triangle list: " << ts01 - ts0 << endl;

	// 4-clique counting
	P = (vertex *) malloc (sizeof(vertex) * tlist.size());
	long fccount = intersect3_count_classic_efficient (graph, ordered_graph, el, xel, xtl, tlist); // todo: can think about convert STLs to pointers

	timestamp ts2minus;
	int Ntlistsize = tlist.size();
	triangle_id* Ntlist = (triangle_id *) malloc (sizeof (triangle_id) * tlist.size());
	for (int i = 0; i < tlist.size(); i++) {
		Ntlist[i].id = tlist[i].id;
		Ntlist[i].triple = tlist[i].triple;
	}
	timestamp ts2minus_end;

	int nt = 1, tn = 0;
#pragma omp parallel
	{
		nt = omp_get_num_threads();
		tn = omp_get_thread_num();

#pragma omp for
		for (int i = 0; i < Ntlistsize; i++)
			P[i] = Ntlist[i].id;
	}

	timestamp ts1;
	cout << "Four clique counting: " << (ts1 - ts01) << endl;

	int* ccs = (int *) calloc (nt, sizeof(int));
	int* nextBatch = (int *) malloc (sizeof(int) * Ntlistsize);
	int oc = 0;
//	print_Hs (tlist, P, vfile, oc);
	oc++;
	timestamp ts2;

#pragma omp parallel for schedule (dynamic, 1000)
	for (int ind = 0; ind < Ntlistsize; ind++) {
		ccs[tn] += initial_h_index (ind, Nsizegraph, Ngraph, Ntlist, Nxel, Nxtl, Nordered_graph, Nsizeordered_graph, P);
	}
	timestamp ts3;


	timestamp ts3minus;
	int degisenler = 0;
	for(int i = 0; i < nt; i++)
		degisenler += ccs[i];
	timestamp ts3minus_end;

	int nb = 0;
	for (int ind = 0; ind < Ntlistsize; ind++)
		nextBatch[nb++] = ind;

	bool changed[Ntlistsize];
	memset (changed, 255, sizeof(bool) * Ntlistsize); // set all true
	memset (ccs, 0, nt * sizeof(int));
	timestamp ts4;


	cout << "CHANGEDS: " << degisenler << " H_0 par time: " << (ts3 - ts2) << "   seq time: " << ts4 - ts3 << "  tu: " << Ntlistsize << endl;


	timestamp td (0, 0);
	while (nb > 0) {
//		if (oc == 4)
//			print_Hs (tlist, P, vfile, oc);
 		timestamp it1;
#pragma omp parallel for schedule (dynamic, 1000)
		for (int i = 0; i < nb; i++) {
			int ind = nextBatch[i];
			changed[ind] = false;
			ccs[tn] += quick_h_index (ind, Nsizegraph, Ngraph, Ntlist, Nxel, Nxtl, Nordered_graph, Nsizeordered_graph, P, changed);
		}
		timestamp it2;
		int onb = 0;
		for (int ind = 0; ind < Ntlistsize; ind++) {
			if (changed[ind])
				nextBatch[onb++] = ind;
		}
		nb = onb;

		timestamp t0;
		int degisenler = 0;
		for(int i = 0; i < nt; i++)
			degisenler += ccs[i];
		timestamp t1;
		td += t1 - t0;


		memset (ccs, 0, nt * sizeof(int));

		timestamp it3;
		cout << "CHANGEDS: " << degisenler << "  H_" << oc << " par time: " << it2 - it1 << "   seq time: " << it3 - it2 << "   tu: " << nb << endl;
		oc++;
	}

	util::timestamp ts6;
	cout << "Total time: " << ts6 - ts0minus - ((ts0minus_end - ts0minus) + (ts1minus_end - ts1minus) + (ts2minus_end - ts2minus) + (ts3minus_end - ts3minus) + td) << endl;

//	print_Ks (tlist, P, vfile);
	return;

}






void justpi34 (Graph& graph, int nEdge, volatile vertex* P, vector<vertex>& Reals, util::timestamp& totaltime, vertex* threefour_number, const char* vfile, FILE* fp, string fname) {

	timestamp ts0minus;

	int ngraphsize = graph.size();
	int* Nsizegraph = (int *) malloc (sizeof (int) * graph.size());
	int** Ngraph = (int **) malloc (sizeof (int*) * graph.size());
	for (int i = 0; i < graph.size(); i++) {
		Ngraph[i] = (int *) malloc (sizeof (int) * graph[i].size());
		for (int j = 0; j < graph[i].size(); j++)
			Ngraph[i][j] = graph[i][j];
		Nsizegraph[i] = graph[i].size();
	}

	timestamp ts0minus_end;

	EdgeList2 el;
	vector<vertex> xel;
	Graph ordered_graph;
	create_ordered_graph (graph, el, xel, ordered_graph);


	timestamp ts1minus;
	tuple<int, int>* Nel = (tuple<int, int> *) malloc (sizeof(tuple<int, int>) * el.size());
	int Nelsize = el.size();
	for (int i = 0; i < el.size(); i++)
		Nel[i] = el[i];

	int* Nxel = (int *) malloc (sizeof(int) * xel.size());
	int Nxelsize = xel.size();
	for (int i = 0; i < xel.size(); i++)
		Nxel[i] = xel[i];


	int nordered_graphsize = ordered_graph.size();
	int* Nsizeordered_graph = (int *) malloc (sizeof (int) * ordered_graph.size());
	int** Nordered_graph = (int **) malloc (sizeof (int*) * ordered_graph.size());
	for (int i = 0; i < ordered_graph.size(); i++) {
		Nordered_graph[i] = (int *) malloc (sizeof (int) * ordered_graph[i].size());
		for (int j = 0; j < ordered_graph[i].size(); j++)
			Nordered_graph[i][j] = ordered_graph[i][j];
		Nsizeordered_graph[i] = ordered_graph[i].size();
	}

	timestamp ts1minus_end;

	timestamp ts0;
	cout << "before iterations: " << ts0 - ts0minus << endl;

	vector<triangle_id> tlist;
	vector<vertex> xtl;
	create_classic_triangleList (ordered_graph, el, tlist, xtl); // not parallel, like ordered_graph creation in pi23
	int Nxtlsize = xtl.size();
	vertex* Nxtl = (vertex *) malloc (sizeof (vertex) * xtl.size());
	for (int i = 0; i < xtl.size(); i++)
		Nxtl[i] = xtl[i];

	timestamp ts01;
	cout << "create triangle list: " << ts01 - ts0 << endl;

	// 4-clique counting
	P = (vertex *) malloc (sizeof(vertex) * tlist.size());
	long fccount = intersect3_count_classic_efficient (graph, ordered_graph, el, xel, xtl, tlist); // todo: can think about convert STLs to pointers

	timestamp ts2minus;
	int Ntlistsize = tlist.size();
	triangle_id* Ntlist = (triangle_id *) malloc (sizeof (triangle_id) * tlist.size());
	for (int i = 0; i < tlist.size(); i++) {
		Ntlist[i].id = tlist[i].id;
		Ntlist[i].triple = tlist[i].triple;
	}
	timestamp ts2minus_end;

	int nt = 1, tn = 0;
#pragma omp parallel
	{
		nt = omp_get_num_threads();
		tn = omp_get_thread_num();

#pragma omp for
		for (int i = 0; i < Ntlistsize; i++)
			P[i] = Ntlist[i].id;
	}

	timestamp ts1;
	cout << "Four clique counting: " << (ts1 - ts01) << endl;

//	int* ccs = (int *) calloc (nt, sizeof(int));
//	int* nextBatch = (int *) malloc (sizeof(int) * Ntlistsize);
	int oc = 0;
//	print_Hs (tlist, P, vfile, oc);
	oc++;
	timestamp ts2;

//	int oc = 0;
	bool flag = true;

#pragma omp parallel for schedule (dynamic, 1000)
	for (int ind = 0; ind < Ntlistsize; ind++) {
		initial_h_index (ind, Nsizegraph, Ngraph, Ntlist, Nxel, Nxtl, Nordered_graph, Nsizeordered_graph, P);
	}
	timestamp ts3;


	timestamp ts3minus;
//	int degisenler = 0;
//	for(int i = 0; i < nt; i++)
//		degisenler += ccs[i];
	timestamp ts3minus_end;

//	int nb = 0;
//	for (int ind = 0; ind < Ntlistsize; ind++)
//		nextBatch[nb++] = ind;

//	bool changed[Ntlistsize];
//	memset (changed, 255, sizeof(bool) * Ntlistsize); // set all true
//	memset (ccs, 0, nt * sizeof(int));
	timestamp ts4;


//	cout << "CHANGEDS: " << degisenler << " H_0 par time: " << (ts3 - ts2) << "   seq time: " << ts4 - ts3 << "  tu: " << Ntlistsize << endl;


	timestamp td (0, 0);
//	while (nb > 0) {
	while (flag) {
		flag = false;
 		timestamp it1;
#pragma omp parallel for schedule (dynamic, 1000)
		for (int i = 0; i < Ntlistsize; i++) {
			int ind = i;
			int fl = regular_h_index (ind, Nsizegraph, Ngraph, Ntlist, Nxel, Nxtl, Nordered_graph, Nsizeordered_graph, P);
			if (fl == 1)
				flag = true;
		}
		timestamp it2;

		timestamp it3;
		cout << "  H_" << oc << " par time: " << it2 - it1 << "   seq time: " << it3 - it2 << endl;
		oc++;
	}

	util::timestamp ts6;
	cout << "Total time: " << ts6 - ts0minus - ((ts0minus_end - ts0minus) + (ts1minus_end - ts1minus) + (ts2minus_end - ts2minus) + (ts3minus_end - ts3minus) + td) << endl;

//	print_Ks (tlist, P, vfile);
	return;

}




// only ASYNC
void base_pi34_LessSpace (Graph& graph, int nEdge, vertex* F, vector<vertex>& Reals, util::timestamp& totaltime, vertex* threefour_number, const char* vfile, FILE* fp, string fname) {

//	int dum;
//	FILE* qp = fopen ("oo", "r");
//	vector<int> peel_order;
//	while (fscanf (qp, "%d", &dum) != EOF)
//		peel_order.push_back (dum);
//	fclose (qp);
//	printf ("size: %d\n", peel_order.size());

//	util::timestamp t_begin;
//	char *p = (char*) &totaltime;
//	util::timestamp *pout = (util::timestamp*) p;


	timestamp ts0minus;
	timestamp t_begin;

//	printf ("Rsdafg\n");
//	for (int i = 0; i < Reals.size(); i++)
//		printf ("Reals[%d]: %d\n", i, Reals[i]);
//
//	exit(1);
	// Ordered (directed) graph creation
	EdgeList2 el;
	vector<vertex> xel;
	Graph ordered_graph;
	create_ordered_graph (graph, el, xel, ordered_graph);

	timestamp ts1minus;
	tuple<int, int>* Nel = (tuple<int, int> *) malloc (sizeof(tuple<int, int>) * el.size());
	int Nelsize = el.size();
	for (int i = 0; i < el.size(); i++)
		Nel[i] = el[i];

	int* Nxel = (int *) malloc (sizeof(int) * xel.size());
	int Nxelsize = xel.size();
	for (int i = 0; i < xel.size(); i++)
		Nxel[i] = xel[i];

	int ngraphsize = graph.size();
	int* Nsizegraph = (int *) malloc (sizeof (int) * graph.size());
	int** Ngraph = (int **) malloc (sizeof (int*) * graph.size());
	for (int i = 0; i < graph.size(); i++) {
		Ngraph[i] = (int *) malloc (sizeof (int) * graph[i].size());
		for (int j = 0; j < graph[i].size(); j++)
			Ngraph[i][j] = graph[i][j];
		Nsizegraph[i] = graph[i].size();
	}



	int nordered_graphsize = ordered_graph.size();
	int* Nsizeordered_graph = (int *) malloc (sizeof (int) * ordered_graph.size());
	int** Nordered_graph = (int **) malloc (sizeof (int*) * ordered_graph.size());
	for (int i = 0; i < ordered_graph.size(); i++) {
		Nordered_graph[i] = (int *) malloc (sizeof (int) * ordered_graph[i].size());
		for (int j = 0; j < ordered_graph[i].size(); j++)
			Nordered_graph[i][j] = ordered_graph[i][j];
		Nsizeordered_graph[i] = ordered_graph[i].size();
	}

	timestamp ts1minus_end;

	util::timestamp t_cog;
	cout << "creating ordered graph: " << t_cog - t_begin << endl;

	vector<triangle_id> tlist;
	vector<vertex> xtl;
	create_classic_triangleList (ordered_graph, el, tlist, xtl); // not parallel, like ordered_graph creation in pi23

	timestamp ts2minus;
	int Nxtlsize = xtl.size();
	vertex* Nxtl = (vertex *) malloc (sizeof (vertex) * xtl.size());
	for (int i = 0; i < xtl.size(); i++)
		Nxtl[i] = xtl[i];

	timestamp ts2minus_end;
	util::timestamp ttl;
	cout << "create triangle list: " << ttl - t_begin << endl;

	// 4-clique counting
	F = (vertex *) malloc (sizeof(vertex) * tlist.size());
#ifndef ASYNC
	vertex* G = (vertex *) malloc (sizeof(vertex) * tlist.size());
#endif

	long fccount = intersect3_count_classic_efficient (graph, ordered_graph, el, xel, xtl, tlist); // todo: can think about convert STLs to pointers


//	cout << "nVtx:" << graph.size() << "  nEdge: " << nEdge << " nTri: " << tlist.size() << " n4c: " << fccount << endl;
//	exit(1);


	timestamp ts3minus;

	int Ntlistsize = tlist.size();
	triangle_id* Ntlist = (triangle_id *) malloc (sizeof (triangle_id) * tlist.size());
	for (int i = 0; i < tlist.size(); i++) {
		Ntlist[i].id = tlist[i].id;
		Ntlist[i].triple = tlist[i].triple;
	}

	timestamp ts3minus_end;

#pragma omp parallel for
	for (int i = 0; i < Ntlistsize; i++)
		F[i] = Ntlist[i].id;


	util::timestamp t_4c;
	cout << "4c counting: " << t_4c - t_begin << endl;
	util::timestamp td (0, 0);
	int oc = 0;

	timestamp ts2;

#pragma omp parallel for schedule (dynamic, 1000)
	for (int ind = 0; ind < Ntlistsize; ind++) {
		initial_h_index (ind, Nsizegraph, Ngraph, Ntlist, Nxel, Nxtl, Nordered_graph, Nsizeordered_graph, F);
	}
	timestamp ts3;

	bool changed[tlist.size()];
	memset (changed, 255, sizeof(bool) * Ntlistsize); // set all true
//	for (int i = 0; i < tlist.size(); i++)
//		changed[i] = true;
	bool flag = true;
	while (flag) {
//		util::timestamp td1;
#ifdef DEBUG_00
		// H values for each triangle
		print_Hs (tlist, F, vfile, oc);
#endif

#ifdef DEBUG_0
//		bool eda_flag = false;
//		if (fname.find("wiki") != string::npos) {
//			if (oc == 20 || oc == 40 || oc == 60 || oc == 80 || oc == 100)
//				eda_flag = true;
//		}
//		if (fname.find("soc-twit") != string::npos) {
//			if (oc == 30 || oc == 40)
//				eda_flag = true;
//		}
//		if (fname.find("orkut") != string::npos) {
//			if (oc == 0 || oc == 1 || oc == 2 || oc == 3 || oc == 20 || oc == 50 || oc == 100)
//				eda_flag = true;
//		}
//		if (fname.find("Live") != string::npos) {
//			if (oc == 0 || oc == 1 || oc == 2 || oc == 3 || oc == 20 || oc == 40 || oc == 60)
//				eda_flag = true;
//		}
//		if (fname.find("email") != string::npos) {
//			if (oc == 0 || oc == 1 || oc == 2 || oc == 3 || oc == 20 || oc == 40 || oc == 60)
//				eda_flag = true;
//		}
//		if (eda_flag) {
//			cout << fname << " " << oc << endl;
//			compute_KT_EDA (graph, tlist, Reals, F, oc, Nxel, Nxtl, Ntlist, Nordered_graph, Nsizeordered_graph, Ngraph, Nsizegraph);
//		}
		simple_distance (Reals, F, oc);
#endif

//		util::timestamp td2;
//		td += td2 - td1;
		util::timestamp t_end;
		cout << "H_" << oc << " time: " << t_end - t_begin << endl;
		flag = false;

		int active_tris = 0;
		for (int ind = 0; ind < Ntlistsize; ind++)
			if (changed[ind] && F[ind] > 0)
				active_tris++;
		cout << "In oc: " << oc << " Active tris: " << active_tris << " / nTri: " << Ntlistsize << endl;

#pragma omp parallel
		{
#pragma omp for nowait schedule(dynamic, 1000)
//		for (vertex ind = 0; ind < Ntlistsize; ind++) {
		for (int i = 0; i < Ntlistsize; i++) {
//		for (int i = Ntlistsize - 1; i >= 0; i--) {
//			int ind = peel_order[i];

			int ind = i;

			if (!changed[ind])
				continue;
			if (F[ind] == 0)
				continue;


			changed[ind] = false;
			vector<vertex> neigs;

			vertex u = get<0> (Ntlist[ind].triple);
			vertex v = get<1> (Ntlist[ind].triple);
			vertex w = get<2> (Ntlist[ind].triple);

			if (oc == 0) {
				HashMap<int> gmap (INT_MIN);
				int gts = 0;
				int eqs = 0;
				int H = 0;

				int ii = 0, jj = 0, kk = 0;
				vertex u_size = Nsizegraph[u];
				vertex v_size = Nsizegraph[v];
				vertex w_size = Nsizegraph[w];

				while (ii < u_size && jj < v_size && kk < w_size) {
					vertex a = Ngraph[u][ii];
					vertex b = Ngraph[v][jj];
					vertex c = Ngraph[w][kk];
					if (a == b && a == c) {
						vertex x = a;
						vertex id1 = NgetTriId (u, v, x, Nxel, Nxtl, Ntlist, Nordered_graph, Nsizeordered_graph, Ngraph, Nsizegraph);
						vertex id2 = NgetTriId (u, w, x, Nxel, Nxtl, Ntlist, Nordered_graph, Nsizeordered_graph, Ngraph, Nsizegraph);
						vertex id3 = NgetTriId (v, w, x, Nxel, Nxtl, Ntlist, Nordered_graph, Nsizeordered_graph, Ngraph, Nsizegraph);
						neigs.push_back (id1);
						neigs.push_back (id2);
						neigs.push_back (id3);
						int sm = minof (F[id1], F[id2], F[id3]);

						if (sm == H + 1) {
							if (eqs > 0) {
								eqs--;
								gts++;
								gmap[sm]++;
							}
							else { // eqs = 0
								H++;
								int gH = 0;
								if (!gmap.hasDefaultValue (H))
									gH = gmap[H];
								eqs = gH + 1;
								gts -= gH;
								gmap.erase (H);
							}
						}
						else if (sm > H + 1) {
							if  (eqs > 0) {
								eqs--;
								gts++;
								gmap[sm]++;
							}
							else { // eqs = 0
								gts++;
								H++;
								int gH = 0;
								if (!gmap.hasDefaultValue (H))
									gH = gmap[H];
								eqs = gH;
								gts -= gH;
								gmap[sm]++;
								gmap.erase (H);
							}
						}
						ii++; jj++; kk++;
					}
					else {
						vertex m = max3_old(a, b, c);
						if (a != m)
							ii++;
						if (b != m)
							jj++;
						if (c != m)
							kk++;
					}
				}
#ifdef ASYNC
				if (F[ind] != H) {
					F[ind] = H;
					for (int k = 0; k < neigs.size(); k++)
						changed[neigs[k]] = true;
				}
#else
				if (G[ind] != H) {
					G[ind] = H;
					for (int k = 0; k < neigs.size(); k++)
						changed[neigs[k]] = true;
				}
#endif

			}

			else {
				int previous_F = F[ind];
				int greaterequals = 0;
				vector<int> smallers (previous_F, 0);
				bool yep_set = false;
				size_t ii = 0, jj = 0, kk = 0;

				vertex u_size = Nsizegraph[u];
				vertex v_size = Nsizegraph[v];
				vertex w_size = Nsizegraph[w];
				while (ii < u_size && jj < v_size && kk < w_size) {
					vertex a = Ngraph[u][ii];
					vertex b = Ngraph[v][jj];
					vertex c = Ngraph[w][kk];

					if (a == b && a == c) {
						vertex x = a;
						vertex id1 = NgetTriId (u, v, x, Nxel, Nxtl, Ntlist, Nordered_graph, Nsizeordered_graph, Ngraph, Nsizegraph);
						vertex id2 = NgetTriId (u, w, x, Nxel, Nxtl, Ntlist, Nordered_graph, Nsizeordered_graph, Ngraph, Nsizegraph);
						vertex id3 = NgetTriId (v, w, x, Nxel, Nxtl, Ntlist, Nordered_graph, Nsizeordered_graph, Ngraph, Nsizegraph);
						neigs.push_back (id1);
						neigs.push_back (id2);
						neigs.push_back (id3);
						int sm = minof (F[id1], F[id2], F[id3]);
						if (sm >= previous_F)
							greaterequals++;
						else
							smallers[sm]++;
						if (greaterequals == previous_F) {
							yep_set = true;
							break;
						}
						ii++; jj++; kk++;
					}
					else {
						vertex m = max3_old(a, b, c);
						if (a != m)
							ii++;
						if (b != m)
							jj++;
						if (c != m)
							kk++;
					}
				}

				if (!yep_set) {
					int newF = 0;
					for (int j = previous_F - 1; j > 0; j--) {
						greaterequals += smallers[j];
						if (greaterequals >= j) {
							newF = j;
							break;
						}
					}
#ifdef ASYNC
					if (F[ind] != newF) {
						F[ind] = newF;
						for (int k = 0; k < neigs.size(); k++)
							changed[neigs[k]] = true;
					}
#else
					if (G[ind] != newF) {
						G[ind] = newF;
						for (int k = 0; k < neigs.size(); k++)
							changed[neigs[k]] = true;
					}
#endif

				}

			}

			flag = true;
		}
		}
#ifndef ASYNC
		memcpy (F, G, sizeof(vertex) * tlist.size());
#endif
		oc++;
	}


	util::timestamp ts6;
	cout << "Total time: " << ts6 - ts0minus - ((ts1minus_end - ts1minus) + (ts2minus_end - ts2minus) + (ts3minus_end - ts3minus) + td) << endl;

	cout << "H_" << oc << " time: " << ts6 - t_begin - td << endl;
	cout << "Converges at " <<  oc << endl;

#ifdef DEBUG_00
	// H values for each triangle
	print_Hs (tlist, F, vfile, oc);
#endif

#ifdef DEBUG_0
	simple_distance (Reals, F, oc);
//	compute_KT_EDA (graph, tlist, Reals, F, oc, Nxel, Nxtl, Ntlist, Nordered_graph, Nsizeordered_graph, Ngraph, Nsizegraph);
#endif

#ifndef CONSTRUCT
//	print_Ks (tlist, F, vfile);
#endif

	free (F);
	for (vertex i = 0; i < ordered_graph.size(); ++i)
		ordered_graph[i].clear();
	ordered_graph.clear();
	el.clear();
	xel.clear();
	return;
}