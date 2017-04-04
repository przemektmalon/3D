/*#include "StackStrings.h"

#define min(a,b) a > b ? b : a
#define max(a,b) a > b ? a : b

bool operator<(const StackString& lhs, const StackString& rhs)
{
auto len = min(lhs.getLength(), rhs.getLength());
for (int i = 0; i < len; ++i)
{
if (lhs.getString()[i] < rhs.getString()[i])
{
return true;
}
}
return false;
}

#undef min
#undef max*/