#include <iostream>
#include <json/json.h>
#include <fstream>
#include <math.h>

//mede tempo de execucao
#include<chrono> 

//rand
#include <stdlib.h>
#include <time.h>

#include "classes/Item.h"
#include "classes/Recipiente.h"
#include "classes/Nodo.h"

using namespace std;

vector<Item> Itens;
vector<Recipiente> Recipientes;
vector<Nodo> Arvores;

vector<Item> ItensOri;
vector<Recipiente> RecipientesOri;
vector<Nodo> ArvoresOri;

void converteDeJsonParaObjeto(Json::Value dadosDoJson, bool ehConversaoDeItem)
{
	int codigoDoItem = 1;
	for (int i = 0; i < dadosDoJson.size(); i++) {
		Json::Value objeto = dadosDoJson[i];

		if (ehConversaoDeItem)
		{
			int demand = objeto["Demand"].asInt64();
			for (int i = 0; i < demand; i++)
			{
				Item novoItem(objeto["Height"].asInt64(), objeto["Length"].asInt64(), demand, objeto["Value"].asInt64(), codigoDoItem);
				Itens.push_back(novoItem);
				codigoDoItem++;
			}
		}
		else
		{
			Recipiente novoRecipiente(objeto["Height"].asInt64(), objeto["Length"].asInt64(), objeto["Cost"].asInt64(), objeto["Reference"].asInt64());
			Recipientes.push_back(novoRecipiente);
		}
	}
}

bool AdicionaItem(Item itemEscolhido, Nodo& raizAtual, Nodo& raizAnterior)
{
	if (raizAtual.Children.empty() && raizAtual.Type == TipoDeNodo::Leftover) {//adiciona item
		Nodo novoFilho(itemEscolhido.Height, itemEscolhido.Length, itemEscolhido.Reference);

		if (novoFilho.Height <= raizAtual.Height && novoFilho.Length <= raizAtual.Length)//item cabe
		{
			int sobraDoCorteHorizontal = (raizAtual.Height - novoFilho.Height) * raizAtual.Length; // ---
			int sobraDoCorteVertical = (raizAtual.Length - novoFilho.Length) * raizAtual.Height; // |

			int alturaDaRaiz = raizAtual.Height;
			int larguraDaRaiz = raizAtual.Length;

			if (novoFilho.Height < raizAtual.Height && novoFilho.Length < raizAtual.Length)  //CORTE 2 NIVEIS
			{

				bool primeiroCorteEhHorizontal = sobraDoCorteHorizontal > sobraDoCorteVertical;

				TipoOrientacao orientacaoDoPrimeiroCorte = primeiroCorteEhHorizontal ? TipoOrientacao::H : TipoOrientacao::V;
				TipoOrientacao orientacaoDoSegundoCorte = primeiroCorteEhHorizontal ? TipoOrientacao::V : TipoOrientacao::H;

				Nodo segundoResto(0, 0);

				int alturaDoSegundoResto, larguraDoSegundoResto;

				if (primeiroCorteEhHorizontal) // resto verdadeiro
				{
					raizAtual.Height -= novoFilho.Height;

					segundoResto.Height = alturaDoSegundoResto = novoFilho.Height;
					segundoResto.Length = larguraDoSegundoResto = raizAtual.Length;
					segundoResto.Length -= novoFilho.Length;

				}
				else
				{
					raizAtual.Length -= novoFilho.Length;

					segundoResto.Height = alturaDoSegundoResto = raizAtual.Height;
					segundoResto.Length = larguraDoSegundoResto = novoFilho.Length;
					segundoResto.Height -= novoFilho.Height;
				}

				vector<Nodo> children, children2;

				children2.push_back(novoFilho);
				children2.push_back(segundoResto);

				Nodo segundoCorte(alturaDoSegundoResto, larguraDoSegundoResto, orientacaoDoSegundoCorte, children2);

				if (raizAnterior.Orientation == orientacaoDoPrimeiroCorte) //caso tenha 2 cortes iguais em sequencia, apenas ajusta o tamanho do resto e adiciona filho na lista.
				{
					raizAnterior.Children.push_back(segundoCorte);
					return true;
				}

				children.push_back(segundoCorte);
				children.push_back(raizAtual);

				Nodo novaRaiz(alturaDaRaiz, larguraDaRaiz, orientacaoDoPrimeiroCorte, children);

				raizAtual = novaRaiz;

				return true;
			}

			else //corte 1 nivel
			{
				TipoOrientacao orientacao
					= sobraDoCorteHorizontal > sobraDoCorteVertical
					? TipoOrientacao::H : TipoOrientacao::V;

				if (orientacao == TipoOrientacao::H)
					raizAtual.Height -= novoFilho.Height;
				else
					raizAtual.Length -= novoFilho.Length;

				if (raizAtual.Height == 0 || raizAtual.Length == 0)
				{
					raizAtual = novoFilho;
					return true;
				}

				if (raizAnterior.Orientation == orientacao) //caso tenha 2 cortes iguais em sequencia, apenas ajusta o tamanho do resto e adiciona filho na lista.
				{
					raizAnterior.Children.push_back(novoFilho);
					return true;
				}

				vector<Nodo> children;

				children.push_back(novoFilho);
				children.push_back(raizAtual);

				Nodo novaRaiz(alturaDaRaiz, larguraDaRaiz, orientacao, children);

				raizAtual = novaRaiz;

				return true;
			}
		}
		return false;
	}
	else
	{
		for (int i = 0; i < raizAtual.Children.size(); i++)
		{
			if (AdicionaItem(itemEscolhido, raizAtual.Children[i], raizAtual)) return true;
		}
	}

	return false;
}

