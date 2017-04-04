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
//#include "Window.h"

class Shader : public ShaderPreprocessor
{
public:
	Shader() : program(0) { }
	~Shader() { if (program != 0) glDeleteProgram(program); }

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

class StandardShader : public Shader
{
public:
	StandardShader() {
		load("Standard", "Standard");

	}
	~StandardShader() {}

	enum Uniform
	{
		viewPos = 0,
		diffuse = 1,
		specular = 2,
		emit = 3,
		emitStrength = 4,
		shininess = 5,
		proj = 6,
		model = 7
	};

	void setViewPos(glm::fvec3 pos)
	{
		glUniform3f(Uniform::viewPos, pos.x, pos.y, pos.z);
	}

	void setDiffuse(GLuint texUnit)
	{
		glUniform1i(Uniform::diffuse, texUnit);
	}
	void setSpecular(GLuint texUnit)
	{
		glUniform1i(Uniform::specular, texUnit);
	}
	void setEmit(GLuint texUnit)
	{
		glUniform1i(Uniform::emit, texUnit);
	}

	void setEmitStrength(float str)
	{
		glUniform1f(Uniform::emitStrength, str);
	}

	void setShininess(float shin)
	{
		glUniform1f(Uniform::shininess, shin);
	}

	void setProjMatrix(glm::fmat4 proj)
	{
		//glUniformMatrix4fv(Uniform::proj, 1, GL_FALSE, glm::value_ptr(proj));
	}

	void setModelMatrix(glm::fmat4 model)
	{
		//glUniformMatrix4fv(Uniform::model, 1, GL_FALSE, glm::value_ptr(model));
	}
};

class ShaderProgram
{
public:
	enum ShaderType { VertFrag, Compute };

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
	static const String16 typeGLSLNames[UniformTypesCount];

public:
	ShaderProgram() : vertexPath(), fragmentPath() {}
	~ShaderProgram() {}

	void load(String64&& pName, ShaderType pType, String128& pShaderLocationPath = String128("res/shader/"))
	{
		//uniformLocations.insert(std::make_pair(pName, 0));
		type = pType;
		switch (type)
		{
		case VertFrag:
			loadVertFrag(pName, pShaderLocationPath);
			break;
		case Compute:
			loadCompute(pName, pShaderLocationPath);
			break;
		}
	}

	void load(String64& pName, ShaderType pType, String128& pShaderLocationPath = String128("res/shader/"))
	{
		//uniformLocations.insert(std::make_pair(pName, 0));
		type = pType;
		switch (type)
		{
		case VertFrag:
			loadVertFrag(pName, pShaderLocationPath);
			break;
		case Compute:
			loadCompute(pName, pShaderLocationPath);
			break;
		}
	}

	GLint getGLID() { return GLID; }

	void compile()
	{
		switch (type)
		{
		case VertFrag:
			compileVertFrag();
			break;
		case Compute:
			compileCompute();
			break;
		}
	}

	void recompile()
	{
		compile();
	}

	void reload()
	{
		//KEEP OLD VAR VALUES FOR SAME VAR NAMES
		//KEEP OLD UNIFORM INFO FOR SAME UNIFORM NAMES
		//KEEP OLD BUFFER INFO FOR SAME BUFFER NAMES
	}

	void use()
	{
		glUseProgram(GLID);
	}

	void stop()
	{
		glUseProgram(0);
	}

	void setUniform(String64& pUniformName, void* pUniformData)
	{
		auto nameLocItr = uniformLocations.find(pUniformName);

		if (nameLocItr == uniformLocations.end())
			assert(0);

		UniformMeta& meta = uniformObjects.at(nameLocItr->second);
		auto size = typeSizes[meta.type];
		auto count = meta.arrayCount;

		if (count == 1)
			memcpy(&meta.data, pUniformData, size);
		else
			memcpy(meta.data.array, pUniformData, size * count);
	}

