#include "shaders/Shader.hpp"
#include "Engine.hpp"

const int ShaderProgram::typeSizes[ShaderProgram::UniformTypesCount] = {
	4, 8, 12, 16,
	4, 8, 12, 16,
	8, 16, 24, 32,
	8, 16, 24, 32,
	4, 8, 12, 16,
	8, 16, 24, 32,
	16, 36, 64,
	32, 72, 128,
	8
};

const char ShaderProgram::typeGLSLNames[ShaderProgram::UniformTypesCount][16] = {
	"int", "ivec2", "ivec3", "ivec4",
	"uint", "uvec2", "uvec3", "uvec4",
	"int64_t", "i64vec2", "i64vec3", "i64vec4",
	"uint64_t", "u64vec2", "u64vec3", "u64vec4",
	"float", "vec2", "vec3", "vec4",
	"double", "dvec2", "dvec3", "dvec4",
	"mat2", "mat3", "mat4",
	"dmat2", "dmat3", "dmat4",
	"sampler2D"
};

ShaderProgram::ShaderProgram() : geomPath(), vertexPath(), fragmentPath() {}
ShaderProgram::ShaderProgram(const ShaderProgram& program) : geomPath(), vertexPath(), fragmentPath() {}

ShaderProgram::~ShaderProgram() {}

void ShaderProgram::destroy()
{
	glDeleteProgram(GLID);
	uniformObjects.clear();
	freeSourceContent();
}

void ShaderProgram::load(std::string && pName, ShaderType pType, std::string & pShaderLocationPath)
{
	//uniformLocations.insert(std::make_pair(pName, 0));
	name = pName;
	type = pType;
	switch (type)
	{
	case VertFrag:
		loadVertFrag(pName, pShaderLocationPath);
		break;
	case VertGeomFrag:
		loadVertGeomFrag(pName, pShaderLocationPath);
		break;
	case Compute:
		loadCompute(pName, pShaderLocationPath);
		break;
	}
}

void ShaderProgram::load(std::string & pName, ShaderType pType, std::string & pShaderLocationPath)
{
	//uniformLocations.insert(std::make_pair(pName, 0));
	name = pName;
	type = pType;
	switch (type)
	{
	case VertFrag:
		loadVertFrag(pName, pShaderLocationPath);
		break;
	case VertGeomFrag:
		loadVertGeomFrag(pName, pShaderLocationPath);
		break;
	case Compute:
		loadCompute(pName, pShaderLocationPath);
		break;
	}
}

void ShaderProgram::compile()
{
	switch (type)
	{
	case VertFrag:
		compileVertFrag();
		break;
	case VertGeomFrag:
		compileVertGeomFrag();
		break;
	case Compute:
		compileCompute();
		break;
	}
}

void ShaderProgram::setUniform(std::string pUniformName, const void * pUniformData, s32 pFlags)
{
	auto uniformObject = uniformObjects.find(pUniformName);
	if (uniformObject == uniformObjects.end())
		return;

	UniformMeta& meta = uniformObject->second;
	auto size = typeSizes[meta.type];
	auto count = meta.arrayCount;

	meta.flags = pFlags;

	memcpy(&meta.data, pUniformData, size * count);
}

void ShaderProgram::setVarVal(std::string & var, std::string & val)
{
	for (u32 i = 0; i < 32; ++i)
	{
		if (varVals[i].var == var)
		{
			varVals[i].val = val;
			break;
		}

		if (varVals[i].var.c_str()[0] == '\0')
		{
			varVals[i].val = val;
			varVals[i].var = var;
			break;
		}
	}
}

