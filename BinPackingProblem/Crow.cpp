#include "Crow.h"

Crow::Crow(vector<Item> itens, vector<Recipiente> recipientes)
	: Itens(itens), RecipientesAtual(recipientes), RecipientesAnterior(recipientes) {}

void Crow::GeraSolucaoInicialAleatoria() {
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
	MemoriaAtual = MemoriaAnterior = MelhorMemoria = solucao;
}

//verifica utilizacao do recipiente
// itens soma( item altura x largura ) / recipiente altura x largura
void Crow::CorrigeSolucoesInviaveis() {
	vector<int> itensRemovidos;
	for (int k = 0; k < RecipientesAtual.size(); k++) {
		RemoveItensDosRecipientesSobrecarregados(k, itensRemovidos);
		ReinsereItensUsandoLeftBottomPolicy(itensRemovidos);
	}
}

double Crow::VerificaUtilizacaoDoRecipiente(int indexDoRecipiente, vector<vector<int>> solucao) {
	double somaItens = 0;
	for (int j = 0; j < Itens.size(); j++) {
		if (solucao[j][indexDoRecipiente] == 1) {
			somaItens += Itens[j].Height * Itens[j].Length;
		}
	}

	return somaItens / (RecipientesAtual[indexDoRecipiente].Height * RecipientesAtual[indexDoRecipiente].Length);
}

void Crow::RemoveItensDosRecipientesSobrecarregados(int indexDoRecipiente, vector<int> itensRemovidos) {
	while (VerificaUtilizacaoDoRecipiente(indexDoRecipiente, MemoriaAtual) > 1){
		int indexDoItemParaRemover = rand() % Itens.size();
		MemoriaAtual[indexDoItemParaRemover][indexDoRecipiente] = 0;
		itensRemovidos.push_back(indexDoItemParaRemover);
	}
}

void Crow::ReinsereItensUsandoLeftBottomPolicy(vector<int> itensRemovidos){
	for (int itemRemovido : itensRemovidos) {
		for (int k = 0; k < RecipientesAtual.size(); k++) {
			MemoriaAtual[itemRemovido][k] = 1;
			if (VerificaUtilizacaoDoRecipiente(itemRemovido, MemoriaAtual) > 1) {
				MemoriaAtual[itemRemovido][k] = 0;
				continue;
			}
			break;
		}

		//nao colocou em nenhum recipiente precisa criar um novo.
		RecipientesAtual.push_back(RecipientesAtual[0]); //alterar isso depois para o caso de tamanhos diferentes de recipientes

		for (int j = 0; j < Itens.size(); j++) {
			if (itemRemovido == j)
				MemoriaAtual[j].push_back(1);
			else
				MemoriaAtual[j].push_back(0);
		}
	}
}