void AjustaIndexDaArvoreNoItemAdicionado(int codigoDoItem, vector<Item>& itens, int indexDaArvore = -1)
{
	for (int i = 0; i < itens.size(); i++)
		if (itens[i].Reference == codigoDoItem)
			itens[i].IndexDaArvore = indexDaArvore;
}


void GerenciaProcessoDeAdicaoDeItens(vector<Item>& itensFaltantes, vector<Nodo>& arvoresLocal, vector<Item>& itensLocal)
{
	int alturaDoRecipiente = Recipientes[0].Height;
	int larguraDoRecipiente = Recipientes[0].Height;

	Nodo arvore(alturaDoRecipiente, larguraDoRecipiente);
	arvoresLocal.push_back(arvore);

	while (!itensFaltantes.empty())
	{
		int indexDoItem = rand() % itensFaltantes.size();

		Item itemEscolhido = itensFaltantes[indexDoItem];

		bool adicionou = false;

		for (int i = 0; i < arvoresLocal.size(); i++)
		{
			if (AdicionaItem(itemEscolhido, arvoresLocal[i], arvoresLocal[i])) {
				itensFaltantes.erase(itensFaltantes.begin() + indexDoItem);
				adicionou = true;
				AjustaIndexDaArvoreNoItemAdicionado(itemEscolhido.Reference, itensLocal, i);
				break;
			}
		}

		if (!adicionou)
		{
			Nodo novaArvore(alturaDoRecipiente, larguraDoRecipiente);

			if (AdicionaItem(itemEscolhido, novaArvore, novaArvore)) {
				itensFaltantes.erase(itensFaltantes.begin() + indexDoItem);
				AjustaIndexDaArvoreNoItemAdicionado(itemEscolhido.Reference, itensLocal, arvoresLocal.size());
			}

			arvoresLocal.push_back(novaArvore);
		}

	}
}

