#pragma once
#include "Types.h"

class String1;

#define min(a,b) a > b ? b : a
#define max(a,b) a > b ? a : b

class String
{
public:
	String(u32 pCapacity) : capacity(pCapacity) {}

	void setToChars(char* str)
	{
		int i = 0;
		for (; i < capacity && (str[i] != '\0'); ++i)
		{
			getString()[i] = str[i];
		}
		length = i;
		for (int j = i; j < capacity; ++j)
		{
			getString()[j] = '\0';
		}
	}

	void overwrite(String& source)
	{
		if (source.getCapacity() < getCapacity())
		{
			copyStr(*this, source, source.getCapacity());
			length = min(getCapacity(), source.getLength());
		}
		else
		{
			copyStr(*this, source, getCapacity());
			length = min(getCapacity(), source.getLength());
		}
	}

	//TODO: RETURN REFERENCE TO SELF, CHECK IF VIRTUAL SPECIALISATIONS
	String& append(String& str)
	{
		//copyStr(*this, str, str.getLength(), getLength());
		replaceAt(getLength(), str);
		length += str.getLength();
		length = min(length, capacity);
		return *this;
	}

	void replicate(char chr, u32 len, u32 start = 0)
	{
		if (start + len > getCapacity())
		{
			len = getCapacity() - start;
		}
		auto me = getString();
		for (int i = start; i < start + len; ++i)
		{
			me[i] = chr;
		}
	}

	void clear(String& clearMask)
	{
		u32 i = 0;
		u32 maskLen = clearMask.getLength();
		while (i < getCapacity())
		{
			getString()[i] = clearMask.getString()[i % maskLen];
			++i;
		}
	}

	void operator+=(String& str)
	{
		append(str);
	}

	void operator+(String& str)
	{
		append(str);
	}

	bool operator==(String& str)
	{
		if (str.getLength() != getLength())
			return false;

		return compareChars(getString(), str.getString(), getLength());
	}
	//void operator>
	//void operator<
	//void operator!=

	s32 contains(String& str, u32 startSearch = 0)
	{
		if (str.getLength() > getLength())
			return -1;

		for (s32 i = startSearch; i < getLength() - str.getLength(); ++i)
		{
			if (compareChars(str.getString(), getString() + i, str.getLength()) == true)
			{
				return i;
			}
		}
		return s32(-1);
	}

	void moveSegmentForward(s32 takeFrom, s32 forward, u32 len, char fill = '_')
	{
		if (takeFrom + len > getCapacity())
			len = getCapacity() - takeFrom;
		if (takeFrom + forward + len > getCapacity())
			len = getCapacity() - (takeFrom + forward);

		for (s32 i = takeFrom + forward + len - 1; i > takeFrom + forward; --i)
		{
			getString()[i] = getString()[i - forward];
		}
	}

	void moveSegmentBackward(s32 takeFrom, s32 backward, u32 len, char fill = '_')
	{
		if (takeFrom + len > getCapacity())
			len = getCapacity() - takeFrom;
		if (takeFrom - backward + len > getCapacity())
			len = getCapacity() - (takeFrom - backward);

		for (s32 i = takeFrom - backward; i < takeFrom - backward + len; ++i)
		{
			getString()[i] = getString()[i + backward];
		}

		for (s32 i = takeFrom - backward + len; i < takeFrom + len; ++i)
		{
			getString()[i] = fill;
		}
	}

	void moveWithin(s32 takeFrom, s32 moveTo, u32 len, char fill = '_')
	{
		if (s32(moveTo) - s32(takeFrom) > 0)
			moveSegmentForward(takeFrom, moveTo - takeFrom, len, fill);
		else
			moveSegmentBackward(takeFrom, takeFrom - moveTo, len, fill);
	}