	void setVarVal(String32& var, String1024& val)
	{
		for (u32 i = 0; i < 32; ++i)
		{
			if (varVals[i].var == var)
			{
				varVals[i].val.overwrite(val);
				break;
			}

			if (varVals[i].var.getString()[0] == '\0')
			{
				varVals[i].val.overwrite(val);
				varVals[i].var.overwrite(var);
				break;
			}
		}
	}

	void sendUniforms()
	{
		for (auto itr = uniformObjects.begin(); itr != uniformObjects.end(); ++itr)
		{
			auto location = itr->first;
			UniformMeta& meta = itr->second;
			auto count = meta.arrayCount;

			switch (meta.type)
			{
			case s32x1:
				glProgramUniform1iv(GLID, location, count, &meta.data.s32x1);
				break;
			case s32x2:
				glProgramUniform2iv(GLID, location, count, &meta.data.s32x1);
				break;
			case s32x3:
				glProgramUniform3iv(GLID, location, count, &meta.data.s32x1);
				break;
			case s32x4:
				glProgramUniform4iv(GLID, location, count, &meta.data.s32x1);
				break;
			case u32x1:
				glProgramUniform1uiv(GLID, location, count, &meta.data.u32x1);
				break;
			case u32x2:
				glProgramUniform2uiv(GLID, location, count, &meta.data.u32x1);
				break;
			case u32x3:
				glProgramUniform3uiv(GLID, location, count, &meta.data.u32x1);
				break;
			case u32x4:
				glProgramUniform4uiv(GLID, location, count, &meta.data.u32x1);
				break;
			case s64x1:
				glProgramUniform1i64vARB(GLID, location, count, &meta.data.s64x1);
				break;
			case s64x2:
				glProgramUniform2i64vARB(GLID, location, count, &meta.data.s64x1);
				break;
			case s64x3:
				glProgramUniform3i64vARB(GLID, location, count, &meta.data.s64x1);
				break;
			case s64x4:
				glProgramUniform4i64vARB(GLID, location, count, &meta.data.s64x1);
				break;
			case u64x1:
				glProgramUniform1ui64vARB(GLID, location, count, &meta.data.u64x1);
				break;
			case u64x2:
				glProgramUniform2ui64vARB(GLID, location, count, &meta.data.u64x1);
				break;
			case u64x3:
				glProgramUniform3ui64vARB(GLID, location, count, &meta.data.u64x1);
				break;
			case u64x4:
				glProgramUniform4ui64vARB(GLID, location, count, &meta.data.u64x1);
				break;
			case flt:
				glProgramUniform1fv(GLID, location, count, &meta.data.flt);
				break;
			case flt2:
				glProgramUniform2fv(GLID, location, count, &meta.data.flt);
				break;
			case flt3:
				glProgramUniform3fv(GLID, location, count, &meta.data.flt);
				break;
			case flt4:
				glProgramUniform4fv(GLID, location, count, &meta.data.flt);
				break;
			case dbl:
				glProgramUniform1dv(GLID, location, count, &meta.data.dbl);
				break;
			case dbl2:
				glProgramUniform2dv(GLID, location, count, &meta.data.dbl);
				break;
			case dbl3:
				glProgramUniform3dv(GLID, location, count, &meta.data.dbl);
				break;
			case dbl4:
				glProgramUniform4dv(GLID, location, count, &meta.data.dbl);
				break;
			case fmat2:
				glProgramUniformMatrix2fv(GLID, location, count, GL_FALSE, &meta.data.fmat2[0][0]);
				break;
			case fmat3:
				glProgramUniformMatrix3fv(GLID, location, count, GL_FALSE, &meta.data.fmat2[0][0]);
				break;
			case fmat4:
				glProgramUniformMatrix4fv(GLID, location, count, GL_FALSE, &meta.data.fmat2[0][0]);
				break;
			case dmat2:
				glProgramUniformMatrix2dv(GLID, location, count, GL_FALSE, &meta.data.dmat2[0][0]);
				break;
			case dmat3:
				glProgramUniformMatrix3dv(GLID, location, count, GL_FALSE, &meta.data.dmat2[0][0]);
				break;
			case dmat4:
				glProgramUniformMatrix4dv(GLID, location, count, GL_FALSE, &meta.data.dmat2[0][0]);
				break;
			case tex2d:
				glProgramUniform1i(GLID, location, meta.data.tex2d);
				break;
			}
		}
	}

private:

