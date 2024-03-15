#include <iostream>
#include <json/json.h>
#include <fstream>

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

void converteDeJsonParaObjeto(Json::Value dadosDoJson, bool ehConversaoDeItem)
{
	for (int i = 0; i < dadosDoJson.size(); i++) {
		Json::Value objeto = dadosDoJson[i];

		if (ehConversaoDeItem)
		{
			Item novoItem(objeto["Height"].asInt64(), objeto["Length"].asInt64(), objeto["Demand"].asInt64(), objeto["Value"].asInt64(), objeto["Reference"].asInt64());
			Itens.push_back(novoItem);
		}
		else
		{
			Recipiente novoRecipiente(objeto["Height"].asInt64(), objeto["Length"].asInt64(), objeto["Cost"].asInt64(), objeto["Reference"].asInt64());
			Recipientes.push_back(novoRecipiente);
		}
	}
}

bool AdicionaItem(Nodo novoFilho, Nodo& raizAtual, Nodo& raizAnterior)
{
	if (raizAtual.Children.empty() && raizAtual.Type == TipoDeNodo::Leftover) {//adiciona item
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
			if (AdicionaItem(novoFilho, raizAtual.Children[i], raizAtual)) return true;
		}
	}

	return false;
}

void GerenciaProcessoDeAdicaoDeItens(vector<Item>& itensFaltantes)
{
	int alturaDoRecipiente = Recipientes[0].Height;
	int larguraDoRecipiente = Recipientes[0].Height;

	Nodo arvore(alturaDoRecipiente, larguraDoRecipiente);
	Arvores.push_back(arvore);

	while (!itensFaltantes.empty())
	{
		int indexDoItem = itensFaltantes.size() != 1 ? rand() % (itensFaltantes.size() - 1) : 0;

		Item itemEscolhido = itensFaltantes[indexDoItem];

		Nodo novoFilho(itemEscolhido.Height, itemEscolhido.Length, itemEscolhido.Reference);

		bool adicionou = false;

		for (int i = 0; i < Arvores.size(); i++)
		{
			if (AdicionaItem(novoFilho, Arvores[i], Arvores[i])) {
				itensFaltantes.erase(itensFaltantes.begin() + indexDoItem);
				adicionou = true;
				break;
			}
		}

		if (!adicionou)
		{
			Nodo novaArvore(alturaDoRecipiente, larguraDoRecipiente);

			if (AdicionaItem(novoFilho, novaArvore, novaArvore))
				itensFaltantes.erase(itensFaltantes.begin() + indexDoItem);

			Arvores.push_back(novaArvore);
		}

	}
}

int main()
{
	srand(time(NULL));

	ifstream base_file("base\\CLASS01_020_01.json", ifstream::binary);

	Json::Value dadosDoJson;
	base_file >> dadosDoJson;

	converteDeJsonParaObjeto(dadosDoJson["Items"], true);
	converteDeJsonParaObjeto(dadosDoJson["Objects"], false);

	vector<Item> itensFaltantes = Itens;

	GerenciaProcessoDeAdicaoDeItens(itensFaltantes);

	return 0;
}