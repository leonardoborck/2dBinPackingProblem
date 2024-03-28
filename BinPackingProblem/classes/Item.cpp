#include "Item.h"

Item::Item(int height, int length, int demand, int value, int reference)
	:Height(height), Length(length), Demand(demand), Value(value), Reference(reference)
{
	this->IndexDaArvore = -1;
}