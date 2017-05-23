#pragma once
#include "Strings.h"
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
		String<128> path;
		String<128> status;
		Mode fileMode;
	} meta;

	bool create(StringGeneric& pPath, s32 pFileMode)
	{
		return create(pPath, (Mode)pFileMode);
	}

	bool create(StringGeneric& pPath, Mode pFileMode = (File::Mode)(File::binary | File::in | File::out))
	{
		if (pPath.getLength() > 127)
			return false;
		meta.path.overwrite(pPath);
		meta.fileMode = pFileMode;

		return create(meta.fileMode);
	}

	bool create(Mode pFileMode = (File::Mode)(File::binary | File::in | File::out))
	{
		if (meta.path.getLength() == 0)
			return false;

		meta.fileMode = pFileMode;

		file.open(meta.path.getString(), meta.fileMode);
	}

	bool open(StringGeneric& pPath, s32 pFileMode)
	{
		return open(pPath, (Mode)pFileMode);
	}

	bool open(StringGeneric& pPath, Mode pFileMode = (File::Mode)(File::binary | File::in | File::out))
	{
		if (pPath.getLength() > 127)
			return false;
		meta.path.overwrite(pPath);
		meta.fileMode = pFileMode;

		return open(meta.fileMode);
	}

	bool open(Mode pFileMode = (File::Mode)(File::binary | File::in | File::out))
	{
		if (meta.path.getLength() == 0)
			return false;

		meta.fileMode = pFileMode;

		file.open(meta.path.getString(), meta.fileMode);
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

	void writeStr(StringGeneric& string, bool includeTerminator = true, bool writeCapacity = true)
	{
		if(!writeCapacity)
			file.write(string.getString(), string.getLength() + (includeTerminator ? 1 : 0));
		else
			file.write(string.getString(), string.getCapacity() + (includeTerminator ? 1 : 0));
	}

	void writeStr(HeapGeneric& string, bool includeTerminator = true)
	{
		file.write(string.getString(), string.getLength() + (includeTerminator ? 1 : 0));
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

	void read(StringGeneric& string, u32 length)
	{
		file.read(string.getString(), length);
	}

	void readStr(StringGeneric& string, bool readCapacity = true, bool readTerminator = true)
	{
		if (readCapacity)
			file.read(string.getString(), string.getCapacity() + (readTerminator ? 1 : 0));
		else
		{
			char lastChar;
			file.read(&lastChar, 1);
			while (lastChar != '/0')
			{
				string.append(lastChar);
				file.read(&lastChar, 1);
			}
		}
		string.determineLength();
	}

	void readStr(HeapGeneric& string, int size = -1)
	{
		if (size == -1)
		{
			char lastChar;
			file.read(&lastChar, 1);
			while (lastChar != '/0')
			{
				string.append(lastChar);
				file.read(&lastChar, 1);
			}
		}
		else
		{
			string.expand(size);
			file.read(string.getString(), size);
		}
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

	void setPath(String<128>& pPath)
	{
		meta.path.overwrite(pPath);
	}

};
