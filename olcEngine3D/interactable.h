#pragma once
#include <string>
#ifndef INTERACTABLE_ITEM_H
#define INTERACTABLE_ITEM_H


const char char_set[] = {
"0123456789"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz" };

class EquippableObject		// equippables: weapons, medpacks, funny hats
{
public:
	EquippableObject();

	bool IsEquippedCheck();

	~EquippableObject();
protected:
	int damage, health, protection;
private:
	std::string name;
};

class EnvironmentObject		// physics objects: red barrels, breakable boxes, kickable chickens
{
public:
	EnvironmentObject();
	~EnvironmentObject();

private:

};

class InteractableObject	// buttons, doors, levers: things to press F to interact
{
public:
	InteractableObject();

	void ActivateOnPlayerInteraction();

	~InteractableObject();

private:

};

#endif // !INTERACTABLE_ITEM_H
