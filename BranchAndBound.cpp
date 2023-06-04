#include "pch.h"
#include "Graph.h"
#include <queue>
#include <iostream>
#include <iomanip> 
#include <map>


struct tupla {
	double m_longitud=0.0;
	int Nvisitats=1;
	double cota_inferiror= 0.0;
	double cota_superior=0.0;
	vector<int> vertexs;
	vector<bool> visitats;
	double h = 0.0;
};
// SalesmanTrackBranchAndBound1 ===================================================


struct comparatorBB1 {
	bool operator()(const tupla* t1, const tupla* t2) {
		return t1->m_longitud > t2->m_longitud;
	}
};


int mida_max;
int podats;
int creats;


CTrack SalesmanTrackBranchAndBound1(CGraph& graph, CVisits& visits)
{
	mida_max=0;
	podats=0;
	creats = 0;
	//DEclaracion Variables
	CTrack auxoptimo = SalesmanTrackGreedy(graph, visits);
	double LenghtAuxOpt = auxoptimo.Length();
	int NCVvisits = visits.GetNVertices();
	//CREEM DICCIONARI
	vector<CVertex*> diccionari;
	for (CVertex* v : visits.m_Vertices)
	{
		diccionari.push_back(v);
	}
	//CREAR MATRIU
	pair<double, CTrack>** matriu = new pair<double, CTrack>*[NCVvisits];
	for (int i = NCVvisits - 1; i >= 0; i--) 
	{
		matriu[i]= new pair<double, CTrack>[NCVvisits];
	}
	//emplenem la fcking matriu
	//mirem tots els nodes
	int origen = 0;
	for (CVertex* vOrigen : visits.m_Vertices)
	{
		if (origen != (NCVvisits - 1)) {
			//cridem dijkstra i actualitzem les distancies del vertex origen als altres
			DijkstraQueue(graph, vOrigen);
			int destino = 0;
			//recorrem els nodes de visits
			for (CVertex* vDestino : visits.m_Vertices)
			{
				if (origen != destino && destino != 0) {
					//guardem la distancia
					matriu[origen][destino].first = vDestino->m_DijkstraDistance;
					//carregem les arestes del cami
					while (vDestino->m_pDijkstraPrevious != NULL)
					{
						matriu[origen][destino].second.AddFirst(vDestino->m_pDijkstraPrevious);
						vDestino = vDestino->m_pDijkstraPrevious->m_pOrigin;
					}
				}
				destino++;
			}
		}
		origen++;
	}
	//FER PRIORITY QUEUE DE LES TUPLES (MATRIU AMB LES VAINES PER NO HAVER DE FER-HO CADA COP)
	std::priority_queue<tupla*, std::vector<tupla*>, comparatorBB1> priority_queue; //cua de prioritat per la merda aquesta
	tupla* actual=new tupla(); //TUPLA DEL VERTEX ORIGEN
	actual->vertexs.push_back(0);
	actual->visitats.resize(NCVvisits);
	actual->visitats[0] = true;
	priority_queue.push(actual); //guardem les tuples a la priority queue
	bool encontrat = false;
	while (!priority_queue.empty())
	{
		actual = priority_queue.top(); //pillem la primera tupla que hi ha a la priority queue
		priority_queue.pop(); //la treiem de la priority queue
		if (diccionari[NCVvisits - 1] == diccionari[actual->vertexs.back()] && actual->Nvisitats == NCVvisits) //si ´l'úlitm vèrtex és el desti
		{
			encontrat = true;
			break;
		}
		// for de les columnes per "expandir el vèrtex"
		for (int destino = NCVvisits - 1; destino > 0; destino--)
		{
			if (destino != actual->vertexs.back() && !actual->visitats[destino] && !(destino == NCVvisits - 1 && destino != actual->Nvisitats))
			{
				double lonDesti = matriu[actual->vertexs.back()][destino].first + actual->m_longitud;
				if (LenghtAuxOpt + 0.00001 > lonDesti) {
					tupla* actual1 = new tupla((*actual));
					actual1->m_longitud = lonDesti;
					actual1->vertexs.push_back(destino);
					actual1->visitats[destino] = true;
					actual1->Nvisitats++;
					creats++;
					priority_queue.push(actual1);
				}
				else
				{
					podats++;
				}
			}
			else
			{
				podats++;
			}
		}

		if (priority_queue.size()>mida_max)
		{
			mida_max = priority_queue.size();
		}
	}

	//AFEGIR LA TUPLA DEL VERTEX ORIGEN A LA PRIORITY QUEUE
	if (encontrat)
	{
		auxoptimo.Clear();
		for (int i = NCVvisits - 2; i >= 0 ; i--)
		{
			auxoptimo.AppendBefore(matriu[actual->vertexs[i]][actual->vertexs[i + 1]].second);
		}
	}
	return auxoptimo;
}


