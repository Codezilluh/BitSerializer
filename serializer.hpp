#include <cmath>
#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

class Serializer {
  private:
	typedef unsigned int uint;
	typedef unsigned long long ulong;

	const static uint FLOAT_32_N_FACTOR = 8388608;			  // 2^23
	const static ulong FLOAT_64_N_FACTOR = 4503599627370496;  // 2^52

	std::tuple<int, double> decodeFloat(double);

	uint curBitPos;

  public:
	typedef std::vector<uint8_t> ByteArray;

	ByteArray byteArray;

	Serializer();

	Serializer(ByteArray);

	/**
	 * Adds a bit (bool) to the byteArray.
	 */
	Serializer* addBit(bool);

	/**
	 * Adds an unsigned integer of a specified bit size to the byteArray.
	 *
	 * Example:
	 * addUint(123, 28); // adds 28 bits to the byteArray with a value of 123
	 */
	Serializer* addUint(uint64_t, uint8_t);

	/**
	 * Adds a signed integer of a specified bit size to the byteArray.
	 *
	 * Used in the same way as addUint.
	 */
	Serializer* addInt(int64_t, uint8_t);

	/**
	 * Adds a (32-bit) float to the byteArray.
	 */
	Serializer* addFloat(float);

	/**
	 * Adds a (64-bit) double to the byteArray.
	 */
	Serializer* addDouble(double);

	/**
	 * Adds a std::string to the byteArray using null-termination. It is very
	 * important that the string does not contain '\x00'.
	 */
	Serializer* addString(std::string);

	/**
	 * Reads the next bit from the byteArray.
	 */
	bool readBit();

	/**
	 * Reads the next X bits from the byteArray as a uint.
	 */
	uint64_t readUint(uint8_t);

	/**
	 * Reads the next X bits from the byteArray as an int.
	 */
	int64_t readInt(uint8_t);

	/**
	 * Reads the next 32 bits from the byteArray as a float.
	 */
	float readFloat();

	/**
	 * Reads the next 64 bits from the byteArray as a double.
	 */
	double readDouble();

	/**
	 * Interprets the next several bits as a string.
	 */
	std::string readString();
};
