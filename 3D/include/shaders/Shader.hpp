#pragma once
#include <fstream>
#include <string>
#include <regex>
#include <iostream>
#include <sstream>
#include "GL\glew.h"
#include <vector>
#include "MathIncludes.hpp"
#include "Types.hpp"
#include <map>



class ShaderProgram
{
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
	static const char typeGLSLNames[UniformTypesCount][16];

public:
	struct UniformMeta
	{
		UniformMeta() {};

		void operator=(UniformMeta& rhs)
		{
			type = rhs.type;
			name = rhs.name;
			data = rhs.data;
			arrayCount = rhs.arrayCount;
			flags = rhs.flags;
		}

		void updateLocation(ShaderProgram& program)
		{
			location = glGetUniformLocation(program.getGLID(), name.c_str());
		}

		UniformType type;
		std::string name;
		union DataUnion
		{
			DataUnion() {}
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
		}data;
		GLsizei arrayCount; //1 == not array
		s32 flags;
		GLint location;
	};

	std::map<std::string, UniformMeta> uniformObjects;
	//std::map<GLint, UniformMeta> uniformObjects;


	std::string geomPath;
	std::string vertexPath;
	std::string fragmentPath;
	std::string computePath;

	char* geomContent;
	char* vertexContent;
	char* fragmentContent;
	char* computeContent;

	u32 geomSize;
	u32 vertexSize;
	u32 fragmentSize;
	u32 computeSize;

	struct VarAndVal
	{
		std::string var;
		std::string val;
	};

	VarAndVal varVals[32];
	
	enum ShaderType { VertFrag, VertGeomFrag, Compute };

protected:

	std::string name;
	std::string errorLog;
	bool loaded;
	ShaderType type;
	GLint GLID;

public:
	ShaderProgram();
	ShaderProgram(const ShaderProgram& program);
	~ShaderProgram();

	void destroy();
	void load(std::string&& pName, ShaderType pType, std::string pShaderLocationPath);
	void load(std::string&& pName, ShaderType pType) { load(pName, pType, std::string("res/shader/")); }
	void load(std::string& pName, ShaderType pType, std::string pShaderLocationPath);
	void load(std::string& pName, ShaderType pType) { load(pName, pType, std::string("res/shader/")); }

	virtual int initialise() { return  0; }

	void compile();

	void reload()
	{
		/// TODO: keep all uniforms and glsl data after shader reloading
		/// This might be more involved if any of the uniform locations change after the reload
		/// But this would either imply major changes to the data section or shader
		/// Just sending data to the same uniform location will be fine for now
		/// For later, there is a good data structure(UniformMeta) that stores uniform names, locations, types and values
		/// we can track changes over the reload from that data structure, but its not populated atm for specialised Shaders that have their own special class (eg SAOShader.hpp)

		destroy();
		load(name, type);
		compile();
		sendUniforms();
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

	void setUniform(std::string pUniformName, const void* pUniformData, s32 pFlags = 0);

	void setVarVal(std::string& var, std::string& val);

	virtual void sendUniforms();

	std::string& getName() { return name; }

	ShaderType getType() { return type; }

private:

	void loadVertFrag(std::string& pName, std::string& pShaderLocationPath);

	void compileVertFrag();

	void loadVertGeomFrag(std::string& pName, std::string& pShaderLocationPath);

	void compileVertGeomFrag();

	void loadCompute(std::string& pName, std::string& pShaderLocationPath);

	void compileCompute();


	bool checkShaderCompilation(GLuint pShader, char* pType);
	bool checkProgramLinking(GLuint pProgram);
	void reportFailedShaderLoad(char* pType, char* pPath);


	void freeSourceContent();

	void extractUniforms(char* pSource);

	void extractBuffers(char* pSource);

	void extractPreprocessorVars(char* pSource);

	char* preprocess(char* pSource, u32 size);
};