#include "Nodo.h"

Nodo::Nodo(int height, int length)
	: Height(height), Length(length)
{
	this->CodigoDoItem = -1;
	this->Type = TipoDeNodo::Leftover;
}

Nodo::Nodo(int height, int length, vector<Nodo> children)
	: Height(height), Length(length), Children(children)
{
	this->CodigoDoItem = -1;
	this->Type = TipoDeNodo::Structure;
}

Nodo::Nodo(int height, int length, int codigoDoItem)
	: Height(height), Length(length), CodigoDoItem(codigoDoItem)
{
	this->Type = TipoDeNodo::Item;
}

void Nodo::criaItem(int height, int length, int codigoDoItem)
{
	Nodo novoItem(height, length, codigoDoItem);
	Children.push_back(novoItem);
}

void Nodo::criaEstrutura(int height, int length, vector<Nodo> children)
{
	Nodo novaEstrutra(height, length);
	Children.push_back(novaEstrutra);
}

void Nodo::criaSobra(int height, int length)
{
	Nodo novaSobra(height, length);
	Children.push_back(novaSobra);
}