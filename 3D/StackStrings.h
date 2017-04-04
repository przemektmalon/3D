#pragma once
#include "Types.h"

class String1;

#define min(a,b) a > b ? b : a
#define max(a,b) a > b ? a : b

class StackString
{
public:
	StackString(u32 pCapacity) : capacity(pCapacity) {}
	StackString(u32 pCapacity, StackString& pStr) : capacity(pCapacity)
	{
		setToChars(pStr.getString());
	}
	StackString(u32 pCapacity, char* pStr) : capacity(pCapacity)
	{
		setToChars(pStr);
	}
	StackString(u32 pCapacity, char* begin, char* end) : capacity(pCapacity)
	{
		setToChars(begin, end);
	}

	void setToChars(const char* str)
	{
		int i = 0;
		for (; i < capacity && (str[i] != '\0'); ++i)
		{
			getString()[i] = str[i];
		}
		length = i;
		for (; i < capacity; ++i)
		{
			getString()[i] = '\0';
		}
	}

	void setToChars(const char* begin, const char* end)
	{
		int i = 0;
		for (; i < capacity && (begin != end); ++i)
		{
			getString()[i] = *begin;
			++begin;
		}
		length = i;
		for (; i < capacity; ++i)
		{
			getString()[i] = '\0';
		}
	}

	void overwrite(StackString& source)
	{
		length = min(getCapacity(), source.getLength());
		copyStr(*this, source, length);
	}

	StackString& append(StackString& str)
	{
		//copyStr(*this, str, str.getLength(), getLength());
		replaceAt(getLength(), str);
		length += str.getLength();
		length = min(length, capacity);
		return *this;
	}

	StackString& shrinkBy(u32 pShrinkBy)
	{
		length -= pShrinkBy;
		getString()[length] = '\0';
		return  *this;
	}

