#pragma once
#include "Strings.h"

#define min(a,b) a > b ? b : a
#define max(a,b) a > b ? a : b

void StringGeneric::setToChars(const char * str)
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

void StringGeneric::setToChars(const char * begin, const char * end)
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

void StringGeneric::overwrite(StringGeneric & source)
{
	length = min(getCapacity(), source.getLength());
	copyStr(*this, source, length);
	getString()[length] = '\0';
}


StringGeneric & StringGeneric::append(StringGeneric & str)
{
	//copyStr(*this, str, str.getLength(), getLength());
	replaceAt(getLength(), str);
	length += str.getLength();
	length = min(length, capacity);
	return *this;
}

StringGeneric & StringGeneric::append(StringGeneric && str)
{
	return append(str);
}

StringGeneric & StringGeneric::append(char * chars, u32 len)
{
	auto& ret = replaceAt(getLength(), chars, len);
	length += len;
	return ret;
}

StringGeneric & StringGeneric::append(char * chars)
{
	return append(chars, StringGeneric::determineLength(chars));
}

StringGeneric & StringGeneric::shrinkBy(u32 pShrinkBy)
{
	length -= pShrinkBy;
	getString()[length] = '\0';
	return  *this;
}

StringGeneric & StringGeneric::append(char chr)
{
	if (getLength() < getCapacity() - 1)
	{
		getString()[getLength()] = chr;
		length++;
	}
	return *this;
}

void StringGeneric::replicate(char chr, u32 len, u32 start)
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

void StringGeneric::clear(StringGeneric & clearMask)
{
	u32 i = 0;
	u32 maskLen = clearMask.getLength();
	while (i < getCapacity())
	{
		getString()[i] = clearMask.getString()[i % maskLen];
		++i;
	}
}

void StringGeneric::clear(char chr)
{
	u32 i = 0;
	while (i < getCapacity())
	{
		getString()[i] = chr;
		++i;
	}
	length = 0;
}

void StringGeneric::clear(StringGeneric && clearMask)
{
	clear(clearMask);
}

s32 StringGeneric::contains(StringGeneric & str, u32 startSearch)
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

s32 StringGeneric::contains(StringGeneric && str, u32 startSearch)
{
	return contains(str, startSearch);
}

s32 StringGeneric::contains(char * begin, char * end, StringGeneric & str, u32 startSearch)
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

s32 StringGeneric::contains(char * begin, char * end, StringGeneric && str, u32 startSearch)
{
	return StringGeneric::contains(begin, end, str, startSearch);
}

void StringGeneric::moveSegmentForward(s32 takeFrom, s32 forward, u32 len, char fill)
{
	if (takeFrom + len > getCapacity())
		len = getCapacity() - takeFrom;
	if (takeFrom + forward + len > getCapacity())
		len = getCapacity() - (takeFrom + forward);

	for (s32 i = takeFrom + forward + len - 1; i >= takeFrom + forward; --i)
	{
		getString()[i] = getString()[i - forward];
	}
	length += forward;
	length = min(length, capacity);
}

void StringGeneric::moveSegmentBackward(s32 takeFrom, s32 backward, u32 len, char fill)
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

void StringGeneric::moveWithin(s32 takeFrom, s32 moveTo, u32 len, char fill)
{
	if (s32(moveTo) - s32(takeFrom) > 0)
		moveSegmentForward(takeFrom, moveTo - takeFrom, len, fill);
	else
		moveSegmentBackward(takeFrom, takeFrom - moveTo, len, fill);
}

StringGeneric & StringGeneric::removeCases(StringGeneric & str)
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

StringGeneric& StringGeneric::insertAt(u32 i, StringGeneric & x)
{
	moveSegmentForward(i, x.getLength(), getLength());
	replaceAt(i, x);
	return *this;
}

StringGeneric& StringGeneric::insertAt(u32 i, StringGeneric && x)
{
	return insertAt(i, x);
}

