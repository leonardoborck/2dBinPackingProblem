#pragma once

#include "Crow.h"

void CarregaVariaveis(vector<Item> itens, vector<Recipiente> recipientes);
void InicializaCorvos(int tamanhoDaPopulacao, int z);
void InicializaCorvos(int tamanhoDaPopulacao, int z, int numeroDeArvores);
void ImprimePopulacaoDeCorvos(vector<vector<int>> solucao, double avaliacao);
Crow CrowSearch(int flightSize, double AP, int z, int maxIteration);
double Sigmoid(double x);