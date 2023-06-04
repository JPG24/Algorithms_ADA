#include "pch.h"
#include "Graph.h"
#include <set>

// =============================================================================
// SalesmanTrackBacktracking ===================================================
// =============================================================================

CVertex* pDesti2;
double LongitudCamiActual2;
CTrack CamiMesCurt2(NULL);
double LongitudCamiMesCurt2;
int nVisits;
int contador = 0;

struct NodeCami {
	CEdge* m_pEdge;
	NodeCami* m_pAnterior;
};

void SalesmanTrackBacktrackingRec(NodeCami* pAnterior, CVertex* pActual) {

	if (pActual->m_VertexToVisit && (pActual->m_Marca == 0 || (pActual->m_Marca == 1 && pActual->m_apariciones > 0)))
	{
		contador++;
		pActual->m_Marca = 1;
		pActual->m_apariciones--;
		pActual->m_tramoBack = contador;
		if (pAnterior)
		{
			NodeCami* aux = pAnterior;
			while (aux) {
				aux->m_pEdge->m_pOrigin->m_JaHePassat = false;
				if (aux->m_pEdge->m_pOrigin->m_tramoBack == contador - 1) break;
				aux = aux->m_pAnterior;
			}
		}
	}
	if (pActual == pDesti2 && nVisits == contador) {
		if (LongitudCamiActual2 < LongitudCamiMesCurt2) {
			CamiMesCurt2.Clear();
			NodeCami* aux = pAnterior;
			while (aux) {
				CamiMesCurt2.m_Edges.push_front(aux->m_pEdge);
				aux = aux->m_pAnterior;
			}
			LongitudCamiMesCurt2 = LongitudCamiActual2;
		}
	}
	else if (LongitudCamiActual2 < LongitudCamiMesCurt2) {
		pActual->m_JaHePassat = true;
		NodeCami node;
		node.m_pAnterior = pAnterior;
		for (CEdge* pE : pActual->m_Edges) {
			if (!pE->m_pDestination->m_JaHePassat) {
				node.m_pEdge = pE;
				LongitudCamiActual2 += pE->m_Length;
				SalesmanTrackBacktrackingRec(&node, pE->m_pDestination);
				LongitudCamiActual2 -= pE->m_Length;
			}
		}
		pActual->m_JaHePassat = false;
	}
	if (pActual->m_VertexToVisit && pActual->m_Marca == 1 && contador == pActual->m_tramoBack)
	{
		pActual->m_Marca = 0;
		pActual->m_apariciones++;
		contador--;
		if (pAnterior)
		{
			NodeCami* aux = pAnterior;
			while (aux) {
				aux->m_pEdge->m_pOrigin->m_JaHePassat = true;
				if (aux->m_pEdge->m_pOrigin->m_tramoBack == contador) break;
				aux = aux->m_pAnterior;
			}
		}
	}
}


CTrack SalesmanTrackBacktracking(CGraph& graph, CVisits& visits)
{
	CVertex* pInici = *visits.m_Vertices.begin();
	pDesti2 = *visits.m_Vertices.rbegin();
	CamiMesCurt2.Clear();
	LongitudCamiMesCurt2 = numeric_limits<double>::max();
	LongitudCamiActual2 = 0.0;
	contador = 0;
	nVisits = visits.GetNVertices();
	for (CVertex& v : graph.m_Vertices) { v.m_JaHePassat = false; v.m_VertexToVisit = false; v.m_Marca = 2; }
	for (CVertex* v : visits.m_Vertices) { v->m_VertexToVisit = true; v->m_Marca = 0; v->m_apariciones++; }
	SalesmanTrackBacktrackingRec(NULL, pInici);
	return CamiMesCurt2;
}

// =============================================================================
// SalesmanTrackBacktrackingGreedy =============================================
// =============================================================================


vector<CTrack> camiActual;
vector<CTrack> camimasCorto;
vector<vector<CTrack>>caminos;
vector<bool>filaboleano;

int co;
void SalesmanTrackBacktrackingGreedyREC(int fila) {

	co++;
	filaboleano[fila] = true;

	if (nVisits == contador + 1 && fila == contador)
	{
		camimasCorto = camiActual;
		LongitudCamiMesCurt2 = LongitudCamiActual2;
	}
	else {
		for (int i = 0; i < nVisits; i++)
		{
			if (fila != i && !filaboleano[i])
			{
				LongitudCamiActual2 += caminos[fila][i].Length();
				if (LongitudCamiActual2 < LongitudCamiMesCurt2) {
					contador++;
					camiActual.push_back(caminos[fila][i]);
					SalesmanTrackBacktrackingGreedyREC(i);
					camiActual.pop_back();
					contador--;
				}
				LongitudCamiActual2 -= caminos[fila][i].Length();
			}
		}
	}
	filaboleano[fila] = false;
}

CTrack SalesmanTrackBacktrackingGreedy(CGraph& graph, CVisits& visits)
{
	LongitudCamiActual2 = 0.0;
	camimasCorto.clear();
	camiActual.clear();
	caminos.clear();
	filaboleano.clear();
	LongitudCamiMesCurt2 = numeric_limits<double>::max();
	nVisits = visits.GetNVertices();
	contador = 0;
	co = 0;
	for (CVertex* vi : visits.m_Vertices)
	{
		vector<CTrack> aux;
		for (CVertex* vj : visits.m_Vertices)
		{
			CTrack caminoauxiliar;
			if (vi != vj)
			{
				CVisits vstIJ(&graph);
				vstIJ.Add(vi);
				vstIJ.Add(vj);
				caminoauxiliar = SalesmanTrackGreedy(graph, vstIJ);
			}
			aux.push_back(caminoauxiliar);
		}
		caminos.push_back(aux);
		filaboleano.push_back(false);
	}

	SalesmanTrackBacktrackingGreedyREC(0);

		CTrack Track(&graph);
	for (int i = 0; i < camimasCorto.size(); i++)
	{
		Track.Append(camimasCorto[i]);
	}
	return Track;
}