StringGeneric& StringGeneric::insertWhere(StringGeneric & whereStr, StringGeneric & insStr)
{
	s32 i = -2;
	s32 j = 0;
	while (i != -1)
	{
		i = contains(whereStr, j);

		if (i > -1)
		{
			insertAt(i + 1, insStr);
			j = i + insStr.getLength();
			i = -2;
		}
	}
	return *this;
}

StringGeneric& StringGeneric::insertWhere(StringGeneric & whereStr, StringGeneric && insStr)
{
	return insertWhere(whereStr, insStr);
}

StringGeneric& StringGeneric::insertWhere(StringGeneric && whereStr, StringGeneric & insStr)
{
	return insertWhere(whereStr, insStr);
}

StringGeneric& StringGeneric::insertWhere(StringGeneric && whereStr, StringGeneric && insStr)
{
	return insertWhere(whereStr, insStr);
}

StringGeneric& StringGeneric::replaceAt(u32 i, StringGeneric & x)
{
	s32 spaceLeftAfter = (getCapacity() - i - x.getLength());
	s32 replaceLen = spaceLeftAfter > 0 ? x.getLength() : x.getLength() - spaceLeftAfter;
	//moveSegmentForward(i + 1, x.getLength(), getLength() - (i + 1));
	copyChars(getString() + i, x.getString(), replaceLen);
	//length += x.getLength();
	//length = min(length, capacity);
	return *this;
}

StringGeneric & StringGeneric::replaceAt(u32 i, char * x, u32 len)
{
	s32 spaceLeftAfter = (getCapacity() - i - len);
	s32 replaceLen = spaceLeftAfter > 0 ? len : len - spaceLeftAfter;
	copyChars(getString() + i, x, replaceLen);
	return *this;
}

StringGeneric& StringGeneric::replaceAt(u32 i, StringGeneric && x)
{
	return replaceAt(i, x);
}

StringGeneric & StringGeneric::replaceWhere(StringGeneric & whereStr, StringGeneric & repStr)
{
	s32 i = -2;
	s32 j = 0;
	while (i != -1)
	{
		i = contains(whereStr, j);

		if (i > -1)
		{
			moveWithin(i + whereStr.getLength(), i + repStr.getLength(), getLength());
			replaceAt(i, repStr);
			j = i - 1;
			i = -2;
		}
	}
	return *this;
}

s32 StringGeneric::find(char c)
{
	auto str = getString();
	for (s32 i = 0; i < length; ++i)
	{
		if (str[i] == c)
		{
			return i;
		}
	}
	return -1;
}

void StringGeneric::take(u32 num, StringGeneric & dst)
{
	auto str = getString();
	num = min(num, getLength());
	for (int i = 0; i < num; ++i)
	{
		dst.append(str[i]);
	}
}

void StringGeneric::drop(u32 num)
{
	num = max(0, num);
	num = min(length, num);
	moveSegmentBackward(num, num, length - num, '\0');
	length -= num;
}

StringGeneric & StringGeneric::replaceWhere(StringGeneric && whereStr, StringGeneric && repStr)
{
	return replaceWhere(whereStr, repStr);
}

StringGeneric & StringGeneric::replaceWhere(StringGeneric && whereStr, StringGeneric & repStr)
{
	return replaceWhere(whereStr, repStr);
}

StringGeneric & StringGeneric::replaceWhere(StringGeneric & whereStr, StringGeneric && repStr)
{
	return replaceWhere(whereStr, repStr);
}

void StringGeneric::operator+=(StringGeneric & str)
{
	append(str);
}

bool StringGeneric::operator==(const StringGeneric & str)
{
	if (str.getLength() != getLength())
		return false;

	return compareChars(getString(), (const_cast<StringGeneric&>(str)).getString(), getLength());
}

bool StringGeneric::operator<(const StringGeneric & str) const
{
	//return getString() < str.getString();
	auto len = min(getLength(), str.getLength());
	for (int i = 0; i < len; ++i)
	{
		if (const_cast<StringGeneric*>(this)->getString()[i] != const_cast<StringGeneric&>(str).getString()[i])
		{
			return const_cast<StringGeneric*>(this)->getString()[i] < const_cast<StringGeneric&>(str).getString()[i];
		}
	}
	return getLength() < str.getLength();
}

