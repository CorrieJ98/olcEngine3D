#pragma once

#include"interactable.h"

#ifndef DOOR_H
#define DOOR_H


class Door
{
public:
	Door(bool, bool, std::string,int);

	bool UnlockDoor(std::string);	// take key id string
	void OpenDoor(bool);			// check if unlocked

	~Door();
	
	bool _GetIsLocked();
	bool _GetIsRemote();
	int _GetRemoteChannel();
	std::string _GetPairedKeyID();	// return id needed to unlock this door

private:
	bool isLocked;
	bool isRemote;
	int remote_channel;
	std::string key_id;
};

class Key
{
public:
	Key(std::string,std::string);
	~Key();

	void CreateKey();

	std::string GenerateNewKeyID(int);
	std::string _GetKeyName();
	std::string _GetKeyID();

private:
	std::string name;
	std::string id;


};
#endif // !DOOR_H
