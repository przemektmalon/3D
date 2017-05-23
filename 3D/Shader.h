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
#include "ShaderPreprocessor.h"
#include "Strings.h"
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

class Shader : public ShaderPreprocessor
{
public:
	Shader() : program(0) { }
	~Shader() { if(program != 0) glDeleteProgram(program); }

	GLuint load(String128 pPathVert, String128 pPathFrag, bool preprocess = false)
	{
		if (pPathVert.contains(String32(".vert")) == -1)
			vertPath.overwrite(pPathVert.append(String32(".vert")));
		else
			vertPath.overwrite(pPathVert);

		if (pPathFrag.contains(String32(".frag")) == -1)
			fragPath.overwrite(pPathFrag.append(String32(".frag")));
		else
			fragPath.overwrite(pPathFrag);

		std::ifstream vertStream(vertPath.getString(), std::ifstream::in);
		std::ifstream fragStream(fragPath.getString(), std::ifstream::in);

		if (vertStream.fail())
		{
			//std::cout << "Failed loading vertex shader: " << vertPath << std::endl;
			return 0;
		}

		if (fragStream.fail())
		{
			//std::cout << "Failed loading fragment shader: " << fragPath << std::endl;
			return 0;
		}

		std::string fragContent;
		std::string vertContent;

		fragContent.assign(std::istreambuf_iterator<char>(fragStream), std::istreambuf_iterator<char>());
		vertContent.assign(std::istreambuf_iterator<char>(vertStream), std::istreambuf_iterator<char>());

		GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		GLchar* glCharFrag = &fragContent[0];
		GLchar* glCharVert = &vertContent[0];

		if (preprocess)
		{
			//String32 e = String32("exposure");
			//String1024 v = String1024("1.f");
			//setVarVal(e, v);
			glCharFrag = preProcess(&fragContent[0], fragContent.size());
			glCharVert = preProcess(&vertContent[0], vertContent.size());
		}

		//const GLchar* const glCharFrag = fragContent.c_str();
		//const GLchar* const glCharVert = vertContent.c_str();

		glShaderSource(fragmentShader, 1, &glCharFrag, NULL);
		glShaderSource(vertexShader, 1, &glCharVert, NULL);

		if (preprocess)
		{
			delete[] glCharFrag;
			delete[] glCharFrag;
		}

		auto err = glGetError();
		glCompileShader(fragmentShader);
		GLint isCompiled = GL_FALSE;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		err = glGetError();
		if (isCompiled == GL_FALSE)
		{
			//std::cout << "Failed to compile fragment shader: " << pPathFrag << std::endl;
		}

		glCompileShader(vertexShader);
		isCompiled = GL_FALSE;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			//std::cout << "Failed to compile vertex shader: " << pPathVert << std::endl;
		}

		fragStream.close();
		vertStream.close();

		auto shaderProgram = glCreateProgram();

		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glBindFragDataLocation(shaderProgram, 0, "outColour");
		glLinkProgram(shaderProgram);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		isCompiled = GL_FALSE;
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			//std::cout << "Failed to link shader: " << pPathFrag << " + " << pPathVert << std::endl;

			GLint maxLength = 0;
			glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> errorLog(maxLength);
			glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &errorLog[0]);

			for (int i = 0; i < errorLog.size(); ++i)
			{
				std::cout << char(errorLog.data()[i]);
			}

			glDeleteProgram(shaderProgram);

			shaderProgram = 0;

			return 0;
		}

		//std::cout << "Shader program " << pPathFrag << " + " << pPathVert << " loaded and linked successfully" << std::endl;

		program = shaderProgram;

		return shaderProgram;
	}

	virtual void reload(bool preprocess = true)
	{
		std::ifstream vertStream(vertPath.getString(), std::ifstream::in);
		std::ifstream fragStream(fragPath.getString(), std::ifstream::in);

		if (vertStream.fail())
		{
			//std::cout << "Failed loading vertex shader: " << vertPath << std::endl;
			return;
		}

		if (fragStream.fail())
		{
			//std::cout << "Failed loading fragment shader: " << fragPath << std::endl;
			return;
		}

		std::string fragContent;
		std::string vertContent;

		fragContent.assign(std::istreambuf_iterator<char>(fragStream), std::istreambuf_iterator<char>());
		vertContent.assign(std::istreambuf_iterator<char>(vertStream), std::istreambuf_iterator<char>());

		GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		GLchar* glCharFrag = &fragContent[0];
		GLchar* glCharVert = &vertContent[0];

		if (preprocess)
		{
			String32 e = String32("exposure");
			String1024 v = String1024("1.f");
			setVarVal(e, v);
			glCharFrag = preProcess(&fragContent[0], fragContent.size());
			glCharVert = preProcess(&vertContent[0], vertContent.size());
		}

		//const GLchar* const glCharFrag = fragContent.c_str();
		//const GLchar* const glCharVert = vertContent.c_str();

		glShaderSource(fragmentShader, 1, &glCharFrag, NULL);
		glShaderSource(vertexShader, 1, &glCharVert, NULL);

		if (preprocess)
		{
			delete[] glCharFrag;
			delete[] glCharFrag;
		}

		auto err = glGetError();
		glCompileShader(fragmentShader);
		GLint isCompiled = GL_FALSE;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		err = glGetError();
		if (isCompiled == GL_FALSE)
		{
			//std::cout << "Failed to compile fragment shader: " << pPathFrag << std::endl;
		}

		glCompileShader(vertexShader);
		isCompiled = GL_FALSE;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			//std::cout << "Failed to compile vertex shader: " << pPathVert << std::endl;
		}

		fragStream.close();
		vertStream.close();

		auto shaderProgram = glCreateProgram();

		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glBindFragDataLocation(shaderProgram, 0, "outColour");
		glLinkProgram(shaderProgram);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		isCompiled = GL_FALSE;
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			//std::cout << "Failed to link shader: " << pPathFrag << " + " << pPathVert << std::endl;

			GLint maxLength = 0;
			glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> errorLog(maxLength);
			glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &errorLog[0]);

			for (int i = 0; i < errorLog.size(); ++i)
			{
				std::cout << char(errorLog.data()[i]);
			}

			glDeleteProgram(shaderProgram);

			return;
		}

		//std::cout << "Shader program " << pPathFrag << " + " << pPathVert << " loaded and linked successfully" << std::endl;

		glDeleteProgram(program);

		program = shaderProgram;
	}

	virtual void initialise() {}

	GLint operator()()
	{
		return program;
	}

	void use() { glUseProgram(program); }
	void stop() { glUseProgram(0); }

protected:

	String128 vertPath;
	String128 fragPath;

	GLuint program;
};