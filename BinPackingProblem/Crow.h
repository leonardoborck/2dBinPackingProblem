#pragma once
#include <iostream>
#include <vector>

#include "classes\Item.h"
#include "classes\Recipiente.h"

class Crow
{
public:
	vector<vector<int>> MemoriaAtual;
	vector<vector<int>> MelhorMemoria;
	double AvaliacaoDaMelhorSolucao = 0;

	vector<Item> Itens;

	vector<Recipiente> RecipientesAtual;
	vector<Recipiente> MelhorRecipientes;

	Crow(vector<Item> itens, vector<Recipiente> recipientes);
	void GeraSolucaoInicialAleatoria(int z);
	void CorrigeSolucoesInviaveis(vector<vector<int>>& solucao, vector<Recipiente>& recipientes);
	double VerificaUtilizacaoDoRecipiente(int indexDoRecipiente, vector<vector<int>> solucao, vector<Recipiente> recipientes);
	void RemoveItensDosRecipientesSobrecarregados(int indexDoRecipiente, vector<int> &itensRemovidos, vector<vector<int>>& solucao, vector<Recipiente> recipientes);
	void ReinsereItensUsandoLeftBottomPolicy(vector<int> itensRemovidos, vector<vector<int>>& solucao, vector<Recipiente>& recipientes);
	bool HouveMelhora(int z);
	double AvaliaSolucao(vector<vector<int>> solucao, vector<Recipiente> recipientes, int z);
	vector<int> IndexDosItensNoRecipiente(int indexDoRecipiente, vector<vector<int>> solucao);
	void GeraSolucaoInicialComArvores(int z, int numeroDeArvores);
};