void AjustaArvoreAposARemocao(Nodo& raiz)
{
	int numeroDeRestos = 0, numeroDeNaoRestos = 0;
	for (int i = 0; i < raiz.Children.size(); i++)
	{
		switch (raiz.Children[i].Type)
		{
		case TipoDeNodo::Leftover:
			numeroDeRestos++;
			break;
		default:
			numeroDeNaoRestos++;
			break;
		}
	}

	if (numeroDeRestos <= 1) //resto que acabei de transformar
		return;

	if (numeroDeRestos == 2 && numeroDeNaoRestos >= 1) //soma restos e tranforma em apenas 1
	{
		bool ehOPrimeiroResto = true;
		int indiceDoPrimeiroResto;
		Nodo primeiroResto(0, 0), segundoResto(0, 0);

		for (int i = 0; i < raiz.Children.size(); i++)
		{
			if (raiz.Children[i].Type == TipoDeNodo::Leftover)
			{
				if (ehOPrimeiroResto)
				{
					primeiroResto = raiz.Children[i];
					indiceDoPrimeiroResto = i;

					ehOPrimeiroResto = false;
				}
				else
				{
					if (raiz.Orientation == TipoOrientacao::H)
						raiz.Children[i].Height += primeiroResto.Height;
					else
						raiz.Children[i].Length += primeiroResto.Length;

					raiz.Children.erase(raiz.Children.begin() + indiceDoPrimeiroResto); //remove primeiro resto
					return;
				}
			}
		}
	}

	raiz.TransformaEmResto();

}

bool RemoveItem(int codigoDoItem, Nodo& raizAtual, vector<Item>& itens) //fazer remoção em 2 etapas, primeiro tranforma esse item em resto e depois normaliza a arvore
{
	if (raizAtual.Children.empty() && raizAtual.CodigoDoItem == codigoDoItem)
	{
		AjustaIndexDaArvoreNoItemAdicionado(raizAtual.CodigoDoItem, itens);
		raizAtual.TransformaEmResto();

		return true;
	}
	else
	{
		for (int i = 0; i < raizAtual.Children.size(); i++)
		{
			if (RemoveItem(codigoDoItem, raizAtual.Children[i], itens)) {
				AjustaArvoreAposARemocao(raizAtual);
				return true;
			}
		}
	}
	return false;
}


int VerificaUtilizacaoDaArvore(Nodo raizAtual)
{
	if (raizAtual.Type == TipoDeNodo::Item)
	{
		return raizAtual.Height * raizAtual.Length;
	}
	else
	{
		int valor = 0;
		for (int i = 0; i < raizAtual.Children.size(); i++)
		{
			valor += VerificaUtilizacaoDaArvore(raizAtual.Children[i]);
		}

		return valor;
	}
}

double AvaliaSolucao(vector<Nodo> arvores, int k)
{
	double somaDaUtilizacaoDasArvores = 0;
	for (Nodo arvore : arvores)
	{
		double utilizacao = pow((double)VerificaUtilizacaoDaArvore(arvore) / (arvore.Height * arvore.Length), k);
		somaDaUtilizacaoDasArvores += utilizacao;
	}

	return somaDaUtilizacaoDasArvores / arvores.size();
}

void AdicionaItemAEsquerda(vector<Nodo>& arvoresLocal, Item item, int& indexDaArvoreEmQueOItemFoiAdicionado)
{
	bool adicionou = false;
	for (int i = 0; i < arvoresLocal.size(); i++)
	{
		if (AdicionaItem(item, arvoresLocal[i], arvoresLocal[i])) {
			adicionou = true;
			indexDaArvoreEmQueOItemFoiAdicionado = i;
			break;
		}
	}

	if (!adicionou)
	{
		Nodo novaArvore(10, 10);
		AdicionaItem(item, novaArvore, novaArvore);
		arvoresLocal.push_back(novaArvore);
		indexDaArvoreEmQueOItemFoiAdicionado = arvoresLocal.size() - 1;
	}
}


