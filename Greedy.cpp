#include "pch.h"
#include "Graph.h"
// SalesmanTrackGreedy =========================================================

CTrack SalesmanTrackGreedy(CGraph& graph, CVisits& visits)
{
	CTrack cami(&graph);
	if (visits.m_Vertices.size() > 2)
	{
		CVertex* v = visits.m_Vertices.front();
		list<CVertex*> candidats = visits.m_Vertices;
		candidats.pop_front();
		candidats.pop_back();
		while (!candidats.empty()) {

			DijkstraQueue(graph, v);
			CVertex* v1 = candidats.front();
			for (CVertex* c : candidats) {
				if (v1->m_DijkstraDistance > c->m_DijkstraDistance)
				{
					v1 = c;
				}
			}

			CTrack cami2(&graph);
			CVertex* aux = v1;

			do
			{
				cami2.AddFirst(aux->m_pDijkstraPrevious);
				aux = aux->m_pDijkstraPrevious->m_pOrigin;
			} while (aux != v);

			cami.Append(cami2);

			candidats.remove(v1);
			v = v1;

		}

		DijkstraQueue(graph, v);


		CVertex* aux = visits.m_Vertices.back();
		CTrack cami2(&graph);
		do
		{
			cami2.AddFirst(aux->m_pDijkstraPrevious);
			aux = aux->m_pDijkstraPrevious->m_pOrigin;
		} while (v != aux);
		cami.Append(cami2);
	}
	else
	{
		DijkstraQueue(graph, visits.m_Vertices.front());        //Esto si funciona pero lo hace muy justito (osea con poco margen de error en la prueba 1)
		CVertex* aux = visits.m_Vertices.back();
		CTrack cami2(&graph);
		do
		{
			cami2.AddFirst(aux->m_pDijkstraPrevious);
			aux = aux->m_pDijkstraPrevious->m_pOrigin;
		} while (visits.m_Vertices.front() != aux);
		cami.Append(cami2);

		/*DijkstraQueue(graph, visits.m_Vertices.front());         Por eso pongo esto para que pase la prueba 1 la pase con un margen de error mas grande
		cami.AddFirst(visits.m_Vertices.back()->m_pDijkstraPrevious);*/
	}
	return cami;
}