	//void map(FUNCTION (CHAR -> CHAR))
	//void mapIf(FUNCTION (CHAR -> CHAR), FUNCTION (CHAR -> BOOL));
	//void mapCase(FUNCTION (CHAR -> CHAR), String& x);
	//void mapCaseIf(FUNCTION (CHAR -> CHAR), FUNCTION (CHAR -> BOOL), String& x);
	String& removeCases(String& str)
	{
		s32 i = -2;
		s32 j = 0;
		while (i != -1)
		{
			i = contains(str, j);

			if (i > -1)
			{
				moveSegmentBackward(i + str.getLength(), str.getLength(), getLength() - (i + str.getLength()), '\0');
				j = i;
				i = -2;
			}
		}
		return *this;
	}
	//void applyWhere(FUNCTION (CHAR -> CHAR), String& x)
	//StringXX findCases(String& x) { RETURN LIST OF INDICES IN STRING ENC AS BINARY }
	void insertAt(u32 i, String& x)
	{
		moveSegmentForward(i, x.getLength(), getLength() - i - x.getLength());
		replaceAt(i, x);
	}
	void insertWhere(String& whereStr, String& insStr)
	{
		s32 i = -2;
		s32 j = 0;
		while (i != -1)
		{
			i = contains(whereStr, j);

			if (i > -1)
			{
				moveWithin(i + whereStr.getLength(), i + insStr.getLength(), getLength() - (i + whereStr.getLength()));
				replaceAt(i, insStr);
				j = i;
				i = -2;
			}
		}
	}
	void replaceAt(u32 i, String& x)
	{
		u32 spaceLeftAfter = (getCapacity() - i - x.getLength());
		u32 replaceLen = spaceLeftAfter > 0 ? x.getLength() : x.getLength() - spaceLeftAfter;
		copyChars(getString() + i, x.getString(), replaceLen);
	}
	String& replaceWhere(String& whereStr, String& repStr)
	{
		s32 i = -2;
		s32 j = 0;
		while (i != -1)
		{
			i = contains(whereStr, j);

			if (i > -1)
			{
				moveWithin(i + whereStr.getLength(), i + repStr.getLength(), getLength() - (i + whereStr.getLength()));
				replaceAt(i, repStr);
				j = i;
				i = -2;
			}
		}
		return *this;
	}

	u32 getCapacity() { return capacity; }
	u32 getLength() { return length; }
	virtual char* getString() { return nullptr; };

	static void copyStr(String& dst, String& src, u32 len, u32 start = 0)
	{


		auto copyLen = len;
		if (copyLen > src.getCapacity())
			copyLen = src.getCapacity();
		if (copyLen > dst.getCapacity())
			copyLen = dst.getCapacity();

		auto dstP = dst.getString();
		auto srcP = src.getString();

		for (int i = 0; i < copyLen; ++i)
		{
			dstP[i + start] = srcP[i];
		}
	}

	//UNSAFE
	static void copyChars(char* dst, char* src, u32 len)
	{
		u32 i = 0;
		while (i < len)
		{
			dst[i] = src[i];
			++i;
		}
	}

	//UNSAFE
	static bool compareChars(char* a, char* b, u32 len)
	{
		bool ret = true;
		u32 i = 0;
		while (i < len && ret == true)
		{
			ret = (a[i] == b[i]);
			++i;
		}
		return ret;
	}

	static char* cReplicate(char chr, u32 len, u32 start = 0)
	{
		char* retChr = new char[len + start];
		for (int i = 0; i < start; ++i)
		{
			retChr[i] = ' ';
		}
		for (int i = start; i < start + len; ++i)
		{
			retChr[i] = chr;
		}
		return retChr;
	}

	//UNSAFE
	static u32 determineLength(char* str)
	{
		u32 ret = 0;
		for (;;)
		{
			if (str[ret] == '\0')
			{
				return ret;
			}
			++ret;
		}
	}


protected:
	u32 capacity;
	u32 length;

};

class String1 : public String
{
public:
	String1() : String(1)
	{
		string[0] = '\0';
		length = 1;
	}
	String1(char* str) : String(1)
	{
		setToChars(str);
	}

	char* getString() { return string; }

private:

	char string[1];
};

class String32 : public String
{
public:
	String32() : String(31)
	{
		clear(String1());
		length = 0;
	}
	String32(char* str) : String(31)
	{
		setToChars(str);
	}

	char* getString() { return string; }

private:

	char string[31];
	const char nullEnd = '\0';
};

class String64 : public String
{
public:
	String64() : String(63)
	{
		clear(String1());
		length = 0;
	}
	String64(char* str) : String(63)
	{
		setToChars(str);
	}

	char* getString() { return string; }

private:

	char string[63];
	const char nullEnd = '\0';
};

#undef min
#undef max