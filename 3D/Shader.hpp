#pragma once
#include <fstream>
#include <string>
#include <regex>
#include <iostream>
#include <sstream>
#include "GL\glew.h"
#include <vector>
#include <glm\glm.hpp>
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\matrix_transform_2d.hpp"
#include "Strings.hpp"
//#include "Window.h"


class ShaderProgram
{
public:
	enum ShaderType { VertFrag, VertGeomFrag, Compute };

private:
	enum UniformType {
		s32x1, s32x2, s32x3, s32x4,
		u32x1, u32x2, u32x3, u32x4,
		s64x1, s64x2, s64x3, s64x4,
		u64x1, u64x2, u64x3, u64x4,
		flt, flt2, flt3, flt4,
		dbl, dbl2, dbl3, dbl4,
		fmat2, fmat3, fmat4,
		dmat2, dmat3, dmat4,
		tex2d,
		UniformTypesCount
	};

	static const int typeSizes[UniformTypesCount];
	static const String<16> typeGLSLNames[UniformTypesCount];

public:
	ShaderProgram();
	ShaderProgram(const ShaderProgram& program);
	~ShaderProgram();

	void destroy();
	void load(String32&& pName, ShaderType pType, String128& pShaderLocationPath = String128("res/shader/"));

	void load(String32& pName, ShaderType pType, String128& pShaderLocationPath = String128("res/shader/"));

	virtual int initialise() { return  0; }

	void compile();

	void reload()
	{
		//VERT/FRAG SHADER RELOAD LEAKS MEMORY WHILE COMPUTE DOESNT

		std::map<String64, UniformMeta> keepUniforms;

		//for (auto itr = uniformLocations.begin(); itr != uniformLocations.end(); ++itr)
		//{
		//	auto find = uniformObjects.find(itr->second);

		//	if (find != uniformObjects.end())
		//	{
		//		keepUniforms.insert(std::make_pair(itr->first, find->second));
		//	}
		//}

		destroy();
		load(name, type);
		compile();

		//if (!initialise())
		//{
		//	for (auto itr = keepUniforms.begin(); itr != keepUniforms.end(); ++itr)
		//	{
		//		auto find = uniformLocations.find(itr->first);

		//		if (find != uniformLocations.end())
		//		{
		//			auto find2 = uniformObjects.find(find->second);

		//			if (find2 != uniformObjects.end())
		//			{
		//				find2->second = itr->second;
		//			}
		//		}
		//	}
		//	sendUniforms();
		//}

		//KEEP OLD VAR VALUES FOR SAME VAR NAMES
		//KEEP OLD UNIFORM INFO FOR SAME UNIFORM NAMES
		//KEEP OLD BUFFER INFO FOR SAME BUFFER NAMES
	}

	GLint getGLID() const { return GLID; }

	void use()
	{
		glUseProgram(GLID);
	}

	void stop()
	{
		glUseProgram(0);
	}

	void setUniform(char* pUniformName, const void* pUniformData, s32 pFlags = 0);
	void setUniform(String64& pUniformName, const void* pUniformData, s32 pFlags = 0);

	void setVarVal(String32& var, String1024& val);

	virtual void sendUniforms();

	String32& getName() { return name; }

	ShaderType getType() { return type; }

private:

	void loadVertFrag(String32& pName, String128& pShaderLocationPath);

	void compileVertFrag();

	void loadVertGeomFrag(String32& pName, String128& pShaderLocationPath);

	void compileVertGeomFrag();

	void loadCompute(String32& pName, String128& pShaderLocationPath);

	void compileCompute();

	void freeSourceContent();

	void extractUniforms(char* pSource);

	void extractBuffers(char* pSource);

	void extractPreprocessorVars(char* pSource);

	char* preprocess(char* pSource, u32 size);

	struct UniformData
	{
		UniformData() {}
		union
		{
			glm::dmat4 dmat4;	//128
			glm::dmat3 dmat3;	//72
			glm::dmat2 dmat2;	//32

			glm::fmat4 fmat4;	//64
			glm::fmat3 fmat3;	//36
			glm::fmat2 fmat2;	//16

			glm::dvec4 dbl4;	//32
			glm::dvec3 dbl3;	//24
			glm::dvec2 dbl2;	//16
			double dbl;			//8

			glm::fvec4 flt4;	//16
			glm::fvec3 flt3;	//12
			glm::fvec2 flt2;	//8
			float flt;			//4

			glm::ivec4 s32x4;	//16
			glm::ivec3 s32x3;	//12
			glm::ivec2 s32x2;	//8
			s32 s32x1;			//4

			glm::uvec4 u32x4;	//16
			glm::uvec3 u32x3;	//12
			glm::uvec2 u32x2;	//8
			u32 u32x1;			//4

			glm::i64vec4 s64x4; //32
			glm::i64vec3 s64x3; //24
			glm::i64vec2 s64x2; //16
			s64 s64x1;			//8

			glm::u64vec4 u64x4; //32
			glm::u64vec3 u64x3; //24
			glm::u64vec2 u64x2; //16
			u64 u64x1;			//8

			u64 tex2d;			//8

			void* array;
		};
	};

	struct UniformMeta
	{
		UniformMeta() {};

		void operator=(UniformMeta& rhs)
		{
			type = rhs.type;
			name.overwrite(rhs.name);
			data = rhs.data;
			arrayCount = rhs.arrayCount;
			flags = rhs.flags;
		}

		void updateLocation(ShaderProgram& program)
		{
			location = glGetUniformLocation(program.getGLID(), name.getString());
		}

		UniformType type;
		String64 name;
		UniformData data;
		GLsizei arrayCount; //1 == not array
		s32 flags;
		GLint location;
	};

	std::map<String64, UniformMeta> uniformObjects;
	//std::map<GLint, UniformMeta> uniformObjects;
	

	union {
		struct {
			String128 geomPath;
			String128 vertexPath;
			String128 fragmentPath;
		};
		String128 computePath;
	};

	union {
		struct {
			char* geomContent;
			char* vertexContent;
			char* fragmentContent;
		};
		char* computeContent;
	};

	union {
		struct {
			u32 geomSize;
			u32 vertexSize;
			u32 fragmentSize;
		};
		u32 computeSize;
	};

	struct VarAndVal
	{
		String32 var;
		String1024 val;
	};

	VarAndVal varVals[32];
	
protected:

	String32 name;
	String<HEAP> errorLog;
	bool loaded;
	ShaderType type;
	GLint GLID;

};