#include <string>
#include <vector>

#ifndef ITEM_CPP
#define ITEM_CPP

struct item {
	std::string name;
	int damage, health;

	std::vector<int, int> size;
};
#endif // !ITEM_H
