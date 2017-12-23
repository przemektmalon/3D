/*#pragma once
#include <string>
#include "Types.hpp"
#include "Text.hpp"
#include <tuple>
#include <cmath>
#include <typeinfo>
#include <type_traits>
#include <utility>
#include <functional>
#include "TextShader.hpp"

template <typename> struct FnArgs;

template <typename R, typename ...Args>
struct FnArgs<R(Args...)>
{
	using type = std::tuple<Args...>;
};

#define PARAM_TUPLE(func) FnArgs<decltype(func)>::type::tuple()
#define PARAM_TUPLE_TYPE(func) FnArgs<decltype(func)>::type
#define PARAM_TUPLE_SIZE(func) std::tuple_size<PARAM_TUPLE_TYPE(func)>::value
#define PARAM_TUPLE_SIZE_INTERM(func) PARAM_TUPLE_SIZE(func)
#define NTH_PARAM_DATA(param_vector, n) param_vector[n].data
#define NTH_PARAM_TYPE(func, n) std::tuple_element<n, PARAM_TUPLE_TYPE(func)>::type
#define CHEAT_CAST(func,n,params) *reinterpret_cast<NTH_PARAM_TYPE(func,n)*>(NTH_PARAM_DATA(params,n))

#define CONSTRUCT_PARAMS_6(func,params) CHEAT_CAST(func, 0, params), CHEAT_CAST(func, 1, params), CHEAT_CAST(func, 2, params), CHEAT_CAST(func, 3, params), CHEAT_CAST(func, 4, params), CHEAT_CAST(func, 5, params)
#define CONSTRUCT_PARAMS_5(func,params) CHEAT_CAST(func, 0, params), CHEAT_CAST(func, 1, params), CHEAT_CAST(func, 2, params), CHEAT_CAST(func, 3, params), CHEAT_CAST(func, 4, params)
#define CONSTRUCT_PARAMS_4(func,params) CHEAT_CAST(func, 0, params), CHEAT_CAST(func, 1, params), CHEAT_CAST(func, 2, params), CHEAT_CAST(func, 3, params)
#define CONSTRUCT_PARAMS_3(func,params) CHEAT_CAST(func, 0, params), CHEAT_CAST(func, 1, params), CHEAT_CAST(func, 2, params)
#define CONSTRUCT_PARAMS_2(func,params) CHEAT_CAST(func, 0, params), CHEAT_CAST(func, 1, params)
#define CONSTRUCT_PARAMS_1(func,params) CHEAT_CAST(func, 0, params)
#define CONSTRUCT_PARAMS_0(func,params)

#define CONSTRUCT_PARAMS_AUTO(func,params,numParms)
#define CONSTRUCT_PARAMS_INTERM(func,params,numParams) CONSTRUCT_PARAMS_##numParams## (func,params)
#define CONSTRUCT_PARAMS(func,params,numParams) CONSTRUCT_PARAMS_INTERM(func,params,numParams)

#define BEGIN_FUNC_SWITCH(name) \
auto funcID = funcIDs.find(name); \
if(funcID == funcIDs.end()) { String64 msg("Function \""); msg.append(name); msg.append("\" not found."); postError(msg); return; } \
switch(funcID->second) {

#define CHECK_CONSOLE_CALLABLE(id,func,numParams) \
case(id):{ \
if(params.size() > numParams) \
	{ postError(String64("Too many arguments passed. Taking required amount")); } \
if(params.size() < numParams) { \
	std::string str("Not enough arguments passed. "); str += std::to_string(numParams) + std::string(" arguments expected. ") + std::to_string(params.size()) + std::string(" were passed."); \
	postError(String128(str.c_str())); return; } \
auto task = MAKE_TASK(func)(CONSTRUCT_PARAMS_##numParams##(func,params,PARAM_TUPLE_SIZE(func))); \
pvoidary pFunc; \
void* pVoid; \
std::tie(pFunc, pVoid) = task; \
pFunc(pVoid); \
delete std::get<1>(task); break; }

#define END_FUNC_SWITCH }

#define REGISTER_CONSOLE_CALLABLE(id,funcName,numParams) funcIDs.insert(std::make_pair(String32(funcName),id));

template<typename T>
struct function_traits;

template<typename R, typename ...Args>
struct function_traits<std::function<R(Args...)>>
{
	static const size_t nargs = sizeof...(Args);

	typedef R result_type;

	template <size_t i>
	struct arg
	{
		typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
	};
};

template<int...> struct seq {};
template<int Min, int Max, int... s> struct make_seq :make_seq<Min, Max - 1, Max - 1, s...> {};
template<int Min, int... s> struct make_seq<Min, Min, s...> {
	typedef seq<s...> type;
};
template<int Max, int Min = 0>
using MakeSeq = typename make_seq<Min, Max>::type;


template<typename Func, Func f, typename Tuple, int... s>
void do_call(seq<s...>, Tuple&& tup) {
	f(std::get<s>(tup)...);
}

typedef void(*pvoidary)(void*);

template<typename FuncType, FuncType Func, typename... Args>
std::tuple<pvoidary, std::tuple<Args...>*> make_task(Args&&... args) {
	typedef std::tuple<Args...> pack;
	pack* pvoid = new pack(std::forward<Args>(args)...);
	return std::make_tuple(
		[](void* pdata)->void {
		pack* ppack = reinterpret_cast<pack*>(pdata);
		do_call<FuncType, Func>(MakeSeq<sizeof...(Args)>(), *ppack);
	},
		pvoid
		);
}

#define MAKE_TASK( FUNC ) make_task< typename std::decay<decltype(FUNC)>::type, FUNC >

class Console
{
	friend class Engine;
public:
	Console() {}
	~Console() {}

	enum Type { Err, Int, Flt, Str, Vec };

	struct CNumber
	{
		union
		{
			s32 integer;
			float floating;
		};
		Type type;

		void negate()
		{
			switch (type)
			{
			case Int:
				integer = -integer;
				break;
			case Flt:
				floating = -floating;
				break;
			}
		}
	};

	static void registerConsoleFuncs();

	static CNumber interpretNumber(char** ccp)
	{
		char* cc = *ccp;
		CNumber ret; ret.type = Int;
		u8 len = 0;
		u8 len2 = 0;
		u8* lenP = &len;
		u8 nums[32];

		char curChar = *cc;

		for (;;)
		{
			if (*cc == ' ' || *cc == '\n' || *cc == '\0' || *cc == ',' || *cc == ')')
				break;

			if (*cc == '.')
			{
				if (ret.type == Flt)
				{
					ret.type = Err; 
					return ret;
				}
				ret.type = Flt; 
				len2 = len;
				lenP = &len2;
				++*ccp;
				cc = *ccp;
				continue;
			}

			if (!isNum(*cc))
			{
				ret.type = Err; return ret;
			}
			
			nums[*lenP] = *cc - 48;
			++*lenP;

			++*ccp;
			cc = *ccp;

			if (len + len2 == 32)
			{
				ret.type = Err; return ret;
			}
		}

		len2 -= len;

		switch (ret.type)
		{
		case Err:
			return ret;
		case Int:
			ret.integer = 0;
			for (u8 i = 0; i < len; ++i)
			{
				ret.integer += u32(nums[i]) * u32(std::pow(10,len-i-1));
			}
			return ret;
		case Flt:
			ret.floating = 0;
			for (u8 i = 0; i < len; ++i)
			{
				ret.floating += float(nums[i]) * float(std::pow(10, len - i - 1));
			}
			for (u8 i = 0; i < len2; ++i)
			{
				ret.floating += float(nums[len+i]) / float(std::pow(10.f, i+1));
			}
			return ret;
		}

	}

	struct CVector
	{
		CVector() {}
		union
		{
			glm::fvec4 fvec4;
			glm::fvec3 fvec3;
			glm::fvec2 fvec2;

			glm::ivec4 ivec4;
			glm::ivec3 ivec3;
			glm::ivec2 ivec2;
		};
		Type type;
		u8 length;

		void negate()
		{
			switch (type)
			{
			case Int:
				ivec4 = -ivec4;
				break;
			case Flt:
				fvec4 = -fvec4;
				break;
			}
		}
	};

	static CVector interpretVector(char** ccp)
	{
		char* cc = *ccp;
		CVector ret; ret.type = Err;
		CNumber nums[4];
		u8 curNum = 0;

		char curChar = *cc;

		for (;;)
		{
			++*ccp;
			curChar = *(*ccp);
			cc = *ccp;
			if (*cc == ' ')
			{
				continue;
			}

			if (*cc == ')')
			{
				break;
			}

			if (*cc == ',')
			{
				if (++curNum == 4) 
				{ ret.type = Err; return ret; }
			}

			if (*cc == '-')
			{
				++*ccp;
				nums[curNum] = interpretNumber(ccp);
				nums[curNum].negate();
				switch (nums[curNum].type)
				{
				case Err:
					ret.type = Err; return ret;
				case Int:
					ret.ivec4[curNum] = nums[curNum].integer; break;
				case Flt:
					ret.fvec4[curNum] = nums[curNum].floating; break;
				default:
					ret.type = nums[curNum].type == nums[0].type ? nums[curNum].type : Err;
				}
				++curNum;
				if (*cc == ',')
					if (curNum == 4) { 
						ret.type = Err; return ret; }
				if (*cc == ')')
					break;
				else
					continue;
			}

			if (!isNum(*cc))
			{
				ret.type = Err; return ret;
			}

			nums[curNum] = interpretNumber(ccp);
			cc = *ccp;
			switch (nums[curNum].type)
			{
			case Err:
				ret.type = Err; return ret;
			case Int:
				ret.ivec4[curNum] = nums[curNum].integer; break;
			case Flt:
				ret.fvec4[curNum] = nums[curNum].floating; break;
			default:
				ret.type = nums[curNum].type == nums[0].type ? nums[curNum].type : Err;
			}
			++curNum;
			if (*cc == ',')
				if (curNum == 4) { ret.type = Err; return ret; }
			if (*cc == ')')
				break;
		}
		
		ret.length = curNum;
		return ret;
	}

	static String32 interpretString(char** ccp)
	{
		String32 ret;
		char cc;
		for (;;)
		{
			++*ccp;
			cc = *(*ccp);
			if (cc == '\"' || cc == '\0')
			{
				break;
			}
			ret.append(cc);
		}
		return ret;
	}

	struct CParam
	{
		CParam() : type(Err) {}
		CParam(CParam& pPar) : type(pPar.type) , str(pPar.str) {}
		CParam(CNumber* pNum) : num(pNum) { type = pNum->type; }
		CParam(CVector* pVec) : vec(pVec) { type = Vec; }
		CParam(String32* pStr) : str(pStr), type(Str) {}
		union
		{
			void* data;
			String32* str;
			CVector* vec;
			CNumber* num;
		};
		Type type;
	};

	void submitCommand(std::string& command);
	void postMessage(std::string& post, char prompt = '>')
	{
		std::vector<std::string*> lines;

		int beg = 0;
		for (auto i = 0; i < post.length(); ++i)
		{
			if (post.c_str()[i] == '\n')
			{
				//lines.push_back(new std::string(post.c_str() + (beg == 0 ? beg : beg + 1), post.c_str() + i));
				beg = i;
			}
			if (i == post.length() - 1)
			{
				//lines.push_back(new String<HEAP>(post.c_str() + (beg == 0 ? beg : beg + 1), post.c_str() + i + 1));
			}
		}

		for (auto itr = lines.begin(); itr != lines.end(); ++itr)
		{
			/*auto newPost = new Text2D();
			newPost->init();
			newPost->setFont(cmd.getFont());
			newPost->setCharSize(25);
			newPost->setString(String<2>(prompt));
			newPost->getString().append(" ");
			newPost->getString().append(*(*itr));
			//consoleHistory.push_back(newPost);
		}

		for (auto itr = lines.begin(); itr != lines.end(); ++itr)
		{
			delete *itr;
		}

		repositionText();
	}
	void postResult(StringGeneric& result)
	{
		//postMessage(result, '#');
	}
	void postError(StringGeneric& error)
	{
		//postMessage(error, '!');
	}
	void textInput(Key code);
	
	void repositionText()
	{
		s32 i = consoleHistory.size();
		for (auto itr = consoleHistory.begin(); itr != consoleHistory.end(); ++itr)
		{
			(*itr)->setPosition(glm::fvec2(10, 5 + heightFromBottom + (i)*((*itr)->getCharSize()+1)));
			--i;
		}
		cmd.setPosition(glm::fvec2(10, heightFromBottom));
	}

	void draw()
	{
		if (stateFlags == 0)
			return;

		for (auto itr = consoleHistory.begin(); itr != consoleHistory.end(); ++itr)
		{
			(*itr)->shader->setProj(Engine::window.getOrthoProj());
			(*itr)->shader->setModel(glm::fmat4());
			(*itr)->shader->setView(glm::fmat4());
			(*itr)->shader->setFontBinding(12);
			(*itr)->draw();
		}
		cmd.shader->setProj(Engine::window.getOrthoProj());
		cmd.shader->setModel(glm::fmat4());
		cmd.shader->setView(glm::fmat4());
		cmd.shader->setFontBinding(12);
		cmd.draw();
	}

	void init();

	void toggle()
	{
		stateFlags = stateFlags == 1 ? 0 : 1;
	}

private:

	static std::map<String32, u32> funcIDs;

	std::vector<Text2D*> consoleHistory;
	std::vector<std::string> cmdHistory;
	Text2D cmd;
	u32 cursor;
	s32 stateFlags; // 0 == closed, 1 == open, others ?
	const float heightFromBottom = 200.f;

	static bool isNum(char c)
	{
		return c > 47 && c < 58;
	}

	static bool isAlpha(char c)
	{
		return (c > 64 && c < 91) || (c > 96 && c < 123);
	}

	static bool isPrintable(char c)
	{
		return (c > 31 && c < 127);
	}

};*/