#pragma once

//--------------------------------------------------------------------//
// "Malware related compile-time hacks with C++11" by <h1>LeFF</h1>   //
// You can use this code however you like, I just don't really		  //
// give a shit, but if you feel some respect for me, please			  //
// don't cut off this comment when copy-pasting.... ;-)				  //
//--------------------------------------------------------------------//

template <int X> struct EnsureCompileTime {
	enum : int {
		Value = X
	};
};




#define Seed	 ((__TIME__[7] - '0') * 1  +   (__TIME__[6] - '0') * 10  + 420 + \
                  (__TIME__[4] - '0') * 60   + (__TIME__[3] - '0') * 600 + 1337 + \
                  (__TIME__[1] - '0') * 3600 + (__TIME__[0] - '0') * 36000 + 69)




constexpr int LinearCongruentGenerator(int Rounds) {
	//return 1013904223ull + 1664525ull * ( (Rounds > 0) ? LinearCongruentGenerator(Rounds - 1ull) : Seed & 0xFFFFFFFF);
	if (Rounds > 0)
	{
		return 1013904223ull + 1664525ull * LinearCongruentGenerator(Rounds - 1ull);
	}
	else
	{
		return 1013904223ull + 1664525ull * Seed & 0xFFFFFFFF; //32bits 
	}
}
#define Random() EnsureCompileTime<LinearCongruentGenerator(11)>::Value //11 Rounds
#define RandomNumber(Min, Max) (Min + (Random() % (Max - Min + 1)))

template <int... Pack> struct IndexList {};

template <typename IndexList, int Right> struct Append;
template <int... Left, int Right> struct Append<IndexList<Left...>, Right> {
	typedef IndexList<Left..., Right> Result;
};

template <int N> struct ConstructIndexList {
	typedef typename Append<typename ConstructIndexList<N - 1>::Result, N - 1>::Result Result;
};
template <> struct ConstructIndexList<0> {
	typedef IndexList<> Result;
};

//const char XORKEY = (char)0x859;
const char cleMagique = (char)(RandomNumber(0, 0xFF));

constexpr char EncryptCharacter(const char Character, int Index) {
	return Character ^ (cleMagique + Index);
}


template <typename IndexList> class CXorString;
template <int... Index> class CXorString<IndexList<Index...> >
{
private:
	char Value[sizeof...(Index) + 1];
public:
	constexpr __forceinline CXorString(const char* const String)
		: Value{ EncryptCharacter(String[Index], Index)... } {}

	char* decrypt() {
		for (int t = 0; t < sizeof...(Index); t++) {
			Value[t] = Value[t] ^ (cleMagique + t);
		}
		Value[sizeof...(Index)] = '\0';
		return Value;
	}

	char* get() {
		return Value;
	}
};

#define XorString(String) CXorString<ConstructIndexList<sizeof(String)-1>::Result>(String).decrypt()