	StackString& append(char chr)
	{
		if (getLength() < getCapacity() - 1)
		{
			getString()[getLength()] = chr;
			length++;
		}
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

	void clear(StackString& clearMask)
	{
		u32 i = 0;
		u32 maskLen = clearMask.getLength();
		while (i < getCapacity())
		{
			getString()[i] = clearMask.getString()[i % maskLen];
			++i;
		}
	}



	s32 contains(StackString& str, u32 startSearch = 0)
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

	static s32 contains(char* begin, char* end, StackString& str, u32 startSearch = 0)
	{
		s32 searchLength = (end - begin);
		if (searchLength < 1) { return -1; }
		if (str.getLength() > searchLength)
			return -1;

		for (s32 i = startSearch; i < searchLength - str.getLength(); ++i)
		{
			if (compareChars(str.getString(), begin + i, str.getLength()) == true)
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
	StackString& removeCases(StackString& str)
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
	void insertAt(u32 i, StackString& x)
	{
		moveSegmentForward(i, x.getLength(), getLength() - i - x.getLength());
		replaceAt(i, x);
	}
	void insertWhere(StackString& whereStr, StackString& insStr)
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
	void replaceAt(u32 i, StackString& x)
	{
		s32 spaceLeftAfter = (getCapacity() - i - x.getLength());
		s32 replaceLen = spaceLeftAfter > 0 ? x.getLength() : x.getLength() - spaceLeftAfter;
		copyChars(getString() + i, x.getString(), replaceLen);
	}
	StackString& replaceWhere(StackString& whereStr, StackString& repStr)
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

	void operator+=(StackString& str)
	{
		append(str);
	}

	bool operator==(const StackString& str)
	{
		if (str.getLength() != getLength())
			return false;

		return compareChars(getString(), (const_cast<StackString&>(str)).getString(), getLength());
	}
	//void operator>
	//void operator!=

	bool operator<(StackString& str)
	{
		//return getString() < str.getString();
		auto len = min(getLength(), str.getLength());
		for (int i = 0; i < len; ++i)
		{
			if (getString()[i] != str.getString()[i])
			{
				return getString()[i] < str.getString()[i];
			}
		}
		return getLength() < str.getLength();
	}

	u32 getCapacity() const { return capacity; }
	u32 getLength() const { return length; }
	virtual char* getString() { return nullptr; };

	static void copyStr(StackString& dst, StackString& src, u32 len, u32 start = 0)
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
	static bool compareChars(const char* a, const char* b, u32 len)
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

	u32 determineLength()
	{
		length = determineLength(getString());
		return length;
	}

	static s32 strtos32(char* begin, char* end)
	{
		if (s32(end) - s32(begin) < 0)
			assert(0);

		s32 ret;
		float pow = 1;
		while (begin != end)
		{
			switch (*end)
			{
			case '0':
				break;
			case '1':
				ret += (1 * s32(std::pow(10, pow)));
				break;
			case '2':
				ret += (2 * s32(std::pow(10, pow)));
				break;
			case '3':
				ret += (3 * s32(std::pow(10, pow)));
				break;
			case '4':
				ret += (4 * s32(std::pow(10, pow)));
				break;
			case '5':
				ret += (5 * s32(std::pow(10, pow)));
				break;
			case '6':
				ret += (6 * s32(std::pow(10, pow)));
				break;
			case '7':
				ret += (7 * s32(std::pow(10, pow)));
				break;
			case '8':
				ret += (8 * s32(std::pow(10, pow)));
				break;
			case '9':
				ret += (9 * s32(std::pow(10, pow)));
				break;
			case '-':
				ret *= -1;
				break;
			}

			++pow;

			--end;
		}
		return ret;
	}

	static s32 strtos32(char* begin, s32 len)
	{
		return strtos32(begin, begin + len);
	}
	s32 strtos32()
	{
		return strtos32(getString(), getString() + getLength());
	}

protected:
	u32 capacity;
	u32 length;

};

//EXISTS TO BE PASSED AS A CLEAR MASK, USUALLY TO CLEAR TO '\0' (DEFAULT VALUE)
class String1 : public StackString
{
public:
	String1() : StackString(1)
	{
		string[0] = '\0';
		length = 1;
	}
	String1(char* str) : StackString(1) { setToChars(str); }
	String1(StackString& str) : StackString(1) { setToChars(str.getString()); }

	char* getString() { return string; }

private:

	char string[1];
};

class String8 : public StackString
{
public:
	String8() : StackString(7)
	{
		clear(String1());
		length = 0;
	}
	String8(char* str) : StackString(7) { setToChars(str); }
	String8(StackString& str) : StackString(7) { setToChars(str.getString()); }

	char* getString() { return string; }

private:

	char string[7];
	const char nullEnd = '\0';
};

class String16 : public StackString
{
public:
	String16() : StackString(15)
	{
		clear(String1());
		length = 0;
	}
	String16(char* str) : StackString(15) { setToChars(str); }
	String16(StackString& str) : StackString(15) { setToChars(str.getString()); }

	char* getString() { return string; }

private:

	char string[15];
	const char nullEnd = '\0';
};

class String32 : public StackString
{
public:
	String32() : StackString(31)
	{
		clear(String1());
		length = 0;
	}
	String32(char* str) : StackString(31) { setToChars(str); }
	String32(StackString& str) : StackString(31) { setToChars(str.getString()); }

	char* getString() { return string; }

private:

	char string[31];
	const char nullEnd = '\0';
};

class String64 : public StackString
{
public:
	String64() : StackString(63)
	{
		clear(String1());
		length = 0;
	}
	String64(char* str) : StackString(63) { setToChars(str); }
	String64(StackString& str) : StackString(63) { setToChars(str.getString()); }

	char* getString() { return string; }

private:

	char string[63];
	const char nullEnd = '\0';
};

class String128 : public StackString
{
public:
	String128() : StackString(127)
	{
		clear(String1());
		length = 0;
	}
	String128(char* str) : StackString(127) { setToChars(str); }
	String128(StackString& str) : StackString(127) { setToChars(str.getString()); }

	char* getString() { return string; }

private:

	char string[127];
	const char nullEnd = '\0';
};

class String256 : public StackString
{
public:
	String256() : StackString(255)
	{
		clear(String1());
		length = 0;
	}
	String256(char* str) : StackString(255) { setToChars(str); }
	String256(StackString& str) : StackString(255) { setToChars(str.getString()); }

	char* getString() { return string; }

private:

	char string[255];
	const char nullEnd = '\0';
};

class String512 : public StackString
{
public:
	String512() : StackString(511)
	{
		clear(String1());
		length = 0;
	}
	String512(char* str) : StackString(511) { setToChars(str); }
	String512(StackString& str) : StackString(511) { setToChars(str.getString()); }

	char* getString() { return string; }

private:

	char string[63];
	const char nullEnd = '\0';
};

class String1024 : public StackString
{
public:
	String1024() : StackString(1023)
	{
		clear(String1());
		length = 0;
	}
	String1024(char* str) : StackString(1023) { setToChars(str); }
	String1024(StackString& str) : StackString(1023) { setToChars(str.getString()); }

	char* getString() { return string; }

private:

	char string[1023];
	const char nullEnd = '\0';
};

class String16_1024 : public StackString
{
public:
	String16_1024() : StackString((16 * 1024) - 1)
	{
		clear(String1());
		length = 0;
	}
	String16_1024(char* str) : StackString((16 * 1024) - 1) { setToChars(str); }
	String16_1024(StackString& str) : StackString((16 * 1024) - 1) { setToChars(str.getString()); }

	char* getString() { return string; }

private:

	char string[(16 * 1024) - 1];
	const char nullEnd = '\0';
};

#define DEFINE_STRING_STD_LESS(strSize) \
template<> struct std::less<String##strSize##> { \
	bool operator() (const String##strSize##& lhs, const String##strSize##& rhs) const { \
		return (const_cast<String##strSize##&>(lhs) < const_cast<String##strSize##&>(rhs));}};

DEFINE_STRING_STD_LESS(128)
DEFINE_STRING_STD_LESS(64)
DEFINE_STRING_STD_LESS(32)
DEFINE_STRING_STD_LESS(16)

#undef min
#undef max