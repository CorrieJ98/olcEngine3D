#ifndef GAMECHARACTER_H
#define GAMECHARACTER_H

#include <string>

class GameCharacter
{
public:
	GameCharacter(std::string, int, int, float);

	void TakeDamage(int);
	void Heal(int);
	bool IsAliveCheck();

	int _GetHealth();
	int _GetDamage();
	float _GetMoveSpeed();
	std::string _GetName();

	~GameCharacter();

protected:
	int maxHP, curHP, damage;
	float moveSpeed;
private:
	std::string name;
};

#endif
