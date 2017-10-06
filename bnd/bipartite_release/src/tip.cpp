#include "main.h"



inline long long NEWnChoosek(long long n, int k) {
	if (k > n)
		return 0;
	if (k * 2 > n)
		k = n - k;
	if (k == 0)
		return 1;

	long long result = n;
	for (long long i = 2; i <= k; ++i) {
		result *= (n - i + 1);
		result /= i;
	}
	return result;
}


//long long countButterflies (Graph& rightGraph, Graph& leftGraph, long long* butterflyCounts, long long* bCount) {
long long countButterflies (Graph& rightGraph, Graph& leftGraph, long long* butterflyCounts, long long* bCount) {

//	long long* butterflyCounts = (long long *) calloc (sizeof(long long), rightGraph.size());
	timestamp t1;
	long long maxBc = 0;
	long long ee = 0;
	HashMap<long long> dup (0);
	vertex i = 0;
	for (i = 0; i < rightGraph.size(); i++) {
		dup.reset (0);
		for (auto v : rightGraph[i])
			for (auto w : leftGraph[v]) {
				if (i < w) { // other order definitions
					dup[w]++;
				}
			}


		for (auto it = dup.begin(); it != dup.end(); it++) {
			int x = it->first;
			long long count = it->second;
			if (x != i && count > 1) {
				long long c = NEWnChoosek (count, 2);
				printf ("c: %lld\n", c);
				ee += c;
				butterflyCounts[i] += c;
				butterflyCounts[x] += c;
			}
		}

//		if (butterflyCounts[i] > maxBc)
//			maxBc = butterflyCounts[i];

		(*bCount) += butterflyCounts[i];
		timestamp t3;
	}

	*bCount /= 2;

	printf ("i: %d\n", i);
	printf ("ee: %lld\n", ee);
	printf ("bFly: %lld, Left: %d, Right: %d\n", *bCount, leftGraph.size(), rightGraph.size());

	exit(1);

	return maxBc;
}
//#define HASHBUCKET
void tipDecomposition (Graph& leftGraph, Graph& rightGraph, edge nEdge, vector<vertex>& K, bool hierarchy, vertex* maxbicore, string vfile, FILE* fp, long long* bCount) {

	timestamp countingStart;

	long long* butterflyCounts = (long long*) calloc (sizeof(long long), rightGraph.size());
	long long maxBc = countButterflies (rightGraph, leftGraph, butterflyCounts, bCount); // counts butterflies for each vertex on the right

	timestamp peelingStart;
	printf ("# bflys: %lld\n", *bCount);
	cout << "Counting butterflies per vertex time: " << peelingStart - countingStart << endl;
	print_time (fp, "Counting butterflies per vertex time: ", peelingStart - countingStart);
	printf ("maxBc: %lld\n", maxBc);
	printf ("nEdge: %d\n", nEdge);

	// peeling
	K.resize (rightGraph.size(), -1);

#ifndef HASHBUCKET
	Naive_Bucket nBucket;
	nBucket.Initialize (maxBc+1, rightGraph.size());
#else
	Bucket nBucket;
	nBucket.Initialize (maxBc+1);
#endif

	long long nb = 0;
	for (size_t i = 0; i < rightGraph.size(); i++) {
		nb += butterflyCounts[i];
		if (butterflyCounts[i] > 0)
			nBucket.Insert (i, butterflyCounts[i]);
		else
			K[i] = 0;
	}

	printf ("nb : %lld\n", nb);



	// required for hierarchy
	vertex cid; // subcore id number
	vector<subcore> skeleton; // equal K valued cores
	vector<vertex> component; // subcore ids for each vertex
	vector<vp> relations;
	vector<vertex> unassigned;
	vertex nSubcores;

	if (hierarchy) {
		cid = 0;
		nSubcores = 0;
		component.resize (rightGraph.size(), -1);
	}

	long long bf_u = 0;
	while (true) {

		vertex u;
#ifdef HASHBUCKET
		long long val;
#else
		int val;
#endif

		if (nBucket.PopMin(&u, &val) == -1) // if the bucket is empty
			break;

		if (hierarchy) {
			unassigned.clear();
			subcore sc (val);
			skeleton.push_back (sc);
		}

		bf_u = K[u] = val;

		// combine neigs of neigs of i in distance-2 neighbors
		HashMap<long long> dup (0);
		for (auto v : rightGraph[u])
			for (auto w : leftGraph[v])
				dup[w]++;

		for (auto it = dup.begin(); it != dup.end(); it++) {
			int x = it->first;
			long long count = it->second;
			if (x != u && count > 1) {
				if (K[x] == -1) {
					long long decreaseAmount = NEWnChoosek (count, 2);
					if (nBucket.CurrentValue(x) - decreaseAmount <= val)
						nBucket.DecTo (x, val);
					else
						nBucket.DecTo (x, nBucket.CurrentValue(x) - decreaseAmount);
				}
#ifndef HASHBUCKET
				else if (hierarchy)
					createSkeleton (u, {x}, &nSubcores, K, skeleton, component, unassigned, relations);
#endif
			}
		}

		if (hierarchy)
			updateUnassigned (u, component, &cid, relations, unassigned);

	}

	nBucket.Free();
	*maxbicore = bf_u;

	printf ("maxBF: %ld\n", bf_u);
	timestamp peelingEnd;
	cout << "Only peeling time: " << peelingEnd - peelingStart << endl;
	print_time (fp, "Only peeling time: ", peelingEnd - peelingStart);
	cout << "Total time: " << peelingEnd - countingStart << endl;
	print_time (fp, "Total time: ", peelingEnd - countingStart);

#ifdef K_VALUES
	for (int i = 0; i < K.size(); i++)
		printf ("K[%d]: %d\n", i, K[i]);
#endif

	if (hierarchy) {
		buildHierarchy (*maxbicore, relations, skeleton, &nSubcores, nEdge, rightGraph.size(), leftGraph.size());
		timestamp nucleusEnd;
//		printf ("asdfasdf\n");
		print_time (fp, "Tip decomposition time with hierarchy construction: ", nucleusEnd - peelingStart);
		fprintf (fp, "# subcores: %d\t\t # subsubcores: %d\t\t |V|: %d\n", nSubcores, skeleton.size(), rightGraph.size());

		helpers dummy;
		presentNuclei ("TIP", skeleton, component, nEdge, dummy, vfile, fp, leftGraph, rightGraph, NULL);
		timestamp totalEnd;

		print_time (fp, "Total time, including the density computations: ", totalEnd - peelingStart);
	}

}

