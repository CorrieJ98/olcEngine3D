#pragma once
#include "interactable.h"
#include "gamecharacter.h"
#include "matrix.cpp"
#include "inventory.h"

class Player : GameCharacter{
public:
	Player();

	void Interact(InteractableObject);
	void PickUp(EquippableObject);
	

protected:

private:

};