void AdicionaItemNaArvoreECasoNaoConsigaAdicionaAEsquerda(Item itemParaAdicionar, Nodo& arvore, vector<Nodo>& arvoresLocal, int& indexDaArvoreEmQueOItemFoiAdicionado) {

	bool deuCerto = AdicionaItem(itemParaAdicionar, arvore, arvore);

	if (deuCerto) return;

	AdicionaItemAEsquerda(arvoresLocal, itemParaAdicionar, indexDaArvoreEmQueOItemFoiAdicionado);
}

void ItemShuffle(vector<Nodo>& arvoresLocal, vector<Item>& itensLocal)
{
	//Escolher 2 Arvores e 1 Item em cada Arvore
	int indexDoPrimeiroItem = rand() % Itens.size();
	int indexDoSegundoItem;
	do
	{
		indexDoSegundoItem = rand() % Itens.size();
	} while (Itens[indexDoPrimeiroItem].IndexDaArvore == Itens[indexDoSegundoItem].IndexDaArvore);

	//TROCA DE ARVORE
	Item primeiroItem = Itens[indexDoPrimeiroItem], segundoItem = Itens[indexDoSegundoItem];

	int indexDaPrimeiraArvore = primeiroItem.IndexDaArvore;
	int indexDaSegundaArvore = segundoItem.IndexDaArvore;

	RemoveItem(primeiroItem.Reference, arvoresLocal[indexDaPrimeiraArvore], itensLocal);
	RemoveItem(segundoItem.Reference, arvoresLocal[indexDaSegundaArvore], itensLocal);

	int indexDaArvoreAdicionada1 = indexDaPrimeiraArvore, indexDaArvoreAdicionada2 = indexDaSegundaArvore;
	AdicionaItemNaArvoreECasoNaoConsigaAdicionaAEsquerda(primeiroItem, arvoresLocal[indexDaSegundaArvore], arvoresLocal, indexDaArvoreAdicionada2);
	AdicionaItemNaArvoreECasoNaoConsigaAdicionaAEsquerda(segundoItem, arvoresLocal[indexDaPrimeiraArvore], arvoresLocal, indexDaArvoreAdicionada1);

	itensLocal[indexDoPrimeiroItem].IndexDaArvore = indexDaArvoreAdicionada2;
	itensLocal[indexDoSegundoItem].IndexDaArvore = indexDaArvoreAdicionada1;
}


void ObtemAPosicaoDosItensDaArvoreNaListaDeItensArvore(Nodo raiz, vector<int>& posicaoDosItens)
{
	if (raiz.Type == TipoDeNodo::Item)
	{
		for (int i = 0; i < Itens.size(); i++)
		{
			if (Itens[i].Reference == raiz.CodigoDoItem)
				posicaoDosItens.push_back(i);
		}
	}
	else
	{
		for (int i = 0; i < raiz.Children.size(); i++)
		{
			ObtemAPosicaoDosItensDaArvoreNaListaDeItensArvore(raiz.Children[i], posicaoDosItens);
		}
	}
}

void RemoveArvoresVaziasDaLista(vector<Nodo>& arvoresLocal, int indexDaArvore) {
	if (arvoresLocal[indexDaArvore].Type == TipoDeNodo::Leftover)
		arvoresLocal.erase(arvoresLocal.begin() + indexDaArvore);
}

void BinShake(vector<Nodo>& arvoresLocal, vector<Item>& itensLocal)
{
	//salvar todos os itens q estão na arvore em uma lista 
	int indexDaArvore = rand() % Arvores.size();

	vector<int> posicoesDosItensDaArvore;
	ObtemAPosicaoDosItensDaArvoreNaListaDeItensArvore(arvoresLocal[indexDaArvore], posicoesDosItensDaArvore);

	for (int posicaoDoItem : posicoesDosItensDaArvore)
		RemoveItem(itensLocal[posicaoDoItem].Reference, arvoresLocal[indexDaArvore], itensLocal);

	while (!posicoesDosItensDaArvore.empty())
	{
		int indexDaArvoreAdicionada = indexDaArvore;

		int indexDaListaDePosicoes = rand() % posicoesDosItensDaArvore.size();

		int indexDoItemEscolhido = posicoesDosItensDaArvore[indexDaListaDePosicoes];

		AdicionaItemNaArvoreECasoNaoConsigaAdicionaAEsquerda(itensLocal[indexDoItemEscolhido], arvoresLocal[indexDaArvoreAdicionada], arvoresLocal, indexDaArvoreAdicionada);

		posicoesDosItensDaArvore.erase(posicoesDosItensDaArvore.begin() + indexDaListaDePosicoes);

		itensLocal[indexDoItemEscolhido].IndexDaArvore = indexDaArvoreAdicionada;
	}
}

