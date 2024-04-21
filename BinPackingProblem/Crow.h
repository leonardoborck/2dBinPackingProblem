#pragma once
#include <iostream>
#include <vector>

#include "classes\Item.h"
#include "classes\Recipiente.h"

using namespace std;

class Crow
{
public:
	vector<vector<int>> MemoriaAtual;
	vector<vector<int>> MemoriaAnterior;
	vector<vector<int>> MelhorMemoria;
	double AvaliacaoDaSolucao;

	vector<Item> Itens;

	vector<Recipiente> RecipientesAtual;
	vector<Recipiente> RecipientesAnterior;

	Crow(vector<Item> itens, vector<Recipiente> recipientes);
	void GeraSolucaoInicialAleatoria();
	void CorrigeSolucoesInviaveis();
	double VerificaUtilizacaoDoRecipiente(int indexDoRecipiente, vector<vector<int>> solucao);
	void RemoveItensDosRecipientesSobrecarregados(int indexDoRecipiente, vector<int> itensRemovidos);
	void ReinsereItensUsandoLeftBottomPolicy(vector<int> itensRemovidos);
};

