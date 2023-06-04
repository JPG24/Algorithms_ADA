#include "pch.h"
#include "Graph.h"
#include <queue>
#include <iostream>
#include <iomanip> 
#include <random>
#include <chrono>
#include <numeric>

// SalesmanTrackProbabilistic ==================================================



vector<int> OrdreVisites(int tamaño) {
	vector<int> array(tamaño-2);
	iota(array.begin(), array.end(), 1);
	random_shuffle(array.begin(), array.end());
	array.push_back(tamaño - 1);
	array.insert(array.begin(), 0);
	return array;
}



double calcularDist(vector<int> ordre, vector<vector<pair<double, CTrack>>> matriu) {
	double dist=0;
	for (int i = 0; i < matriu.size()-1; i++)
	{
		dist += matriu[ordre[i]][ordre[i + 1]].first;
	}
	return dist;
}

vector<int> cambiarIndices(vector<int> orden, int i, int j) {
	int aux = orden[i];
	orden[i] = orden[j];
	orden[j] = aux;
	return orden;
}

CTrack SalesmanTrackProbabilistic(CGraph& graph, CVisits& visits)
{
	int tamaño = visits.GetNVertices();
	vector<vector<pair<double, CTrack>>> matriu; //creem la matriu (sera de double(longitud) i camins(camins fins al vertex)
	matriu.resize(tamaño);
	for (int i = 0; i < tamaño; i++) {
		matriu[i].resize(tamaño);
	}
	


	int origen = 0;
	for (CVertex* vOrigen : visits.m_Vertices)
	{
		if (origen != (tamaño - 1)) {
			//cridem dijkstra i actualitzem les distancies del vertex origen als altres
			DijkstraQueue(graph, vOrigen);
			int destino = 0;
			//recorrem els nodes de visits
			for (CVertex* vDestino : visits.m_Vertices)
			{
				//guardem la distancia
				if (origen != destino && destino != 0) {

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
	
	vector < pair<double, vector<int>>> millor_global;
	millor_global.push_back({ DBL_MAX,vector<int>()});

	for (int intent = 0; intent < tamaño *7000; intent++)
	{

		//Ordre aleatori
		vector<int> orden_utilitzat = OrdreVisites(tamaño);
		vector < pair<double, vector<int>>> millors_local;
		double dist_ord = calcularDist(orden_utilitzat,matriu);
		millors_local.push_back({ dist_ord,orden_utilitzat });
		
		//Descens
		bool espotmes=true;
		while (espotmes) 
		{
			espotmes = false;
			//Todos los posibles cambios del orden original
			for (int i = 1; i < tamaño - 2; i++)
			{
				for (int j = i + 1; j < tamaño - 1; j++)
				{
					double org = matriu[orden_utilitzat[i - 1]][orden_utilitzat[i]].first + matriu[orden_utilitzat[i]][orden_utilitzat[i + 1]].first + matriu[orden_utilitzat[j - 1]][orden_utilitzat[j]].first + matriu[orden_utilitzat[j]][orden_utilitzat[j + 1]].first;;
					double aux = matriu[orden_utilitzat[i - 1]][orden_utilitzat[j]].first + matriu[orden_utilitzat[j]][orden_utilitzat[i + 1]].first + matriu[orden_utilitzat[j - 1]][orden_utilitzat[i]].first + matriu[orden_utilitzat[i]][orden_utilitzat[j + 1]].first;;
					if (j == i + 1)
					{
						org = matriu[orden_utilitzat[i - 1]][orden_utilitzat[i]].first + matriu[orden_utilitzat[i]][orden_utilitzat[i + 1]].first + matriu[orden_utilitzat[j]][orden_utilitzat[j + 1]].first;
						aux = matriu[orden_utilitzat[i - 1]][orden_utilitzat[j]].first + matriu[orden_utilitzat[j]][orden_utilitzat[i]].first + matriu[orden_utilitzat[i]][orden_utilitzat[j + 1]].first;
					}
					double dist_aux = dist_ord - org + aux;
					if (millors_local[0].first + 0.00001 > dist_aux)
					{
						vector<int> orden_aux = cambiarIndices(orden_utilitzat, i, j);

						if (millors_local[0].first - 0.00001 < dist_aux)
						{
							millors_local.push_back({ dist_aux,orden_aux });
						}
						else
						{
							millors_local.clear();
							millors_local.push_back({ dist_aux,orden_aux });
							espotmes = true;
						}
					}
				}
				if (espotmes)
				{
					random_shuffle(millors_local.begin(), millors_local.end());
					orden_utilitzat = millors_local[0].second;
					dist_ord = millors_local[0].first;
				}
			}
		}
		if (millor_global[0].first + 0.00001 > millors_local[0].first)
		{
			millor_global.clear();
			millor_global.push_back({ millors_local[0].first,orden_utilitzat });
		}
	}


	CTrack optimo(&graph);
	for (int i = 0; i < tamaño - 1; i++)
	{
		optimo.Append(matriu[millor_global[0].second[i]][millor_global[0].second[i + 1]].second);
	}
	return optimo;

}
