#pragma once
//lang::Cpp

// #include <string.h>
#include "../utils/string.h"

// represents a class that serializes primitives and Strings, adding them to an internal buffer
class Serialize {
public:
	size_t size = 1024;
	char* buffer = new char[size];
	size_t offset = 0;

	// constructor
	Serialize() {}

	// destructor
	~Serialize() {}

	// serializes the given size_t and writes it to the buffer
	void write(size_t arg) {
		size_t growVal = sizeof(size_t);
		growBufferIfNeeded(growVal);
		memcpy(buffer+offset, &arg, growVal);
		offset += growVal;
	}

	// serializes the given int and writes it to the buffer
	void write(int i) {
		size_t growVal = sizeof(int);
		growBufferIfNeeded(growVal);
		memcpy(buffer+offset, &i, growVal);
		offset += growVal;
	}

	// serializes the given double and writes it to the buffer
	void write(double f) {
		size_t growVal = sizeof(double);
		growBufferIfNeeded(growVal);
		memcpy(buffer+offset, &f, growVal);
		offset += growVal;
	}

	// serializes the given bool and writes it to the buffer
	void write(bool b) {
		size_t growVal = sizeof(bool);
		growBufferIfNeeded(growVal);
		memcpy(buffer+offset, &b, growVal);
		offset += growVal;
	}

	// serializes the given String and writes it to the buffer
	void write(String* s) {
		std::cout << "serializing a string in write(String* s): " << s->cstr_ << "\n";
		std::cout << "serializing the string's size in write(String* s): " << s->size_ << "\n";
		size_t growVal = sizeof(char);
		write(s->size_);
		growBufferIfNeeded(growVal * s->size_);
		memcpy(buffer+offset, s->c_str(), s->size_ * growVal); // make variable here of size
		offset += s->size_ * growVal;
	}

	// serializes the given char and writes it to the buffer
	void write(char c) {
		size_t growVal = sizeof(char);
		growBufferIfNeeded(growVal);
		memcpy(buffer+offset, &c, growVal);
		offset += growVal;
	}

	void write(char* chars) {
		size_t growVal = strlen(chars) * sizeof(char);
		growBufferIfNeeded(growVal);
		memcpy(buffer+offset, chars, growVal);
		offset += growVal;
	}

	// grows the buffer if an element we want to add will overflow the buffer
	// @arg: elementSize: the size of the element we want to add
	void growBufferIfNeeded(size_t elementSize) {
		if (offset + elementSize > size) {
			size_t newSize = size * 2;
			char* newBuffer = new char[newSize];
			memcpy(newBuffer, buffer, size);
			delete[] buffer;
			buffer = newBuffer;
			size = newSize;
		}
	}

	// returns the buffer
	char* getChars() {
		return buffer;
	}

	size_t get_size() {
		return size;
	}
};

class Deserialize {
public:
	char* buffer;
	size_t offset = 0;

	// constructor
	Deserialize(char* givenBuff) {
		buffer = givenBuff;
	}

	// destructor
	~Deserialize() {
		delete[] buffer;
	}

	// reads a size_t from the buffer
	size_t readSizeT() {
		size_t cpySize = sizeof(size_t);
		size_t result = 0;
		memcpy(&result, buffer+offset, cpySize);
		offset += cpySize;
		return result;
	}

	// reads a char array from the buffer
	char* readChars(size_t size) {
		char* result = buffer+offset;
		offset += size * sizeof(char);
		return result;
	}

	// reads a char from the buffer
	char readChar() {
		char* result = buffer+offset;
		// result[offset] = 0;
		offset += sizeof(char);
		return *result;
	}

	// reads a String from the buffer
	String* readString() {
		size_t size = readSizeT();
		std::cout << "readString()'s size_T size: " << size << "\n";

		char* cstr = readChars(size);
		std::cout << "readString()'s char* cstr: " << cstr << "\n";

		String* result = new String(cstr, size);
		return result;
	}

	// reads an int from the buffer
	int readInt() {
		size_t cpySize = sizeof(int);
		int result = 0;
		memcpy(&result, buffer+offset, cpySize);
		offset += cpySize;
		return result;
	}

	// reads a double from the buffer
	double readDouble() {
		size_t cpySize = sizeof(double);
		double result = 0.0;
		memcpy(&result, buffer+offset, cpySize);
		offset += cpySize;
		return result;
	}

	// reads a bool from the buffer
	bool readBool() {
		size_t cpySize = sizeof(bool);
		bool result;
		memcpy(&result, buffer+offset, cpySize);
		offset += cpySize;
		return result;
	}
};