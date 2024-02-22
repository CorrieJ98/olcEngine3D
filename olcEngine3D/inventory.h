#ifndef INVENTORY_H
#define INVENTORY_H

#include "item.cpp"

class Inventory
{
public:
	Inventory(std::vector<int,int>);

	void AddItemToInventory(std::string, std::vector<int,int>);
	void RemoveItemFromInventory(std::string);
	void MoveInInventory();
	void RotateInInventory();

private:
	std::vector<int, int> inventory = { 16,16 };
};
#endif // !INVENTORY_H