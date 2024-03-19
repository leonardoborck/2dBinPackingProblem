#include "Nodo.h"

Nodo::Nodo(int height, int length)
	: Height(height), Length(length)
{
	this->CodigoDoItem = -1;
	this->Type = TipoDeNodo::Leftover;
}

Nodo::Nodo(int height, int length, TipoOrientacao orientation, vector<Nodo> children)
	: Height(height), Length(length), Orientation(orientation), Children(children)
{
	this->CodigoDoItem = -1;
	this->Type = TipoDeNodo::Structure;
}

Nodo::Nodo(int height, int length, int codigoDoItem)
	: Height(height), Length(length), CodigoDoItem(codigoDoItem)
{
	this->Type = TipoDeNodo::Item;
}

void Nodo:: TransformaEmResto() {
	this->Type = TipoDeNodo::Leftover;
	this->Orientation = TipoOrientacao::NULO;
	this->CodigoDoItem = -1;
	this->Children.clear();
}