void StringGeneric::copyStr(StringGeneric & dst, StringGeneric & src, u32 len, u32 start)
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

void StringGeneric::copyChars(char * dst, char * src, u32 len)
{
	u32 i = 0;
	while (i < len)
	{
		dst[i] = src[i];
		++i;
	}
}

bool StringGeneric::compareChars(const char * a, const char * b, u32 len)
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

char * StringGeneric::cReplicate(char chr, u32 len, u32 start)
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

u32 StringGeneric::determineLength(const char * str)
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

u32 StringGeneric::determineLength()
{
	length = determineLength(getString());
	return length;
}

s32 StringGeneric::strtos32(char * begin, char * end)
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

s32 StringGeneric::strtos32(char * begin, s32 len)
{
	return strtos32(begin, begin + len);
}

s32 StringGeneric::strtos32()
{
	return strtos32(getString(), getString() + getLength());
}

void HeapGeneric::overwrite(StringGeneric & source)
{
	length = source.getLength();
	if (length > capacity)
		expand(length * 1.2);
	copyStr(*this, source, length);
	getString()[length] = '\0';
}

void HeapGeneric::replicate(char chr, u32 len, u32 start)
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

StringGeneric & HeapGeneric::append(StringGeneric & str)
{
	if (length + str.getLength() > capacity)
		expand((length + str.getLength()) * 1.2);
	replaceAt(getLength(), str);
	length += str.getLength();
	length = min(length, capacity);
	return *this;
}

StringGeneric & HeapGeneric::append(char chr)
{
	if (length + 1 > capacity)
		autoExpand();
	//replaceAt(getLength(), str);
	insertAt(length, String<1>(chr));
	++length;;
	return *this;
}

StringGeneric & HeapGeneric::append(char * chars, u32 len)
{
	if (length + len > capacity)
		expand((length + len) * 1.2);
	replaceAt(getLength(), chars, len);
	length += len;
	length = min(length, capacity);
	return *this;
}

void HeapGeneric::moveSegmentForward(s32 takeFrom, s32 forward, u32 len, char fill)
{
	if (takeFrom + len + forward > getCapacity())
	{
		expand((takeFrom + len + forward) * 1.2);
	}

	if (takeFrom + len > getCapacity())
		len = getCapacity() - takeFrom;
	if (takeFrom + forward + len > getCapacity())
		len = getCapacity() - (takeFrom + forward);

	for (s32 i = takeFrom + forward + len - 1; i >= takeFrom + forward; --i)
	{
		getString()[i] = getString()[i - forward];
	}
	length += forward;
	length = min(length, capacity);
}

StringGeneric& HeapGeneric::insertAt(u32 i, StringGeneric & x)
{
	if (getLength() + x.getLength() > getCapacity())
	{
		expand((getLength() + x.getLength()) * 1.2);
	}
	moveSegmentForward(i, x.getLength(), getLength());
	replaceAt(i, x);
	return *this;
}

StringGeneric & HeapGeneric::insertWhere(StringGeneric & whereStr, StringGeneric & insStr)
{
	s32 i = -2;
	s32 j = 0;
	while (i != -1)
	{
		i = contains(whereStr, j);

		if (i > -1)
		{
			insertAt(i + 1, insStr);
			j = i + insStr.getLength();
			i = -2;
		}
	}
	return *this;
}

StringGeneric & HeapGeneric::replaceAt(u32 i, StringGeneric & x)
{
	//s32 spaceLeftAfter = (getCapacity() - i - x.getLength());
	//s32 replaceLen = spaceLeftAfter > 0 ? x.getLength() : x.getLength() - spaceLeftAfter;
	if (i + x.getLength() > getCapacity())
		expand((i + x.getLength()) * 1.2);
	copyChars(getString() + i, x.getString(), x.getLength());
	return *this;
}

StringGeneric & HeapGeneric::replaceAt(u32 i, char * chars, u32 len)
{
	if (i + len > getCapacity())
		expand((i + len) * 1.2);
	copyChars(getString() + i, chars, len);
	return *this;
}

#undef min
#undef max