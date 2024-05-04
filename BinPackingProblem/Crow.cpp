#include "Crow.h"
#include <algorithm>

Crow::Crow(vector<Item> itens, vector<Recipiente> recipientes)
	: Itens(itens), RecipientesAtual(recipientes), MelhorRecipientes(recipientes) {}

void Crow::GeraSolucaoInicialComArvores(int z, int numeroDeArvores) {
	vector<vector<int>> solucao;
	vector<int> recipientes;
	recipientes.push_back(0);
	for (int k = 0; k < numeroDeArvores -1; k++) {
		recipientes.push_back(0);
		RecipientesAtual.push_back(RecipientesAtual[0]);
	}
	for (int i = 0; i < Itens.size(); i++) {
		solucao.push_back(recipientes);
	}
	for (int j = 0; j < solucao.size(); j++) {
		solucao[j][Itens[j].IndexDaArvore] = 1;
	}
	//CorrigeSolucoesInviaveis(solucao, RecipientesAtual);
	MemoriaAtual = MelhorMemoria = solucao;
	MelhorRecipientes = RecipientesAtual;
	AvaliacaoDaMelhorSolucao = AvaliaSolucao(MelhorMemoria, MelhorRecipientes, z);
}

void Crow::GeraSolucaoInicialAleatoria(int z) {
	//a ideia aqui é gerar solucoes onde informa onde cada item está em cada recipiente...
	//tempo 0
	vector<vector<int>> solucao;
	vector<int> recipientes;
	for (int i = 0; i < RecipientesAtual.size(); i++)
		recipientes.push_back(0); //valor inicial
	for (int i = 0; i < Itens.size(); i++) {
		solucao.push_back(recipientes);
	}

	for (int j = 0; j < solucao.size(); j++) {
		int qualBin = rand() % solucao[j].size();
		solucao[j][qualBin] = 1;
	}

	CorrigeSolucoesInviaveis(solucao, RecipientesAtual);
	MemoriaAtual = MelhorMemoria = solucao;
	MelhorRecipientes = RecipientesAtual;
	AvaliacaoDaMelhorSolucao = AvaliaSolucao(MelhorMemoria, MelhorRecipientes, z);
}

//verifica utilizacao do recipiente
// itens soma( item altura x largura ) / recipiente altura x largura
void Crow::CorrigeSolucoesInviaveis(vector<vector<int>> &solucao, vector<Recipiente> &recipientes) {
	vector<int> itensRemovidos;
	for (int k = 0; k < RecipientesAtual.size(); k++)
		RemoveItensDosRecipientesSobrecarregados(k, itensRemovidos, solucao, recipientes);
	ReinsereItensUsandoLeftBottomPolicy(itensRemovidos, solucao, recipientes);

	//remove recipiente vazio
	vector<int> recipientesParaRemover;
	for (int k = 0; k < RecipientesAtual.size(); k++) {
		bool temItem = false;
		for (int j = 0; j < solucao.size(); j++) {
			if (solucao[j][k] == 1) {
				temItem = true;
				break;
			}
		}
		if (!temItem)
			recipientesParaRemover.push_back(k);
	}
	//remove da lista
	sort(recipientesParaRemover.begin(), recipientesParaRemover.end(), greater<int>());
	for (int index : recipientesParaRemover) {
		recipientes.erase(recipientes.begin() + index);
		for (int j = 0; j < solucao.size(); j++) {
			solucao[j].erase(solucao[j].begin() + index);
		}
	}

}

double Crow::VerificaUtilizacaoDoRecipiente(int indexDoRecipiente, vector<vector<int>> solucao, vector<Recipiente> recipientes) {
	double somaItens = 0;
	for (int j = 0; j < Itens.size(); j++) {
		if (solucao[j][indexDoRecipiente] == 1) {
			somaItens += Itens[j].Height * Itens[j].Length;
		}
	}

	return somaItens/(recipientes[indexDoRecipiente].Height * recipientes[indexDoRecipiente].Length);
}

double Crow::AvaliaSolucao(vector<vector<int>> solucao, vector<Recipiente> recipientes, int z) { //verificar depois
	double soma = 0;
	for (int i = 0; i < recipientes.size(); i++)
		soma += pow(VerificaUtilizacaoDoRecipiente(i, solucao, recipientes), z);

	double numeroDeRecipientesUtilizados = recipientes.size();

	return soma / numeroDeRecipientesUtilizados;
}

bool Crow::HouveMelhora(int z) {
	double solucaoAtual = AvaliaSolucao(MemoriaAtual, RecipientesAtual, z);

	//cout << "Solucao ATUAL: " << solucaoAtual << endl;
	//cout << "Melhor Solucao: " << melhorSolucao << endl;
	//cout << endl;
	if (solucaoAtual <= AvaliacaoDaMelhorSolucao)
		return false;

	MelhorMemoria = MemoriaAtual;
	MelhorRecipientes = RecipientesAtual;
	AvaliacaoDaMelhorSolucao = solucaoAtual;

	return true;
}

bool Existe(vector<int> itensRemovidos, int item)
{
	for (int i = 0; i < itensRemovidos.size(); i++)
	{
		if (itensRemovidos[i] == item)
			return true;
	}
	return false;
}

vector<int> Crow::IndexDosItensNoRecipiente(int indexDoRecipiente, vector<vector<int>> solucao) {
	vector<int> indexDosItens;
	for (int j = 0; j < Itens.size(); j++) {
		if (solucao[j][indexDoRecipiente] == 1) {
			indexDosItens.push_back(j);
		}
	}
	return indexDosItens;
}

void Crow::RemoveItensDosRecipientesSobrecarregados(int indexDoRecipiente, vector<int> & itensRemovidos, vector<vector<int>> &solucao, vector<Recipiente> recipientes) {
	while (VerificaUtilizacaoDoRecipiente(indexDoRecipiente, solucao, recipientes) > 1){

		vector<int> indexDosItensNoRecipiente = IndexDosItensNoRecipiente(indexDoRecipiente, solucao);

		int indexDoindexDoItemParaRemover = rand() % indexDosItensNoRecipiente.size();
		while (Existe(itensRemovidos, indexDosItensNoRecipiente[indexDoindexDoItemParaRemover]))
			indexDoindexDoItemParaRemover = rand() % indexDosItensNoRecipiente.size();

		solucao[indexDosItensNoRecipiente[indexDoindexDoItemParaRemover]][indexDoRecipiente] = 0;
		itensRemovidos.push_back(indexDosItensNoRecipiente[indexDoindexDoItemParaRemover]);
	}
}

void Crow::ReinsereItensUsandoLeftBottomPolicy(vector<int> itensRemovidos, vector<vector<int>> &solucao, vector<Recipiente> &recipientes){
	for (int itemRemovido : itensRemovidos) {
		bool conseguiuAdicionar = false;
		for (int k = 0; k < RecipientesAtual.size(); k++) {
			solucao[itemRemovido][k] = 1;
			if (VerificaUtilizacaoDoRecipiente(k, solucao, recipientes) > 1) {
				solucao[itemRemovido][k] = 0;
				continue;
			}
			else {
				conseguiuAdicionar = true;
				break;
			}
		}

		if (conseguiuAdicionar)
			continue;

		//nao colocou em nenhum recipiente precisa criar um novo.
		recipientes.push_back(recipientes[0]); //alterar isso depois para o caso de tamanhos diferentes de recipientes

		for (int j = 0; j < Itens.size(); j++) {
			if (itemRemovido == j)
				solucao[j].push_back(1);
			else
				solucao[j].push_back(0);
		}
	}
}