	void loadVertFrag(String64& pName, String128& pShaderLocationPath)
	{
		String128 shaderPath; shaderPath.overwrite(pShaderLocationPath);

		vertexPath.overwrite((shaderPath.append(pName)).append(String8(".vert")));
		fragmentPath.overwrite((shaderPath.shrinkBy(5)).append(String8(".frag")));

		std::ifstream vertStream(vertexPath.getString(), std::ifstream::in | std::ifstream::ate);
		std::ifstream fragStream(fragmentPath.getString(), std::ifstream::in | std::ifstream::ate);

		if (vertStream.fail())
		{
			return; //TODO: LOG ERROR
		}

		if (fragStream.fail())
		{
			return; //TODO: LOG ERROR
		}

		vertexSize = 1 + vertStream.tellg();
		fragmentSize = 1 + fragStream.tellg();

		vertexContent = new char[vertexSize];
		fragmentContent = new char[fragmentSize];

		vertexContent[vertexSize - 1] = '\0';
		fragmentContent[fragmentSize - 1] = '\0';

		vertStream.seekg(0);
		fragStream.seekg(0);

		vertStream.read(vertexContent, vertexSize - 1);
		fragStream.read(fragmentContent, fragmentSize - 1);

		vertStream.close();
		fragStream.close();

		extractPreprocessorVars(vertexContent);
		extractPreprocessorVars(fragmentContent);
	}

	void compileVertFrag()
	{
		GLchar* glCharVert = vertexContent;
		GLchar* glCharFrag = fragmentContent;

		glCharVert = preprocess(vertexContent, vertexSize);
		glCharFrag = preprocess(fragmentContent, fragmentSize);

		GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vertexShader, 1, &glCharVert, NULL);
		glShaderSource(fragmentShader, 1, &glCharFrag, NULL);


		glCompileShader(fragmentShader);
		GLint isCompiled = GL_FALSE;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			//return; //TODO: LOG ERROR
		}

