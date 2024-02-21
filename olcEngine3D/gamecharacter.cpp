#include "gamecharacter.h"
#include <string>

#ifndef GAME_CHARACTER_CPP
#define GAME_CHARACTER_CPP


GameCharacter::GameCharacter(std::string t_name, int t_health, int t_dmg, float t_movespeed) {
	name = t_name;
	maxHP = t_health;
	curHP = t_health;
	damage = t_dmg;
	moveSpeed = t_movespeed;
}

std::string GameCharacter::_GetName() { return name; }
int GameCharacter::_GetDamage() { return damage; }
int GameCharacter::_GetHealth() { return curHP; }
float GameCharacter::_GetMoveSpeed() { return moveSpeed; }

void GameCharacter::TakeDamage(int damage) { curHP -= damage; }
bool GameCharacter::IsAliveCheck() { return curHP > 0; }

#endif // !GAME_CHARACTER_CPP