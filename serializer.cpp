#include "serializer.hpp"

#include <cmath>
#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

Serializer::Serializer() {
	curBitPos = 0;
}

Serializer::Serializer(Serializer::ByteArray array) {
	byteArray = array;
	curBitPos = 0;
}

Serializer* Serializer::addBit(bool on) {
	// Find the byte that would contain bit number X in the byteArray
	uint bytePos = curBitPos >> 3;

	if (bytePos >= byteArray.size()) byteArray.push_back(0);

	if (on) {
		// enable bit at a specified position
		byteArray[bytePos] |= 1 << curBitPos % 8;
	} else {
		// disable bit at a specified position
		byteArray[bytePos] &= ~(1 << curBitPos % 8);
	}

	curBitPos++;

	return this;
}

Serializer* Serializer::addUint(uint64_t val, uint8_t bits) {
	for (uint8_t i = 0; i < bits; ++i) {
		bool bitAtPos = (val >> i) & 1;

		addBit(bitAtPos);
	}

	return this;
}

Serializer* Serializer::addInt(int64_t val, uint8_t bits) {
	// An int is just a shifted uint.

	if (val >= 0) {
		addBit(false);
		addUint(val, bits - 1);
	} else {
		addBit(true);
		addUint(-val - 1, bits - 1);  // no need for -0 and +0
	}

	return this;
}

std::tuple<int, double> Serializer::decodeFloat(double val) {
	val = std::abs(val);

	int exponent = std::log2(val);
	double mantissa = val / std::pow(2, exponent);

	return {exponent, mantissa};
}

Serializer* Serializer::addFloat(float val) {
	// Could be generalized to allow floats of X size, but that would be
	// precision hell.

	int exponent;
	double mantissa;

	std::tie(exponent, mantissa) = decodeFloat(val);

	addBit(val < 0);

	if (val == 0) {
		addInt(0, 8);
		addUint(0, 23);

		return this;
	}

	// TODO: handle inf?

	addInt(exponent, 8);
	addUint((mantissa - 1) * FLOAT_32_N_FACTOR, 23);  // normalize

	return this;
}

Serializer* Serializer::addDouble(double val) {
	int exponent;
	double mantissa;

	std::tie(exponent, mantissa) = decodeFloat(val);

	addBit(val < 0);

	if (val == 0) {
		addInt(0, 11);
		addUint(0, 52);

		return this;
	}

	// TODO: handle inf?

	addInt(exponent, 11);
	addUint((mantissa - 1) * FLOAT_64_N_FACTOR, 52);  // normalize

	return this;
}

Serializer* Serializer::addString(std::string str) {
	for (auto c : str) {
		addUint(c, 8);	// 1 byte per char, doesn't handle wide strings
	}

	addUint(0, 8);

	return this;
}

bool Serializer::readBit() {
	// Find the byte that contains bit number X in the byteArray
	uint8_t byte = byteArray[curBitPos >> 3];

	return (byte >> (curBitPos++ % 8)) & 1;
}

uint64_t Serializer::readUint(uint8_t bits) {
	// Probably a better way to do it, but this is what my brain thought of
	// first.

	uint64_t out = 0;

	for (uint i = 0; i < bits; ++i) {
		out += readBit() * 1ull << i;
	}

	return out;
}

int64_t Serializer::readInt(uint8_t bits) {
	bool negative = readBit();
	int64_t out = readUint(bits - 1);

	return negative ? -out - 1 : out;
}

float Serializer::readFloat() {
	int sign = readBit() ? -1 : 1;
	int exponent = readInt(8);
	double normalized = readUint(23);

	if (exponent == 0 && normalized == 0) return 0;

	double mantissa = normalized / FLOAT_32_N_FACTOR + 1;

	return sign * std::pow(2, exponent) * mantissa;
}

double Serializer::readDouble() {
	int sign = readBit() ? -1 : 1;
	int exponent = readInt(11);
	double normalized = readUint(52);

	if (exponent == 0 && normalized == 0) return 0;

	double mantissa = normalized / FLOAT_64_N_FACTOR + 1;

	return sign * std::pow(2, exponent) * mantissa;
}

std::string Serializer::readString() {
	std::string str = "";

	for (uint i = curBitPos / 8; i < byteArray.size(); i++) {
		char c = readUint(8);

		if (c == '\x00') break;

		str += c;
	}

	return str;
}