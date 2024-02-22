#pragma once
#include "interactable.h"
#include "gamecharacter.h"
#include "matrix.cpp"
#include "inventory.h"

#ifndef PLAYER_H
#define PLAYER_H

class Player : GameCharacter{
public:
	Player();

	void Interact(InteractableObject);
	void PickUp(EquippableObject);
	

protected:

private:

};


#endif // !PLAYER_H