// SalesmanTrackBranchAndBound2 ===================================================

struct comparatorBB2 {
	bool operator()(const tupla* t1, const tupla* t2) {
		return t1->cota_inferiror > t2->cota_inferiror;
	}
};


CTrack SalesmanTrackBranchAndBound2(CGraph& graph, CVisits& visits)
{
	mida_max = 0;
	podats = 0;
	creats = 0;
	CTrack auxoptimo = SalesmanTrackGreedy(graph, visits);
	double LenghtAuxOpt = auxoptimo.Length();
	int NCVvisits = visits.GetNVertices();
	//CREEM DICCIONARI
	vector<CVertex*> diccionari;
	for (CVertex* v : visits.m_Vertices)
	{
		diccionari.push_back(v);
	}
	//CREAR MATRIU	
	vector<vector<pair<double, CTrack>>> matriu; //creem la matriu (sera de double(longitud) i camins(camins fins al vertex)
	matriu.resize(visits.GetNVertices());
	vector<pair<double, double>> heuristica; //creem la array (sera de double(minimo) i double(maximo)
	heuristica.resize(NCVvisits);
	for (int i = 0; i < NCVvisits; i++) 
	{
		matriu[i].resize(NCVvisits);
		heuristica[i].first = DBL_MAX;
	}
	//emplenem la fcking matriu
	//mirem tots els nodes
	int origen = 0;
	for (CVertex* vOrigen : visits.m_Vertices)
	{
		if (origen != (NCVvisits - 1)) 
		{
			//cridem dijkstra i actualitzem les distancies del vertex origen als altres
			DijkstraQueue(graph, vOrigen);
			int destino = 0;
			//recorrem els nodes de visits
			for (CVertex* vDestino : visits.m_Vertices)
			{
				//guardem la distancia
				if (origen != destino) //&& destino != 0)
				{
					matriu[origen][destino].first = vDestino->m_DijkstraDistance;
					//carregem les arestes del cami
					while (vDestino->m_pDijkstraPrevious != NULL)
					{
						matriu[origen][destino].second.AddFirst(vDestino->m_pDijkstraPrevious);
						vDestino = vDestino->m_pDijkstraPrevious->m_pOrigin;
					}
					if (matriu[origen][destino].first > heuristica[destino].second)
					{
						heuristica[destino].second = matriu[origen][destino].first;
					}
					if (matriu[origen][destino].first < heuristica[destino].first)
					{
						heuristica[destino].first = matriu[origen][destino].first;
					}
				}
				destino++;
			}
		}
		origen++;
	}
	//FER PRIORITY QUEUE DE LES TUPLES (MATRIU AMB LES VAINES PER NO HAVER DE FER-HO CADA COP)
	std::priority_queue<tupla*, std::vector<tupla*>, comparatorBB2> priority_queue; //cua de prioritat per la merda aquesta
	//TUPLA DEL VERTEX ORIGEN
	tupla* actual = new tupla();
	actual->vertexs.push_back(0);
	actual->visitats.resize(visits.GetNVertices());
	actual->visitats[0] = true;
	for (int i = 1; i < heuristica.size(); i++)
	{
		actual->cota_inferiror += heuristica[i].first;
		actual->cota_superior += heuristica[i].second;

	}
	priority_queue.push(actual); //guardem les tuples a la priority queue
	bool encontrat = false;
	double cotasup_global = DBL_MAX;
	while (!priority_queue.empty())
	{
		actual = priority_queue.top(); //pillem la primera tupla que hi ha a la priority queue
		priority_queue.pop(); //la treiem de la priority queue
		//mirem si el cami és solucio
		if (diccionari[NCVvisits - 1] == diccionari[actual->vertexs.back()] && actual->Nvisitats == NCVvisits) //si ´l'úlitm vèrtex és el desti
		{
			encontrat = true;
			break;
		}
		// for de les columnes per "expandir el vèrtex"
		for (int destino = NCVvisits - 1; destino > 0; destino--)
		{
			if (destino != actual->vertexs.back() && !actual->visitats[destino] && !(destino == NCVvisits - 1 && NCVvisits - 1 != actual->Nvisitats))
			{
				double lonD = matriu[actual->vertexs.back()][destino].first;
				double CI = (actual->cota_inferiror - heuristica[destino].first) + lonD;
				double CS = (actual->cota_superior - heuristica[destino].second) + lonD;
				if (LenghtAuxOpt + 0.00001 > actual->m_longitud + lonD && cotasup_global + 0.00001 > CI)
				{

					tupla* actual1 = new tupla(*actual);
					actual1->visitats[destino] = true;
					actual1->vertexs.push_back(destino);
					actual1->m_longitud += lonD;
					actual1->cota_inferiror = CI;
					actual1->cota_superior = CS;
					actual1->Nvisitats++;
					creats++;
					priority_queue.push(actual1);
					if (cotasup_global > CS)
					{
						cotasup_global = CS;
					}
				}
				else
				{
					podats++;
				}
			}
			else
			{
				podats++;
			}
		}
		if (priority_queue.size() > mida_max)
		{
			mida_max = priority_queue.size();
		}
	}
	if (encontrat)
	{
		auxoptimo.Clear();
		for (int i = 0; i < actual->vertexs.size() - 1; i++)
		{
			auto a = matriu[actual->vertexs[i]][actual->vertexs[i + 1]].second;
			auxoptimo.Append(matriu[actual->vertexs[i]][actual->vertexs[i + 1]].second);
		}
	}
	return auxoptimo;
}

