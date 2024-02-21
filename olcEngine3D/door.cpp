#include "door.h"

const int id_length = 8;


Door::Door(bool t_isLocked, bool t_isRemote, int t_remote_channel, std::string t_key_id) {
	isLocked = t_isLocked;
	isRemote = t_isRemote;
	remote_channel = t_remote_channel;
	key_id = t_key_id;
}

Key::Key(std::string t_name, std::string t_id) {
	name = t_name;
	id = t_id;
}

void Key::CreateKey() {
}
//
//std::string Key::GenerateNewKeyID(int length) {
//	INTERACTABLE_ITEM_H::char_set;
//
//	// generate a random 8 digit alphanumeric key
//}