/*


// rightGraph is primary, leftGraph is secondary
void oldtipDecomposition (Graph& leftGraph, Graph& rightGraph, edge nEdge, vector<vertex>& K, bool hierarchy, vertex* maxbicore, string vfile, FILE* fp, long long* bCount) {

	timestamp peelingStart;

	vertex* butterflyCounts = (vertex *) calloc (sizeof(vertex), rightGraph.size());
	vertex maxBc = countButterflies (rightGraph, leftGraph, butterflyCounts, bCount); // counts butterflies for each vertex on the right

	// peeling
	K.resize (rightGraph.size(), -1);
	Naive_Bucket nBucket;
	nBucket.Initialize (maxBc+1, rightGraph.size());
	for (size_t i = 0; i < rightGraph.size(); i++)
		if (butterflyCounts[i] > 0)
			nBucket.Insert (i, butterflyCounts[i]);
		else
			K[i] = 0;

	vertex bf_u = 0;

	// required for hierarchy
	vertex cid; // subcore id number
	vector<subcore> skeleton; // equal K valued cores
	vector<vertex> component; // subcore ids for each vertex
	vector<vp> relations;
	vector<vertex> unassigned;
	vertex nSubcores;

	if (hierarchy) {
		cid = 0;
		nSubcores = 0;
		component.resize (rightGraph.size(), -1);
	}

	while (true) {
		vertex u, val;
		if (nBucket.PopMin(&u, &val) == -1) // if the bucket is empty
			break;

		if (hierarchy) {
			unassigned.clear();
			subcore sc (val);
			skeleton.push_back (sc);
		}

		bf_u = K[u] = val;

		for (size_t i = 0; i < rightGraph[u].size(); i++) {
			vertex v = rightGraph[u][i];
			for (size_t j = i + 1; j < rightGraph[u].size(); j++) {
				vertex w = rightGraph[u][j];
				vector<vertex> commons;
				intersection (leftGraph[v], leftGraph[w], commons);

				// each x in commons (except u) is the 4th vertex of the butterfly u, x (rights), v, w (lefts)
				for (vertex k = 0; k < commons.size(); k++) {
					vertex x = commons[k];
					if (x == u)
						continue;
					if (K[x] == -1) {
						if (nBucket.CurrentValue(x) > val)
							nBucket.DecVal(x);
					}
					else if (hierarchy)
						createSkeleton (u, {x}, &nSubcores, K, skeleton, component, unassigned, relations);
				}
			}
		}

		if (hierarchy)
			updateUnassigned (u, component, &cid, relations, unassigned);
	}

	nBucket.Free();
	*maxbicore = bf_u;

	timestamp peelingEnd;
	print_time (fp, "Peeling time: ", peelingEnd - peelingStart);

#ifdef K_VALUES
	for (int i = 0; i < K.size(); i++)
		printf ("K[%d]: %d\n", i, K[i]);
#endif

	if (hierarchy) {
		buildHierarchy (*maxbicore, relations, skeleton, &nSubcores, nEdge, rightGraph.size(), leftGraph.size());
		timestamp nucleusEnd;


		print_time (fp, "Tip decomposition time with hierarchy construction: ", nucleusEnd - peelingStart);
		fprintf (fp, "# subcores: %d\t\t # subsubcores: %d\t\t |V|: %d\n", nSubcores, skeleton.size(), rightGraph.size());

		helpers dummy;
		presentNuclei ("TIP", skeleton, component, nEdge, dummy, vfile, fp, leftGraph, rightGraph, NULL);
		timestamp totalEnd;

		print_time (fp, "Total time, including the density computations: ", totalEnd - peelingStart);
	}
}
*/