//PRA DEBUGAR
vector<int> itensRep;
void VerificaSeTemItensRepetidos(Nodo raizAtual)
{
	if (raizAtual.Type == TipoDeNodo::Item)
	{
		itensRep.push_back(raizAtual.CodigoDoItem);
	}
	else
	{
		int valor = 0;
		for (int i = 0; i < raizAtual.Children.size(); i++)
		{
			VerificaSeTemItensRepetidos(raizAtual.Children[i]);
		}

	}
}

void Checa(vector<Nodo>arvores)
{
	itensRep.clear();
	for (Nodo arvore : arvores)
	{
		VerificaSeTemItensRepetidos(arvore);
	}
}

void Corrige(Nodo raizAtual, vector<Item>& itensLocal, int indice) {
	if (raizAtual.Type == TipoDeNodo::Item)
	{
		for (int i = 0; i < Itens.size(); i++) {
			if (raizAtual.CodigoDoItem == itensLocal[i].Reference) {
				itensLocal[i].IndexDaArvore = indice;
				return;
			}
		}
	}
	else
	{
		for (int i = 0; i < raizAtual.Children.size(); i++)
		{
			Corrige(raizAtual.Children[i], itensLocal, indice);
		}

	}
}

void CorrigeIndiceDasArvoresNosItens(vector<Nodo>& arvoresLocal, vector<Item>& itensLocal) {
	for (int i = 0; i < arvoresLocal.size(); i++) {
		Corrige(arvoresLocal[i], itensLocal, i);
	}
}

int AchaIndiceDaArvoreMenosUtilizada(vector<Nodo> arvoresLocal) {
	double menorvalor = 0;
	int indiceDaArvore = 0;
	for (int i = 0; i < arvoresLocal.size(); i++) {
		double utilizacao = VerificaUtilizacaoDaArvore(arvoresLocal[i]);
		if (utilizacao > menorvalor)
			indiceDaArvore = i;
	}
	return indiceDaArvore;
}
void BinDelete(vector<Nodo>& arvoresLocal, vector<Item>& itensLocal)
{
	//escolher um recipiente para remover os itens e realocar todos eles em outros..
	int indexDoRecipienteParaRemover = rand() % arvoresLocal.size();
	//int indexDoRecipienteParaRemover = AchaIndiceDaArvoreMenosUtilizada(arvoresLocal);
	//salvar em uma lista todos os itens que estão naquele recipiente e precisam ser realocados. == ja estao direto na lista de itens com o index.
	vector<int> indexDosItensDaArvore;
	ObtemAPosicaoDosItensDaArvoreNaListaDeItensArvore(arvoresLocal[indexDoRecipienteParaRemover], indexDosItensDaArvore);

	vector<Item> itensDaArvore;
	for (int i = 0; i < indexDosItensDaArvore.size(); i++)
		itensDaArvore.push_back(itensLocal[indexDosItensDaArvore[i]]);

	arvoresLocal.erase(arvoresLocal.begin() + indexDoRecipienteParaRemover);

	CorrigeIndiceDasArvoresNosItens(arvoresLocal, itensLocal);

	if (!itensDaArvore.empty())
		GerenciaProcessoDeAdicaoDeItens(itensDaArvore, arvoresLocal, itensLocal);
}

