#pragma once
#include <map>
#include <assert.h>
#include <cmath>
#include "Types.h"

class StringGeneric
{
public:
	StringGeneric(u32 pCapacity) : capacity(pCapacity) {}
	StringGeneric(u32 pCapacity, StringGeneric& pStr) : capacity(pCapacity)
	{
		setToChars(pStr.getString());
	}
	StringGeneric(u32 pCapacity, char* pStr) : capacity(pCapacity)
	{
		setToChars(pStr);
	}
	StringGeneric(u32 pCapacity, char* begin, char* end) : capacity(pCapacity)
	{
		setToChars(begin, end);
	}

	void setToChars(const char* str);

	void setToChars(const char* begin, const char* end);

	virtual void overwrite(StringGeneric& source);

	StringGeneric& append(StringGeneric&& str);
	StringGeneric& append(char* chars, u32 len);
	StringGeneric& append(char* chars);
	virtual StringGeneric& append(StringGeneric& str);
	virtual StringGeneric& append(char chr);

	StringGeneric& shrinkBy(u32 pShrinkBy);

	virtual void replicate(char chr, u32 len, u32 start = 0);

	void clear(StringGeneric&& clearMask);
	void clear(StringGeneric& clearMask);
	void clear(char chr);

	s32 contains(StringGeneric&& str, u32 startSearch = 0);
	s32 contains(StringGeneric& str, u32 startSearch = 0);

	static s32 contains(char* begin, char* end, StringGeneric&& str, u32 startSearch = 0);
	static s32 contains(char* begin, char* end, StringGeneric& str, u32 startSearch = 0);

	virtual void moveSegmentForward(s32 takeFrom, s32 forward, u32 len, char fill = ' ');

	void moveSegmentBackward(s32 takeFrom, s32 backward, u32 len, char fill = ' ');

	void moveWithin(s32 takeFrom, s32 moveTo, u32 len, char fill = ' ');

	//void map(FUNCTION (CHAR -> CHAR))
	//void mapIf(FUNCTION (CHAR -> CHAR), FUNCTION (CHAR -> BOOL));
	//void mapCase(FUNCTION (CHAR -> CHAR), String& x);
	//void mapCaseIf(FUNCTION (CHAR -> CHAR), FUNCTION (CHAR -> BOOL), String& x);
	StringGeneric& removeCases(StringGeneric& str);
	//void applyWhere(FUNCTION (CHAR -> CHAR), String& x)
	//StringXX findCases(String& x) { RETURN LIST OF INDICES IN STRING ENC AS BINARY }
	StringGeneric& insertAt(u32 i, StringGeneric&& x);
	virtual StringGeneric& insertAt(u32 i, StringGeneric& x);
	StringGeneric& insertWhere(StringGeneric&& whereStr, StringGeneric&& insStr);
	StringGeneric& insertWhere(StringGeneric& whereStr, StringGeneric&& insStr);
	StringGeneric& insertWhere(StringGeneric&& whereStr, StringGeneric& insStr);
	virtual StringGeneric& insertWhere(StringGeneric& whereStr, StringGeneric& insStr);

	StringGeneric& replaceAt(u32 i, StringGeneric&& x);
	virtual StringGeneric& replaceAt(u32 i, StringGeneric& x);
	virtual StringGeneric& replaceAt(u32 i, char* x, u32 len);

	StringGeneric& replaceWhere(StringGeneric&& whereStr, StringGeneric&& repStr);
	StringGeneric& replaceWhere(StringGeneric&& whereStr, StringGeneric& repStr);
	StringGeneric& replaceWhere(StringGeneric& whereStr, StringGeneric&& repStr);
	virtual StringGeneric& replaceWhere(StringGeneric& whereStr, StringGeneric& repStr);

	StringGeneric& removeAt(u32 i, u32 num);

	s32 find(char c);

	void take(u32 num, StringGeneric& dst);
	void drop(u32 num);

	void operator+=(StringGeneric& str);

	bool operator==(const StringGeneric& str);

	//void operator>
	//void operator!=

	bool operator<(const StringGeneric& str) const;
	//bool operator<(const StringGeneric& str);

	void operator=(StringGeneric& str)
	{
		overwrite(str);
	}

	void operator=(const StringGeneric& str)
	{
		overwrite(const_cast<StringGeneric&>(str));
	}

	void operator=(const std::string& str)
	{
		setToChars(str.c_str());
	}

	u32 getCapacity() const { return capacity; }
	u32 getLength() const { return length; }
	virtual char* getString() { return nullptr; };

	static void copyStr(StringGeneric& dst, StringGeneric& src, u32 len, u32 start = 0);

	//UNSAFE
	static void copyChars(char* dst, char* src, u32 len);

	//UNSAFE
	static bool compareChars(const char* a, const char* b, u32 len);

	static char* cReplicate(char chr, u32 len, u32 start = 0);

	//UNSAFE
	static u32 determineLength(const char* str);

	u32 determineLength();

	static s32 strtos32(char* begin, char* end);

	static s32 strtos32(char* begin, s32 len);
	s32 strtos32();

protected:
	u32 capacity;
	u32 length;
};

class HeapGeneric : public StringGeneric
{
public:
	HeapGeneric() : StringGeneric(0) {}
	HeapGeneric(u32 pCapacity) : StringGeneric(pCapacity) {}
	HeapGeneric(u32 pCapacity, HeapGeneric& pStr) : StringGeneric(pCapacity)
	{
		setToChars(pStr.getString());
	}
	HeapGeneric(u32 pCapacity, char* pStr) : StringGeneric(pCapacity)
	{
		setToChars(pStr);
	}
	HeapGeneric(u32 pCapacity, char* begin, char* end) : StringGeneric(pCapacity)
	{
		setToChars(begin, end);
	}

	virtual void expand(u32 pNewSize) {}

	void autoExpand()
	{
		expand(length + (length / 2));
	}

	void overwrite(StringGeneric& source);
	void replicate(char chr, u32 len, u32 start = 0);
	StringGeneric& append(StringGeneric& str);
	StringGeneric& append(char chr);
	StringGeneric& append(char* chars, u32 len);

	void moveSegmentForward(s32 takeFrom, s32 forward, u32 len, char fill = ' ');

	StringGeneric& insertAt(u32 i, StringGeneric& x);

	StringGeneric& insertWhere(StringGeneric& whereStr, StringGeneric& insStr);

	StringGeneric& replaceAt(u32 i, StringGeneric& x);
	StringGeneric& replaceAt(u32 i, char* chars, u32 len);

	virtual char* getString() { return nullptr; }

};