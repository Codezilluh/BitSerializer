#include "serializer.hpp"

int main() {
	Serializer arr;

	arr.addBit(false);

	arr.addUint(654321, 60);

	arr.addInt(-1234, 30);

	arr.addFloat(123.45678);

	arr.addDouble(-123.45678);

	arr.addString("Hello world!");

	Serializer decoder(arr.byteArray);

	decoder.readBit();	// 0

	decoder.readUint(60);  // 654321

	decoder.readInt(30);  // -1234

	decoder.readFloat();  // 123.45678

	decoder.readDouble();  // -123.45678

	decoder.readString();  // Hello world!
}