bool Existe(vector<Item> itensParaAlterar, Item novoItem)
{
	for (int i = 0; i < itensParaAlterar.size(); i++)
	{
		if (itensParaAlterar[i].Reference == novoItem.Reference)
			return true;
	}
	return false;
}

void Perturbacao(vector<Nodo>& arvoresLocal, vector<Item>& itensLocal, double porcentagemDePerturbacao)
{
	int totalDeItens = Itens.size();

	int numeroDeItensParaAlterar = round(totalDeItens * porcentagemDePerturbacao);

	if (numeroDeItensParaAlterar == 0)
		return;
	vector<Item> itensParaAlterar;
	for (int i = 0; i < numeroDeItensParaAlterar; i++)
	{
		int indexDoItemParaAlterar = rand() % totalDeItens;

		while (Existe(itensParaAlterar, Itens[indexDoItemParaAlterar]))
			indexDoItemParaAlterar = rand() % totalDeItens;

		itensParaAlterar.push_back(Itens[indexDoItemParaAlterar]);
	}
	//remove itens da arvore
	for (int i = 0; i < itensParaAlterar.size(); i++)
	{
		int indexDaArvore = itensParaAlterar[i].IndexDaArvore;
		RemoveItem(itensParaAlterar[i].Reference, arvoresLocal[indexDaArvore], itensLocal);
	}
	//recoloca itens na arvore
	GerenciaProcessoDeAdicaoDeItens(itensParaAlterar, arvoresLocal, itensLocal);
}

void BuscaLocal(vector<Nodo>& arvoresLocal, vector<Item>& itensLocal)
{
	int operacao = rand() % 3; // saber operação 0 - Item-Shuffle - 1 - Bin-Shake - 2 Mistura

	switch (operacao)
	{
	case 0:
		ItemShuffle(arvoresLocal, itensLocal);
		break;
	case 1:
		BinShake(arvoresLocal, itensLocal);
		break;
	case 2:
		BinDelete(arvoresLocal, itensLocal);
		break;
	}
}

bool HouveMelhora(vector<Nodo> novaSolucao, vector<Nodo> solucaoAtual, int k)
{
	double resultadoDaNovaSolucao = AvaliaSolucao(novaSolucao, k);
	double resultadoDaSolucaoAtual = AvaliaSolucao(solucaoAtual, k);

	if (resultadoDaNovaSolucao > resultadoDaSolucaoAtual)
		return true;
	return false;
}

void BuscaLocalIterada(int numeroDeIteracoes, double porcentagemDePerturbacao, int k)
{
	int cont = 0;
	BuscaLocal(Arvores, Itens);
	while (cont < numeroDeIteracoes) {
		vector<Nodo> arvoresLocal(Arvores);
		vector<Item> itensLocal(Itens);

		if (cont > 0)
			Perturbacao(arvoresLocal, itensLocal, porcentagemDePerturbacao);
		BuscaLocal(arvoresLocal, itensLocal);
		bool houveMelhora = HouveMelhora(arvoresLocal, Arvores, k);

		if (houveMelhora)
		{
			Arvores = arvoresLocal;
			Itens = itensLocal;
			cont = 0;
		}
		else {
			cont++;
		}

		// cout << "utilizacao: " << AvaliaSolucao(Arvores, 2) << "\t cont:" << cont << endl;
	}
}

#include <algorithm>
#include<stdio.h>
#include<stdlib.h>
#include "CrowSearch.h"

void CriaArquivoDeTextoCrowSearch(int numeroDoTeste, Crow corvo, double tempo, ofstream& arq, string nome, int maxIteration) {
	arq << "Arquivo," << nome << endl;
	arq << "Execucao," << numeroDoTeste << endl;
	arq << "MaxIteration," << maxIteration << endl;
	arq << "Tempo Decorrido (segundos)," << tempo << endl;
	arq << "Porcentagem De Utilizacao," << corvo.AvaliacaoDaMelhorSolucao << endl;
	arq << "Quantidade de Recipientes," << corvo.MelhorMemoria[0].size() << endl;

	/*for (int j = 0; j < corvo.MelhorMemoria.size(); j++) {
		for (int k = 0; k < corvo.MelhorMemoria[j].size(); k++) {
			if (k == corvo.MelhorMemoria[j].size() - 1)
				arq << corvo.MelhorMemoria[j][k];
			else
				arq << corvo.MelhorMemoria[j][k] << ",";
		}
		arq << endl;
	}*/
}