		glCompileShader(vertexShader);
		isCompiled = GL_FALSE;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			//return; //TODO: LOG ERROR
		}

		auto shaderProgram = glCreateProgram();

		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);

		//glBindFragDataLocation(shaderProgram, 0, "outColour");

		glLinkProgram(shaderProgram);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		isCompiled = GL_FALSE;
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			//TODO: LOG ERROR

			GLint maxLength = 0;
			glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> errorLog(maxLength);
			glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &errorLog[0]);

			for (int i = 0; i < errorLog.size(); ++i)
			{

			}

			//OutputDebugStringA((errorLog.data()));

			glDeleteProgram(shaderProgram);

			shaderProgram = 0;

			return;
		}

		GLID = shaderProgram;

		use();
		extractUniforms(vertexContent);
		extractUniforms(fragmentContent);

		return;
	}

	void loadCompute(String64& pName, String128& pShaderLocationPath)
	{
		computePath.overwrite(pShaderLocationPath.append(pName).append(String32(".vert")));

		std::ifstream computeStream(computePath.getString(), std::ifstream::in | std::ifstream::ate);

		if (computeStream.fail())
		{
			return; //TODO: LOG ERROR
		}

		computeSize = 1 + computeStream.tellg();

		computeContent = new char[computeSize];

		computeContent[computeSize - 1] = '\0';

		computeStream.seekg(0);
		computeStream.read(computeContent, computeSize - 1);
		computeStream.close();

		extractPreprocessorVars(computeContent);

		loaded = true;
	}

	void compileCompute()
	{
		GLchar* glCharCompute = computeContent;
		glCharCompute = preprocess(computeContent, computeSize);

		GLint computeShader = glCreateShader(GL_VERTEX_SHADER);

		glShaderSource(computeShader, 1, &glCharCompute, NULL);

		glCompileShader(computeShader);
		GLint isCompiled = GL_FALSE;
		glGetShaderiv(computeShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			return; //TODO: LOG ERROR
		}

		auto shaderProgram = glCreateProgram();

		glAttachShader(shaderProgram, computeShader);
		glLinkProgram(shaderProgram);
		glDeleteShader(computeShader);

		isCompiled = GL_FALSE;
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			//TODO: LOG ERROR

			GLint maxLength = 0;
			glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> errorLog(maxLength);
			glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &errorLog[0]);

			for (int i = 0; i < errorLog.size(); ++i)
			{
				std::cout << char(errorLog[i]);
			}

			glDeleteProgram(shaderProgram);

			return;
		}

		GLID = shaderProgram;


		extractUniforms(computeContent);
		extractBuffers(computeContent);

		delete[] computeContent;
		computeContent = glCharCompute;

		return;
	}

	void freeContent()
	{
		if (!loaded) return;

		switch (type)
		{
		case VertFrag:
			delete[] vertexContent;
			delete[] fragmentContent;
			vertexContent = nullptr;
			fragmentContent = nullptr;
			break;
		case Compute:
			delete[] computeContent;
			computeContent = nullptr;
			break;
		}
	}

	void extractUniforms(char* pSource)
	{
		//GET LINES WITH "uniform"

		struct Line
		{
			char* begin;
			char* end;
		};

		std::vector<Line> lines;

		Line curLine; curLine.begin = pSource;

		for (;;)
		{
			if (*pSource == '\n')
			{
				curLine.end = pSource;
				auto pos = StackString::contains(curLine.begin, curLine.end, String32("uniform "));
				if (pos != -1)
				{
					lines.push_back(curLine);
				}
				curLine.begin = pSource + 1;
			}
			++pSource;

			if (*pSource == '\0')
				break;
		}

		//TODO: GET LAYOUT INFO

		for (auto itr = lines.begin(); itr != lines.end(); ++itr)
		{
			std::regex e(" int | ivec2 | ivec3 | ivec4 | uint | uvec2 | uvec3 | uvec4 | float | vec2 | vec3 | vec4 | double | dvec2 | dvec3 | dvec4 | mat2 | mat3 | mat4 | dmat2 | dmat3 | dmat4 | sampler2D ");
			std::match_results<char*> cm;

			std::regex_search(itr->begin, itr->end, cm, e);

			if (cm.size() != 1)
				assert(false);

			String16 typeName;
			typeName.copyChars(typeName.getString(), cm[0].first + 1, (cm[0].second - 1) - (cm[0].first + 1));
			typeName.determineLength();

			char* nameBegin = cm[0].second;
			while (nameBegin[0] == ' ')
			{
				++nameBegin;
			}

			char* nameEnd = itr->end;
			char* arraySizeEnd = nameEnd;
			char* arraySizeBegin = nameEnd;
			while (*nameEnd == '\n' || *nameEnd == ' ' || *nameEnd == ';')
			{
				--nameEnd;
				if (*nameEnd == ']')
				{
					arraySizeEnd = nameEnd;
					arraySizeBegin = nameEnd;
					while (*arraySizeBegin != '[')
					{
						--arraySizeBegin;
						--nameEnd;
					}
					++arraySizeBegin;
				}
			}
			++nameEnd;

			String64 name; name.setToChars(nameBegin, nameEnd);
			GLint location = glGetUniformLocation(GLID, name.getString());
			uniformLocations.insert(std::make_pair(name, location));

			if (location != -1)
			{
				UniformMeta meta;
				meta.name.copyChars(meta.name.getString(), nameBegin, nameEnd - nameBegin);

				if (arraySizeBegin == itr->end)
				{
					meta.arrayCount = 1;
				}
				else
				{
					meta.arrayCount = StackString::strtos32(arraySizeBegin, arraySizeEnd);
				}

				meta.data.fmat4 = glm::dmat4(0);

				for (int i = 0; i < UniformTypesCount; ++i)
				{
					if (typeName == typeGLSLNames[i])
					{
						meta.type = UniformType(i);
					}
				}

				uniformObjects.insert(std::make_pair(location, meta));
			}
		}
	}

	void extractBuffers(char* pSource)
	{

	}

	void extractPreprocessorVars(char* pSource)
	{
		bool foundVar = false;

		for (;;)
		{
			if (*pSource == '@')
				foundVar = true;

			if (foundVar)
			{
				++pSource;
				//std::string varName; varName.reserve(20);
				//char varName[32];
				String32 varName;

				u32 varNameIndex = 0;
				for (;;)
				{
					varName.append(*pSource);

					++varNameIndex;

					if (varNameIndex > 31) { assert(0); }

					if (*++pSource == '@')
					{
						++pSource;
						break;
					}
				}

				//auto find = std::find(varNamesVals.begin(), varNamesVals.end(), varName);

				bool varExists = false;

				u32 i = 0;
				for (; i < 32; ++i)
				{
					//if (memcmp(varName, varVals[i].var, 32) == 0)
					if (varVals[i].var.getLength() == 0)
					{
						break;
					}

					if (varName == varVals[i].var)
					{
						varExists = true;
						break;
					}
				}

				if (!varExists)
				{
					varVals[i].var.overwrite(varName);
				}

				foundVar = false;
			}

			if (*++pSource == '\0')
				break;
		}
	}

	char* preprocess(char* pSource, u32 size)
	{
		char* processedShader = new char[size + (32 * 1024)];

		bool converting = false;

		int sourceIndex = 0;
		int outputIndex = 0;

		for (;;)
		{
			if (*pSource == '@')
				converting = true;

			if (converting)
			{
				++pSource;
				//std::string varName; varName.reserve(20);
				//char varName[32];
				String32 varName;

				u32 varNameIndex = 0;
				for (;;)
				{
					varName.append(*pSource);

					++varNameIndex;

					if (varNameIndex > 31) { assert(0); }

					if (*++pSource == '@')
					{
						++pSource;
						break;
					}
				}

				//auto find = std::find(varNamesVals.begin(), varNamesVals.end(), varName);

				String1024* findVal = nullptr;

				{
					for (u32 i = 0; i < 32; ++i)
					{
						//if (memcmp(varName, varVals[i].var, 32) == 0)
						if (varName == varVals[i].var)
						{
							findVal = &varVals[i].val;
							break;
						}
					}
				}

				if (findVal == nullptr)
					assert(0);

				StackString::copyChars(&processedShader[outputIndex], findVal->getString(), findVal->getLength());
				outputIndex += findVal->getLength();
				converting = false;
			}

			processedShader[outputIndex] = *pSource;

			++outputIndex;
			++sourceIndex;

			if (*++pSource == '\0')
				break;
		}

		processedShader[outputIndex] = '\0';
		return processedShader;
	}



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
		UniformType type;
		String64 name;
		UniformData data;
		GLsizei arrayCount; //1 == not array
	};

	std::map<String64, GLint> uniformLocations;
	std::map<GLint, UniformMeta> uniformObjects;

	union
	{
		struct
		{
			String128 vertexPath;
			String128 fragmentPath;
		};
		String128 computePath;
	};

	union
	{
		struct
		{
			char* vertexContent;
			char* fragmentContent;
		};
		char* computeContent;
	};

	union
	{
		struct
		{
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

	bool loaded;
	ShaderType type;
	GLint GLID;

};