// SalesmanTrackBranchAndBound3 ===================================================


CTrack SalesmanTrackBranchAndBound3(CGraph& graph, CVisits& visits)
{
	mida_max = 0;
	podats = 0;
	creats = 0;
	CTrack auxoptimo = SalesmanTrackGreedy(graph, visits);
	double LenghtAuxOpt = auxoptimo.Length() + 0.00001;
	int NCVvisits = visits.GetNVertices();
	//CREEM DICCIONARI
	vector<CVertex*> diccionari;
	for (CVertex* v : visits.m_Vertices)
	{
		diccionari.push_back(v);
	}
	//CREAR MATRIU	
	vector<vector<pair<double, CTrack>>> matriu; //creem la matriu (sera de double(longitud) i camins(camins fins al vertex)
	matriu.resize(visits.GetNVertices());
	for (int i = 0; i < NCVvisits; i++)
	{
		matriu[i].resize(NCVvisits);
	}
	//emplenem la fcking matriu
	//mirem tots els nodes
	int origenaux = 0;
	for (CVertex* vOrigen : visits.m_Vertices)
	{
		if (origenaux != (NCVvisits - 1))
		{
			//cridem dijkstra i actualitzem les distancies del vertex origen als altres
			DijkstraQueue(graph, vOrigen);
			int destino = 0;
			//recorrem els nodes de visits
			for (CVertex* vDestino : visits.m_Vertices)
			{
				//guardem la distancia
				if (origenaux != destino) //&& destino != 0)
				{
					matriu[origenaux][destino].first = vDestino->m_DijkstraDistance;
					//carregem les arestes del cami
					while (vDestino->m_pDijkstraPrevious != NULL)
					{
						matriu[origenaux][destino].second.AddFirst(vDestino->m_pDijkstraPrevious);
						vDestino = vDestino->m_pDijkstraPrevious->m_pOrigin;
					}
					/*if (matriu[origenaux][destino].first > heuristica[destino].second)
					{
						heuristica[destino].second = matriu[origenaux][destino].first;
					}
					if (matriu[origenaux][destino].first < heuristica[destino].first)
					{
						heuristica[destino].first = matriu[origenaux][destino].first;
					}*/
				}
				destino++;
			}
		}
		origenaux++;
	}
	//FER PRIORITY QUEUE DE LES TUPLES (MATRIU AMB LES VAINES PER NO HAVER DE FER-HO CADA COP)
	std::priority_queue<tupla*, std::vector<tupla*>, comparatorBB2> priority_queue; //cua de prioritat per la merda aquesta
	//TUPLA DEL VERTEX ORIGEN
	tupla* actual = new tupla();
	actual->vertexs.push_back(0);
	actual->visitats.resize(visits.GetNVertices());
	actual->visitats[0] = true;
	/*for (int i = 1; i < heuristica.size(); i++)
	{
		actual->cota_inferiror += heuristica[i].first;
		actual->cota_superior += heuristica[i].second;
	}*/
	priority_queue.push(actual); //guardem les tuples a la priority queue
	bool encontrat = false;
	double cotasup_global = DBL_MAX;
	while (!priority_queue.empty())
	{
		actual = priority_queue.top(); //pillem la primera tupla que hi ha a la priority queue
		priority_queue.pop(); //la treiem de la priority queue
		//mirem si el cami és solucio
		if (diccionari[NCVvisits - 1] == diccionari[actual->vertexs.back()] && actual->Nvisitats == NCVvisits) //si ´l'úlitm vèrtex és el desti
		{
			encontrat = true;
			break;
		}

		// for de les columnes per "expandir el vèrtex"
		for (int destino = NCVvisits - 1; destino > 0; destino--)
		{
			if (destino != actual->vertexs.back() && !actual->visitats[destino] && !(destino == NCVvisits - 1 && NCVvisits - 1 != actual->Nvisitats))
			{
				tupla* actual1 = new tupla(*actual);
				actual1->visitats[destino] = true;
				actual1->vertexs.push_back(destino);
				actual1->Nvisitats++;

				vector<pair<double, double>> heuristica; //creem la array (sera de double(minimo) i double(maximo)
				heuristica.resize(NCVvisits);
				for (int i = 0; i < NCVvisits; i++)
				{
					heuristica[i].first = DBL_MAX;
				}
				for (int origen = 0; origen < NCVvisits - 1; origen++)
				{
					if (!actual1->visitats[origen] || origen == actual1->vertexs.back())
					{
						for (int destino = 1; destino < NCVvisits; destino++)
						{
							if (origen != destino && !actual1->visitats[destino] && !(destino == NCVvisits - 1 && NCVvisits - 1 != actual1->Nvisitats && origen == actual1->vertexs.back()))
							{
								if (matriu[origen][destino].first > heuristica[destino].second)
								{
									heuristica[destino].second = matriu[origen][destino].first;
								}
								if (matriu[origen][destino].first < heuristica[destino].first)
								{
									heuristica[destino].first = matriu[origen][destino].first;
								}
							}
						}
					}
				}

				actual1->m_longitud += matriu[actual->vertexs.back()][destino].first;
				actual1->cota_inferiror = actual1->m_longitud;
				actual1->cota_superior = actual1->m_longitud;

				for (int i = 0; i < NCVvisits; i++)
				{
					if (!actual1->visitats[i])
					{
						actual1->cota_inferiror += heuristica[i].first;
						actual1->cota_superior += heuristica[i].second;
					}
				}
				//double CI= aux+lonD;
				//double CS = aux1 + lonD;//(actual->cota_superior - heuristica[destino].second) + lonD;
				if (LenghtAuxOpt > actual1->cota_inferiror && cotasup_global + 0.00001 > actual1->cota_inferiror)
				{

					//	actual1->m_longitud += lonD;
		//				actual1->cota_inferiror = CI;
	//					actual1->cota_superior = CS;
						//actual1->h = heuristica[destino].first;
					creats++;
					priority_queue.push(actual1);
					if (cotasup_global > actual1->cota_superior)
					{
						cotasup_global = actual1->cota_superior;
					}
				}
				else
				{
					podats++;
				}
			}
			else
			{
				podats++;
			}
		}
		if (priority_queue.size() > mida_max)
		{
			mida_max = priority_queue.size();
		}
	}
	if (encontrat)
	{
		auxoptimo.Clear();
		for (int i = 0; i < actual->vertexs.size() - 1; i++)
		{
			auto a = matriu[actual->vertexs[i]][actual->vertexs[i + 1]].second;
			auxoptimo.Append(matriu[actual->vertexs[i]][actual->vertexs[i + 1]].second);
		}
	}
	return auxoptimo;

}