void ShaderProgram::sendUniforms()
{
	glUseProgram(GLID);
	for (auto itr = uniformObjects.begin(); itr != uniformObjects.end(); ++itr)
	{
		UniformMeta& meta = itr->second;
		auto count = meta.arrayCount;
		auto location = meta.location;

		GLenum transposeMatrix = meta.flags == 1 ? GL_TRUE : GL_FALSE;

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
			glProgramUniformMatrix2fv(GLID, location, count, transposeMatrix, &meta.data.fmat2[0][0]);
			break;
		case fmat3:
			glProgramUniformMatrix3fv(GLID, location, count, transposeMatrix, &meta.data.fmat2[0][0]);
			break;
		case fmat4:
			glProgramUniformMatrix4fv(GLID, location, count, transposeMatrix, &meta.data.fmat2[0][0]);
			break;
		case dmat2:
			glProgramUniformMatrix2dv(GLID, location, count, transposeMatrix, &meta.data.dmat2[0][0]);
			break;
		case dmat3:
			glProgramUniformMatrix3dv(GLID, location, count, transposeMatrix, &meta.data.dmat2[0][0]);
			break;
		case dmat4:
			glProgramUniformMatrix4dv(GLID, location, count, transposeMatrix, &meta.data.dmat2[0][0]);
			break;
		case tex2d:
			glProgramUniform1i(GLID, location, meta.data.tex2d);
			break;
		}
	}
}