void CriaArquivoDeTextoBuscaLocalIterada(int numeroDoTeste, double tempo, ofstream& arq) {
	arq << "Execucao," << numeroDoTeste << endl;
	arq << "Tempo Decorrido (segundos)," << tempo << endl;
	arq << "Porcentagem De Utilizacao," << AvaliaSolucao(Arvores, 2) << endl << endl;
	arq << "Quantidade de Recipientes," << Arvores.size() << endl;
}

//#include <mutex>
//std::mutex file_mutex;

void CrowSearch(int repeticoes, ofstream& arq, string nome)
{
	for (int i = 0; i < repeticoes; i++) {
		vector<int> max = { 1, 20, 50, 80, 100 };
			for (int m = 0; m < max.size(); m++) {
			arq << "Busca do Corvo" << endl;
			int z = 2;
			int flightsize = 2;
			double AP = 0.1;
			int flocksize = 50;
			CarregaVariaveis(Itens, Recipientes);
			InicializaCorvos(flocksize, z);
			auto start = chrono::steady_clock::now();
			Crow corvo = CrowSearch(flightsize, AP, z, m);
			auto end = chrono::steady_clock::now();
			auto elapsed = end - start;
			double tempo = ((double)elapsed.count() / 1000000000);
			CriaArquivoDeTextoCrowSearch(i, corvo, tempo, arq, nome, m);
		}
	}
}

void IteratedLocalSearch(int repeticao) {
	ofstream arq("execucaoILS.csv");
	if (arq.is_open())
	{
		double perturbacao = 0.0;
		int z = 2;
		int maxIteracoes = 150;

		for (int i = 0; i < repeticao; i++) {
			Recipientes = RecipientesOri;
			Itens = ItensOri;
			Arvores = ArvoresOri;
			vector<Item> itensFaltantes = Itens;
			GerenciaProcessoDeAdicaoDeItens(itensFaltantes, Arvores, Itens);
			auto start = chrono::steady_clock::now();
			BuscaLocalIterada(maxIteracoes, perturbacao, z);

			auto end = chrono::steady_clock::now();
			auto elapsed = end - start;
			double tempo = ((double)elapsed.count() / 1000000000);
			CriaArquivoDeTextoBuscaLocalIterada(i, tempo, arq);
		}
		arq.close();
	}
}

int main()
{
	srand(time(NULL));

	string classe = "CLASS01";
	vector<string> numeroDeItens = { "020","040" ,"060" ,"080" ,"100" };
	vector<string> instancia = { "01","02", "03", "04", "05", "06", "07", "08", "09", "10" };

	ofstream arqbcsa("execucaoBCSA.csv");
	if (arqbcsa.is_open())
	{
		for (int n = 0; n < numeroDeItens.size(); n++) {
			for (int i = 0; i < instancia.size(); i++) {

				Itens.clear();
				Arvores.clear();
				Recipientes.clear();

				string nome = "base\\" + classe + "_" + numeroDeItens[n] + "_" + instancia[i] + ".json";
				ifstream base_file(nome, ifstream::binary);
				Json::Value dadosDoJson;
				base_file >> dadosDoJson;

				converteDeJsonParaObjeto(dadosDoJson["Items"], true);
				converteDeJsonParaObjeto(dadosDoJson["Objects"], false);

				CrowSearch(10, arqbcsa, nome);
			}
		}
	}
	arqbcsa.close();

	return 0;
}
