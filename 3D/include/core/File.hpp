#pragma once
#include <string>
#include <fstream>
#include <io.h>

class File
{
private:
	std::fstream file;

public:
	File() {}

	enum Mode
	{
		in = 0x01,
		out = 0x02,
		ate = 0x04,
		app = 0x08,
		trunc = 0x10,
		binary = 0x20
	};

	struct FileMeta
	{
		std::string path;
		std::string status;
		Mode fileMode;
	} meta;

	std::fstream& fstream() { return file; }

	bool create(std::string&& pPath, Mode pFileMode = (File::Mode)(File::binary | File::in | File::out))
	{
		if (pPath.length() > 127)
			return false;
		meta.path = pPath;
		meta.fileMode = pFileMode;

		return create(meta.fileMode);
	}

	bool create(Mode pFileMode = (File::Mode)(File::binary | File::in | File::out))
	{
		if (meta.path.length() == 0)
			return false;

		meta.fileMode = pFileMode;

		file.open(meta.path.c_str(), meta.fileMode);
	}

	bool open(std::string&& pPath, Mode pFileMode = (File::Mode)(File::binary | File::in | File::out))
	{
		return open(pPath, pFileMode);
	}

	bool open(std::string& pPath, Mode pFileMode = (File::Mode)(File::binary | File::in | File::out))
	{
		if (pPath.length() > 127)
			return false;
		meta.path = pPath;
		meta.fileMode = pFileMode;

		return open(meta.fileMode);
	}

	bool open(Mode pFileMode = (File::Mode)(File::binary | File::in | File::out))
	{
		if (meta.path.length() == 0)
			return false;

		meta.fileMode = pFileMode;

		file.open(meta.path.c_str(), meta.fileMode);

		if (file.bad())
		{
			std::cout << "S" << std::endl;
		}
	}

	bool atEOF()
	{
		return file.eof();
	}

	void close()
	{
		file.close();
	}

	/////////////////////////// WRITING ///////////////////////////

	void write(void* data, u32 size)
	{
		file.write((char*)data, size);
	}

	template<class T>
	void write(T val)
	{
		file.write((char*)&val, sizeof(T));
	}

	template<class T>
	void writeArray(T* val, u32 length = 1)
	{
		file.write((char*)val, sizeof(T)*length);
	}

	void writeStr(std::string& string, bool includeTerminator = true, bool writeCapacity = true)
	{
		if(!writeCapacity)
			file.write(string.c_str(), string.length() + (includeTerminator ? 1 : 0));
		else
			file.write(string.c_str(), string.size() + (includeTerminator ? 1 : 0));

	}

	void writeStr(std::string& string, bool includeTerminator = true)
	{
		file.write(string.c_str(), string.length() + (includeTerminator ? 1 : 0));
	}

	/////////////////////////// WRITING ///////////////////////////

	/////////////////////////// READING ///////////////////////////

	void read(void* data, u32 size)
	{
		file.read((char*)data, size);
	}

	template<class T>
	void read(T& val)
	{
		file.read((char*)&val, sizeof(T));
	}

	template<class T>
	void readArray(T* val, u32 length = 1)
	{
		file.read((char*)val, sizeof(T)*length);
	}

	void read(std::string& string, u32 length)
	{
		file.read((char*)string.c_str(), length);
	}

	void readStr(std::string& string, bool readCapacity = true, bool readTerminator = true)
	{
		if (readCapacity)
			file.read((char*)string.c_str(), string.size() + (readTerminator ? 1 : 0));
		else
		{
			char lastChar;
			file.read(&lastChar, 1);
			while (lastChar != '\0')
			{
				string.push_back(lastChar);
				file.read(&lastChar, 1);
			}
		}
	}

	void readStr(std::string& string, char delim = '\n', int size = -1)
	{
		if (size == -1)
		{
			char lastChar[2];
			lastChar[1] = 0;
			file.read(lastChar, 1);
			while (*lastChar != delim && *lastChar != '\0' && !file.eof())
			{
				string.append(lastChar);
				file.read(lastChar, 1);
			}
		}
		else
		{
			string.resize(size);
			file.read(const_cast<char*>(string.c_str()), size);
		}
	}

	char peekChar()
	{
		return file.peek();
	}

	char pullChar()
	{
		char pull;
		file.read(&pull, 1);
		return pull;
	}

	/////////////////////////// READING ///////////////////////////

	bool checkWritable()
	{
		return (meta.fileMode & out) != 0;
	}

	bool checkReadable()
	{
		return (meta.fileMode & in) != 0;
	}

	s64 getSize()
	{
		
	}

	void setPath(std::string pPath)
	{
		meta.path = pPath;
	}

};