void ShaderProgram::loadVertFrag(std::string & pName, std::string & pShaderLocationPath)
{
	std::string shaderPath; shaderPath = pShaderLocationPath;

	vertexPath = shaderPath;
	vertexPath.append(pName);
	vertexPath.append(".vert");

	fragmentPath = vertexPath;
	fragmentPath.erase(fragmentPath.length() - 5, 5);
	fragmentPath.append(".frag");

	std::ifstream vertStream(vertexPath.c_str(), std::ifstream::in | std::ifstream::ate);
	std::ifstream fragStream(fragmentPath.c_str(), std::ifstream::in | std::ifstream::ate);

	if (vertStream.fail())
	{
		reportFailedShaderLoad("Vertex", (char*)vertexPath.c_str());
		return;
	}

	if (fragStream.fail())
	{
		reportFailedShaderLoad("Fragment", (char*)fragmentPath.c_str());
		return;
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

void ShaderProgram::compileVertFrag()
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
	if (!checkShaderCompilation(fragmentShader, "Fragment"))
	{
		glDeleteShader(fragmentShader);
		return;
	}

	glCompileShader(vertexShader);
	if (!checkShaderCompilation(vertexShader, "Vertex"))
	{
		glDeleteShader(fragmentShader);
		glDeleteShader(vertexShader);
		return;
	}

	auto shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	if (!checkProgramLinking(shaderProgram))
	{
		glDeleteProgram(shaderProgram);
		return;
	}
	
	GLID = shaderProgram;

	use();
	extractUniforms(vertexContent);
	extractUniforms(fragmentContent);
}

void ShaderProgram::loadVertGeomFrag(std::string & pName, std::string & pShaderLocationPath)
{
	loadVertFrag(pName, pShaderLocationPath);

	std::string shaderPath; shaderPath  = pShaderLocationPath;

	geomPath = shaderPath;
	geomPath.append(pName);
	geomPath.append(".geom");

	std::ifstream geomStream(geomPath.c_str(), std::ifstream::in | std::ifstream::ate);

	if (geomStream.fail())
	{
		reportFailedShaderLoad("Geometry", (char*)geomPath.c_str());
		return;
	}

	geomSize = 1 + geomStream.tellg();

	geomContent = new char[geomSize];

	geomContent[geomSize - 1] = '\0';

	geomStream.seekg(0);

	geomStream.read(geomContent, geomSize - 1);

	geomStream.close();

	extractPreprocessorVars(geomContent);
}

void ShaderProgram::compileVertGeomFrag()
{
	GLchar* glCharGeom = geomContent;
	GLchar* glCharVert = vertexContent;
	GLchar* glCharFrag = fragmentContent;

	glCharGeom = preprocess(geomContent, geomSize);
	glCharVert = preprocess(vertexContent, vertexSize);
	glCharFrag = preprocess(fragmentContent, fragmentSize);

	GLint geomShader = glCreateShader(GL_GEOMETRY_SHADER);
	GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(geomShader, 1, &glCharGeom, NULL);
	glShaderSource(vertexShader, 1, &glCharVert, NULL);
	glShaderSource(fragmentShader, 1, &glCharFrag, NULL);

	glCompileShader(fragmentShader);
	if (!checkShaderCompilation(fragmentShader, "Fragment"))
	{
		glDeleteShader(fragmentShader);
		return;
	}

	glCompileShader(vertexShader);
	if (!checkShaderCompilation(vertexShader, "Vertex"))
	{
		glDeleteShader(fragmentShader);
		glDeleteShader(vertexShader);
		return;
	}
	
	glCompileShader(geomShader);
	if (!checkShaderCompilation(vertexShader, "Geometry"))
	{
		glDeleteShader(fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(geomShader);
		return;
	}

	auto shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glAttachShader(shaderProgram, geomShader);

	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(geomShader);

	if (!checkProgramLinking(shaderProgram))
	{
		glDeleteProgram(shaderProgram);
		return;
	}

	GLID = shaderProgram;

	use();
	extractUniforms(vertexContent);
	extractUniforms(fragmentContent);
	extractUniforms(geomContent);
}

void ShaderProgram::loadCompute(std::string & pName, std::string & pShaderLocationPath)
{
	computePath = pShaderLocationPath.append(pName).append(std::string(".comp"));

	std::ifstream computeStream(computePath.c_str(), std::ifstream::in | std::ifstream::ate);

	if (computeStream.fail())
	{
		reportFailedShaderLoad("Compute", (char*)computePath.c_str());
		return;
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

void ShaderProgram::compileCompute()
{
	GLchar* glCharCompute = computeContent;

	GLint computeShader = glCreateShader(GL_COMPUTE_SHADER);

	glShaderSource(computeShader, 1, &glCharCompute, NULL);

	glCompileShader(computeShader);

	if (!checkShaderCompilation(computeShader, "Compute"))
	{
		glDeleteShader(computeShader);
		return;
	}

	auto shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, computeShader);
	glLinkProgram(shaderProgram);
	glDeleteShader(computeShader);

	if (!checkProgramLinking(shaderProgram))
	{
		glDeleteProgram(shaderProgram);
		return;
	}

	GLID = shaderProgram;

	use();
	extractUniforms(computeContent);
	extractBuffers(computeContent);

	//delete[] computeContent;
	//computeContent = glCharCompute;

	return;
}


void ShaderProgram::reportFailedShaderLoad(char* pType, char* pPath)
{
	std::string errorMessage;
	errorMessage.append(pType);
	errorMessage.append(" shader failed to load: ");
	errorMessage.append(name.c_str());
	errorMessage.append("\n");
	errorMessage.append("Expected path: ");
	errorMessage.append(pPath);

	Engine::engineLog.postTime();
	Engine::engineLog.appendMessage(errorMessage.c_str());
	Engine::engineLog.printLog(Engine::engineLog, "ShaderLoadFailed");

	MessageBox(NULL, errorMessage.c_str(), "Shader Loading Failed", MB_OK);
}

bool ShaderProgram::checkShaderCompilation(GLuint pShader, char* pType)
{
	GLint isCompiled = GL_FALSE;
	glGetShaderiv(pShader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(pShader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(pShader, maxLength, &maxLength, &errorLog[0]);

		std::string errorMessage;
		errorMessage.append(pType);
		errorMessage.append(" shader failed to compile: ");
		errorMessage.append(name.c_str());

		Engine::engineLog.postTime();
		Engine::engineLog.appendMessage(errorMessage.c_str());
		Engine::engineLog.appendMessage("\n\n");
		Engine::engineLog.appendMessage(errorLog.data());
		Engine::engineLog.appendMessage("\n\n");
		Engine::engineLog.printLog(Engine::engineLog, "ShaderCompileFailed");

		MessageBox(NULL, errorMessage.c_str(), "Shader Compilation Failed", MB_OK);

		return false;
	}
	return true;
}

bool ShaderProgram::checkProgramLinking(GLuint pProgram)
{
	GLint isCompiled = GL_FALSE;
	glGetProgramiv(pProgram, GL_LINK_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(pProgram, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(pProgram, maxLength, &maxLength, &errorLog[0]);

		std::string errorMessage;
		errorMessage.append("Program failed to link: ");
		errorMessage.append(name.c_str());

		Engine::engineLog.postTime();
		Engine::engineLog.appendMessage(errorMessage.c_str());
		Engine::engineLog.appendMessage("\n\n");
		Engine::engineLog.appendMessage(errorLog.data());
		Engine::engineLog.appendMessage("\n\n");
		Engine::engineLog.printLog(Engine::engineLog, "ProgramLinkingFailed");

		MessageBox(NULL, errorMessage.c_str(), "Program Linking Failed", MB_OK);

		return false;
	}
	return true;
}

void ShaderProgram::freeSourceContent()
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

void ShaderProgram::extractUniforms(char * pSource)
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
			std::string cl; 
			for (char* c = curLine.begin; c != curLine.end; ++c) {
				cl.push_back(*c);
			}
			auto pos = cl.find("uniform ");
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
		{ 
			//TODO: USER DEFINE TYPE (STRUCT)
			continue;
		}

		std::string typeName;
		typeName.assign(cm[0].first + 1, (cm[0].first + 1) + ((cm[0].second - 1) - (cm[0].first + 1)));

		char* nameBegin = cm[0].second;
		while (nameBegin[0] == ' ')
		{
			++nameBegin;
		}

		char* nameEnd = itr->end;
		char* arraySizeEnd = nameEnd;
		char* arraySizeBegin = nameEnd;
		while (*nameEnd == '\n' || *nameEnd == ' ' || *nameEnd == ';' || *nameEnd == '=')
		{
			--nameEnd;

			if (*nameEnd == ')')
			{
				while (*nameEnd != '(')
				{
					--nameEnd;
				}
				--nameEnd;
			}

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
				--nameEnd;
			}
		}
		++nameEnd;

		std::string name; name.assign(nameBegin, nameEnd);
		GLint location = glGetUniformLocation(GLID, name.c_str());
		//uniformLocations.insert(std::make_pair(name, location));

		UniformMeta newMeta;
		auto ins = uniformObjects.insert(std::make_pair(name, newMeta));
		auto& meta = ins.first->second;

		if (location != -1)
		{
			meta.name = name;

			if (arraySizeBegin == itr->end)
			{
				meta.arrayCount = 1;
			}
			else
			{
				meta.arrayCount = std::stoi(std::string(arraySizeBegin, arraySizeEnd));
			}

			meta.data.fmat4 = glm::dmat4(0);

			for (int i = 0; i < UniformTypesCount; ++i)
			{
				if (typeName == typeGLSLNames[i])
				{
					meta.type = UniformType(i);
				}
			}

			meta.location = location;
		}
	}
}

void ShaderProgram::extractBuffers(char * pSource)
{
}

void ShaderProgram::extractPreprocessorVars(char * pSource)
{
	bool foundVar = false;

	for (;;)
	{
		if (*pSource == '@')
			foundVar = true;

		if (foundVar)
		{
			++pSource;
			std::string varName; varName.reserve(20);

			u32 varNameIndex = 0;
			for (;;)
			{
				varName.push_back(*pSource);

				++varNameIndex;

				if (varNameIndex > 31) { assert(0); }

				if (*++pSource == '@')
				{
					++pSource;
					break;
				}
			}

			bool varExists = false;

			u32 i = 0;
			for (; i < 32; ++i)
			{
				if (varVals[i].var.length() == 0)
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
				varVals[i].var = varName;
			}

			foundVar = false;
		}

		if (*++pSource == '\0')
			break;
	}
}

char * ShaderProgram::preprocess(char * pSource, u32 size)
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
			std::string varName; varName.reserve(20);

			u32 varNameIndex = 0;
			for (;;)
			{
				varName.push_back(*pSource);

				++varNameIndex;

				if (varNameIndex > 31) { assert(0); }

				if (*++pSource == '@')
				{
					++pSource;
					break;
				}
			}

			std::string* findVal = nullptr;

			{
				for (u32 i = 0; i < 32; ++i)
				{
					if (varName == varVals[i].var)
					{
						findVal = &varVals[i].val;
						break;
					}
				}
			}

			if (findVal == nullptr)
				assert(0);

			memcpy(processedShader + outputIndex, findVal->c_str(), findVal->length());
			outputIndex += findVal->length();
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