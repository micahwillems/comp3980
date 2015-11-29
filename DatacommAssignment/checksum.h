#pragma once
#include <windows.h>
#include <vector>
#include <string>

class Checksum {
public:
	Checksum() { clear(); }
	void clear() { sum = 0; r = 55665; c1 = 52845; c2 = 22719; }
	void add(char b);
	std::vector<char>get();
	boolean check(char a, char b); 
protected:
	WORD r;
	WORD c1;
	WORD c2;
	WORD sum;
};

inline
void Checksum::add(char value) {
	char cipher = (value ^ (r >> 8));
	r = (cipher + r) * c1 + c2;
	sum += cipher;
}

inline
std::vector<char>Checksum::get() {
	std::vector<char> out;
	out.push_back((char)(sum / 256));
	out.push_back((char)(sum % 256));
	return out;
}

inline
boolean Checksum::check(char a, char b) {
	WORD check;
	check = (WORD)a * 256;
	check += (byte)b;
	return check == sum;
}