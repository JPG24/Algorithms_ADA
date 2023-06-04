#include "pch.h"
#include "Graph.h"
#include <climits>
#include <queue>

// =============================================================================
// Dijkstra ====================================================================
// =============================================================================



struct comparatorCV {
	bool operator()(CVertex* a, CVertex* b) {
		return a->m_DijkstraDistance > b->m_DijkstraDistance;
	}
};


void Dijkstra(CGraph& graph, CVertex* pStart)
{
	for (CVertex& v : graph.m_Vertices) {
		v.m_DijkstraDistance = DBL_MAX;
		v.m_DijkstraVisit = false;
		v.m_pDijkstraPrevious = NULL;
	}
	pStart->m_DijkstraDistance = 0;


	CVertex* p_actual = pStart;
	for (int i = 0; i < graph.GetNVertices(); i++)
	{
		p_actual->m_DijkstraVisit = true;

		for (CEdge* pE : p_actual->m_Edges) //processar totes les arestes d'un vèrtex
		{
			if (pE->m_pDestination->m_DijkstraDistance > (p_actual->m_DijkstraDistance + pE->m_Length)) {//distancia va + longitud de l'aresta que uneix amb v
				pE->m_pDestination->m_DijkstraDistance = (p_actual->m_DijkstraDistance + pE->m_Length);
				pE->m_pDestination->m_pDijkstraPrevious = pE;
			}
		}

		//nou pActual = vertex no visitat amb distancia mes petita

		double distancia = DBL_MAX;

		for (CVertex& v : graph.m_Vertices) //processar totes les arestes d'un vèrtex
		{
			if (v.m_DijkstraDistance < distancia && !v.m_DijkstraVisit) //si la distancia del seguent vertex és mes petita que la distancia és el nou pactual
			{
				p_actual = &v;
				distancia = v.m_DijkstraDistance;
			}
		}
	}
}

// =============================================================================
// DijkstraQueue ===============================================================
// =============================================================================

void DijkstraQueue(CGraph& graph, CVertex* pStart)
{
	for (CVertex& v : graph.m_Vertices) {
		v.m_DijkstraDistance = DBL_MAX;
		v.m_DijkstraVisit = false;
		v.m_pDijkstraPrevious = NULL;
	}
	pStart->m_DijkstraDistance = 0;
	priority_queue<CVertex*, std::vector<CVertex*>, comparatorCV> cola;
	cola.push(pStart);

	while (!cola.empty())
	{
		CVertex* va = cola.top();
		cola.pop();
		for (CEdge* pE : va->m_Edges)
		{
			if (pE->m_pDestination->m_DijkstraDistance > (va->m_DijkstraDistance + pE->m_Length))//distancia va + longitud de l'aresta que uneix amb v
			{
				pE->m_pDestination->m_DijkstraDistance = (va->m_DijkstraDistance + pE->m_Length);
				pE->m_pDestination->m_pDijkstraPrevious = pE;
				cola.push(pE->m_pDestination);
			}
		}
		va->m_DijkstraVisit = true;
	}
}
