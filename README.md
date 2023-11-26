# C++ Bit Serializer

A wrapper for a vector of bytes (`ByteArray`) that allows easy bit-level manipulations for use in databases, real-time multiplayer, or anything where byte size is a concern. This is just my little creation and I don't recommend its use in production projects, I haven't thoroughly tested and vetted it.

## Example

Although this is C++, think of it as pseudo-code. I haven't actually compiled this, so it could have an error.

```c++
#include "serializer.hpp"

...

void serializePlayer(Serializer* arr, Player player) {
	arr->addBit(player.dead);
	arr->addFloat(player.pos.x); // or addDouble(double)
	arr->addFloat(player.pos.y);
	arr->addString(player.name);

	// Dead players don't have health or weapons, do they?
	if (!player.dead) {
		arr->addUint(player.health, 7); // Max health is 100, which can fit in 7 bits (2^7 = 128)
		arr->addUint(player.curWeapId, 5); // Only 30 weapons in the game, which can fit in 5 bits.
		arr->addBit(player.isDeveloper); // Special developer-only skin?
		arr->addInt(player.someInt, 12); // Couldn't think of an int property a player would have :)
	}
}

Serializer::ByteArray serializePlayers(PlayerVector players) {
	Serializer arr;

	arr.addUint(game.version, 8); // make sure client and server are compatible
	arr.addUint(players.length, 6); // max players is 50, which fits in 6 bits

	for (int i = 0; i < players.size(); ++i) {
		serializePlayer(&arr, players[i]);
	}

	return arr.byteArray;
}

Player deserializePlayer(Serializer* barr) {
	Player player;

	// basically the serialization code, but "read" instead of "add"

	player.dead = arr->readBit();
	player.pos.x = arr->readFloat();
	player.pos.y = arr->readFloat();
	player.name = arr->readString();

	if (!player.dead) {
		player.health = arr->readUint(7);
		player.curWeapId = arr->readUint(5);
		player.isDeveloper = arr->readBit();
		player.someInt = arr->readInt(12);
	}

	return player;
}

PlayerVector deserializePlayers(Serializer::ByteArray barr) {
	Serializer arr(barr);

	PlayerVector players;

	auto version = arr.readUint(8);
	auto length = arr.readUint(6);

	for (int i = 0; i < length; ++i) {
		players.push_back(deserializePlayer(&arr));
	}

	return players;
}
```

## Why?

If you have a boolean value, why send it as 8 bits? If you know an integer will only ever go up to a certain value, why serialize it as something that can be way larger? This C++ code makes size optimization for messages and data easier. Why not?

## The ByteArray

This code inputs and outputs a `Serializer::ByteArray` (under the hood it is an `std::vector<uint8_t>`, a vector of bytes). I suppose it could just be an `std::string`, but it is also not too hard to convert between the two.
