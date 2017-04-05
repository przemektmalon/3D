#pragma once
#include "StringGenerics.h"

#define FORCE_LAST_CHAR_NULL 1
#define INCLUDE_NULL_IN_LENGTH 1
#define HEAP 0

template<int size>
class String : public StringGeneric
{
public:
#if FORCE_LAST_CHAR_NULL

#if INCLUDE_NULL_IN_LENGTH
	String() : StringGeneric(size - 1)
	{
		clear('\0');
		length = 0;
	}
	String(char* str) : StringGeneric(size - 1) { setToChars(str); }
	String(const char* str) : StringGeneric(size - 1) { setToChars(str); }
	String(StringGeneric& str) : StringGeneric(size - 1) { overwrite(str); }
	String(const char chr) : StringGeneric(size - 1) { string[0] = chr; }
#else //INCLUDE_NULL_IN_LENGTH
	String() : StringGeneric(size)
	{
		clear('\0');
		length = 0;
	}
	String(const char* str) : StringGeneric(size) { setToChars(str); }
	String(StringGeneric& str) : StringGeneric(size) { overwrite(str); }
	String(const char chr) : StringGeneric(size) { string[0] = chr; }
#endif //INCLUDE_NULL_IN_LENGTH

	char* getString() { return string; }

private:

#if INCLUDE_NULL_IN_LENGTH
	char string[size - 1];
#else //INCLUDE_NULL_IN_LENGTH
	char string[size];
#endif //INCLUDE_NULL_IN_LENGTH


	const char lastNull = '\0';


#else //FORCE_LAST_CHAR_NULL
	String() : StringGeneric(size)
	{
		clear('\0');
		length = 0;
	}
	String(const char* str) : StringGeneric(size) { setToChars(str); }
	String(StringGeneric& str) : StringGeneric(size) { overwrite(str); }
	String(const char chr) : StringGeneric(size) { string[0] = chr; }

	char* getString() { return string; }
private:

	char string[size];
#endif //FORCE_LAST_CHAR_NULL
};

template<>
class String<1> : public StringGeneric
{
public:
	String() : StringGeneric(1)
	{
		string[0] = '\0';
		length = 1;
	}
	String(const char* str) : StringGeneric(1) { string[0] = str[0]; length = 1; }
	String(StringGeneric& str) : StringGeneric(1) { string[0] = str.getString()[0]; length = 1; }
	String(const char chr) : StringGeneric(1) { string[0] = chr; length = 1; }

	char* getString() { return string; }

private:

	char string[1];
};



template<>
class String<HEAP> : public HeapGeneric
{
public:
	String() : HeapGeneric(0)
	{
		string = nullptr;
	}
	String(const char* str) : HeapGeneric(0) 
	{ 
		length = StringGeneric::determineLength(str);
		capacity = length * 1.5; ///TODO: better value ?
		string = new char[capacity];
		setToChars(str); 
	}
	String(const char* start, const char* end) : HeapGeneric(0)
	{
		length = end - start;
		capacity = length * 1.5; ///TODO: better value ?
		string = new char[capacity];
		setToChars(start,end);
	}
	String(StringGeneric& str) : HeapGeneric(0) 
	{
		length = str.getLength();
		capacity = length * 1.5;
		string = new char[capacity];
		setToChars(str.getString()); 
	}
	~String() { length = 0; capacity = 0; if (string != nullptr) { delete[] string; } }

	void expand(u32 pNewSize)
	{
		if (getString() == nullptr)
		{
			string = new char[pNewSize + 1];
			capacity = pNewSize;
			replicate('\0', capacity + 1, 0);
			return;
		}
		char* oldString = string;
		string = new char[pNewSize + 1];
		copyChars(string, oldString, length);
		capacity = pNewSize;
		replicate('\0', capacity - length, length);
		//HeapGeneric nullEnd(capacity - length);
		//nullEnd.replicate('\0', capacity - length);
		//replaceAt(length, nullEnd);
		delete[] oldString;
	}

	char* getString() { return string; }

private:

	char* string;
};

int constexpr stringLiteralLength(const char* str)
{
	return *str ? 1 + stringLiteralLength(str + 1) : 1;
}

#define SSTR(chars) String<stringLiteralLength(chars)>(chars)

#define DEF_STR(size) typedef String<##size##> String##size##;

typedef String<HEAP> HeapString;

//Defines for example String32 <-> String<32>

DEF_STR(8);
DEF_STR(16);
DEF_STR(32);
DEF_STR(64);
DEF_STR(128);
DEF_STR(256);
DEF_STR(512);
DEF_STR(1024);