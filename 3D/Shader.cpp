#include "Shader.h"

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

const String16 ShaderProgram::typeGLSLNames[ShaderProgram::UniformTypesCount] = {
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

ShaderProgram::ShaderProgram() : vertexPath(), fragmentPath() {}
ShaderProgram::ShaderProgram(const ShaderProgram& program) : vertexPath(), fragmentPath() {}

ShaderProgram::~ShaderProgram() {}

void ShaderProgram::destroy()
{
	glDeleteProgram(GLID);
	uniformObjects.clear();
	freeSourceContent();
}

void ShaderProgram::load(String32 && pName, ShaderType pType, String128 & pShaderLocationPath)
{
	//uniformLocations.insert(std::make_pair(pName, 0));
	name.overwrite(pName);
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

void ShaderProgram::load(String32 & pName, ShaderType pType, String128 & pShaderLocationPath)
{
	//uniformLocations.insert(std::make_pair(pName, 0));
	name.overwrite(pName);
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

void ShaderProgram::compile()
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

void ShaderProgram::setUniform(String64 & pUniformName, const void * pUniformData, s32 pFlags)
{
	auto uniformObject = uniformObjects.find(pUniformName);
	if (uniformObject == uniformObjects.end())
		return;

	UniformMeta& meta = uniformObject->second;
	auto size = typeSizes[meta.type];
	auto count = meta.arrayCount;

	meta.flags = pFlags;

	if (count == 1)
		memcpy(&meta.data, pUniformData, size);
	else
		memcpy(meta.data.array, pUniformData, size * count);
}

void ShaderProgram::setVarVal(String32 & var, String1024 & val)
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

void ShaderProgram::loadVertFrag(String32 & pName, String128 & pShaderLocationPath)
{
	String128 shaderPath; shaderPath.overwrite(pShaderLocationPath);

	vertexPath.overwrite(shaderPath);
	vertexPath.append(pName);
	vertexPath.append(String8(".vert"));

	fragmentPath.overwrite(vertexPath);
	fragmentPath.shrinkBy(5);
	fragmentPath.append(String8(".frag"));

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

void ShaderProgram::loadCompute(String32 & pName, String128 & pShaderLocationPath)
{
	computePath.overwrite(pShaderLocationPath.append(pName).append(String32(".comp")));

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

void ShaderProgram::compileCompute()
{
	GLchar* glCharCompute = computeContent;
	//glCharCompute = preprocess(computeContent, computeSize);

	GLint computeShader = glCreateShader(GL_COMPUTE_SHADER);

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

	use();
	extractUniforms(computeContent);
	extractBuffers(computeContent);

	//delete[] computeContent;
	//computeContent = glCharCompute;

	return;
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
			auto pos = StringGeneric::contains(curLine.begin, curLine.end, String32("uniform "));
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

		String64 name; name.setToChars(nameBegin, nameEnd);
		GLint location = glGetUniformLocation(GLID, name.getString());
		//uniformLocations.insert(std::make_pair(name, location));

		UniformMeta newMeta;
		auto ins = uniformObjects.insert(std::make_pair(name, newMeta));
		auto& meta = ins.first->second;

		if (location != -1)
		{
			meta.name.copyChars(meta.name.getString(), nameBegin, nameEnd - nameBegin);

			if (arraySizeBegin == itr->end)
			{
				meta.arrayCount = 1;
			}
			else
			{
				meta.arrayCount = StringGeneric::strtos32(arraySizeBegin, arraySizeEnd);
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

			StringGeneric::copyChars(&processedShader[outputIndex], findVal->getString(), findVal->getLength());
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