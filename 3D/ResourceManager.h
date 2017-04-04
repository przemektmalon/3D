#include "Include.h"
#include "Font.h"
#include "Mesh.h"


class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

	void init()
	{
		meshManager.newBatch();
	}

	u32 registerFont(const char* pPath, const char* pName);
	u32 register2DTex(const char* pPath, const char* pName);
	u32 register2DMipTex(const char* pPath, const char* pName);
	u32 registerCubeTex(const char* pPath, const char* pName);
	u32 registerMesh(const char* pPath, const char* pName)
	{
		meshNames.insert(std::make_pair(pName, ++lastMeshID));
		meshList.insert(std::make_pair(lastMeshID, MeshCPUMeta(pPath, pName)));
		return lastMeshID;
	}

	u32 loadFont(u32 pID);
	u32 load2DTex(u32 pID);
	u32 load2DMipTex(u32 pID);
	u32 loadCubeTex(u32 pID);
	u32 loadMesh(u32 pID)
	{
		MeshCPUMeta& meta = meshList.at(pID);
		meta.renderMeta = meshManager.loadMesh(meta.loadMeta.path);
		return pID;
	}

	Font* getFont(std::string pName);
	GLTexture2D* get2DTex(std::string pName);
	GLTexture2DMip get2DMipTex(std::string pName);
	GLTextureCube getCubeTex(std::string pName);
	MeshCPUMeta& getMesh(std::string pName)
	{
		return meshList.at(meshNames.at(pName));
	}

	Font* getFont(u32 pID);
	GLTexture2D* get2DTex(u32 pID);
	GLTexture2DMip get2DMipTex(u32 pID);
	GLTextureCube getCubeTex(u32 pID);
	MeshCPUMeta& getMesh(u32 pID)
	{
		return meshList.at(pID);
	}

	MeshManager meshManager;

private:



	std::map<std::string, u32> meshNames;
	std::map<u32, MeshCPUMeta> meshList;
	u32 lastMeshID = 0xa0000000;
};
