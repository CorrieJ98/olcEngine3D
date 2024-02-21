#include "door.h"

const int id_length = 8;

//Door
Door::Door(bool t_isLocked, bool t_isRemote,std::string t_key_id, int t_remote_channel) {
	isLocked = t_isLocked;
	isRemote = t_isRemote;
	remote_channel = t_remote_channel;
	key_id = t_key_id;
}


bool Door::UnlockDoor(std::string key_id) {

}

void Door::OpenDoor(bool isLocked) {

}

// Key
Key::Key(std::string t_name, std::string t_id) {
	name = t_name;
	id = t_id;
}

void Key::CreateKey() {
}
std::string Key::GenerateNewKeyID(int length) {
	std::string a;
	INTERACTABLE_ITEM_H::char_set;


	return a;

	// generate a random 8 digit alphanumeric key
}