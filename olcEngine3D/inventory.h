#pragma once

struct item {
	std::string name;
	int damage, health;

	std::vector<int> size;
};


class Inventory{
public:
	Inventory(std::vector<int,int>);

	void AddItemToInventory(std::string, std::vector<int,int>);
	void RemoveItemFromInventory(std::string);
	void MoveInInventory();
	void RotateInInventory();

private:
	std::vector<int, int> inventory = { 16,16 };
};

