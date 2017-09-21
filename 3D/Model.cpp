#include "Model.hpp"
#include "AssetManager.hpp"
#include "Engine.hpp"

void Model::importModel()
{
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(std::string(getPath().getString()), 0);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode * node, const aiScene * scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, scene);
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

void Model::processMesh(aiMesh * mesh, const aiScene * scene)
{
	u32		numVerts = mesh->mNumFaces * 3;
	u32		vDataSize = numVerts * 8;
	float*	vData = new float[vDataSize];

	u32 di = 0; //Data Index

	for (u32 i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace f = mesh->mFaces[i]; //Face

		for (u32 j = 0; j < f.mNumIndices; ++j)
		{
			u32 vi = f.mIndices[j]; //Vertex Index

			aiVector3D* pos = mesh->mVertices;
			aiVector3D* nor = mesh->mNormals;
			aiVector3D* uv = mesh->mTextureCoords[0];

			vData[di] = pos[vi].x; ++di;
			vData[di] = pos[vi].y; ++di;
			vData[di] = pos[vi].z; ++di;

			vData[di] = nor[vi].x; ++di;
			vData[di] = nor[vi].y; ++di;
			vData[di] = nor[vi].z; ++di;

			if (!uv)
			{
				vData[di] = 0.f; ++di;
				vData[di] = 0.f; ++di;
			}
			else
			{
				vData[di] = uv[vi].x; ++di;
				vData[di] = uv[vi].y; ++di;
			}
		}
	}

	triLists.push_back(TriangleList(vData, vDataSize, numVerts));
	auto& triList = triLists.back();

	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	bool splat = material->GetTextureCount(aiTextureType_DIFFUSE) > 1;

	auto normalType = material->GetTextureCount(aiTextureType_HEIGHT) > material->GetTextureCount(aiTextureType_NORMALS) ? aiTextureType_HEIGHT : aiTextureType_NORMALS;
	auto specularType = material->GetTextureCount(aiTextureType_SPECULAR) > material->GetTextureCount(aiTextureType_SHININESS) ? aiTextureType_SPECULAR : aiTextureType_SHININESS;

	loadMaterialTextures(material, aiTextureType_DIFFUSE, triList);
	loadMaterialTextures(material, normalType, triList);
	loadMaterialTextures(material, specularType, triList);

}

void Model::loadMaterialTextures(aiMaterial * material, aiTextureType type, TriangleList & triList)
{
	u32 count = material->GetTextureCount(type);

	if (count == 0)
	{
		triList.matMeta.albedo.name.setToChars("null_D");
		triList.matMeta.normal.name.setToChars("null_N");
		triList.matMeta.specular.name.setToChars("null_S");

		triList.matMeta.albedo.glTex = Engine::assets.get2DTexGL("null_D");
		triList.matMeta.normal.glTex = Engine::assets.get2DTexGL("null_N");
		triList.matMeta.specular.glTex = Engine::assets.get2DTexGL("null_S");
	}

	for (u32 i = 0; i < count; ++i)
	{
		std::string texName;
		aiString texPath;
		material->GetTexture(type, i, &texPath);

		texName = texPath.C_Str();

		boost::char_separator<char> sep("\\");
		boost::tokenizer<boost::char_separator<char>> tokens(texName, sep);

		std::vector<std::string> toks;

		for (auto& t : tokens)
			toks.push_back(t);

		boost::char_separator<char> sep2(".");
		boost::tokenizer<boost::char_separator<char>> tokens2(toks.back(), sep2);

		std::vector<std::string> toks2;

		for (auto& t : tokens2)
			toks2.push_back(t);

		switch (type)
		{
		case(aiTextureType_DIFFUSE):
		{
			triList.matMeta.albedo.name.setToChars(toks2.front().c_str());
			std::cout << "Model: \"" << name.getString() << "\" has diffuse texture: \"" << toks2.front() << "\"" << std::endl;

			triList.matMeta.albedo.glTex = Engine::assets.get2DTexGL(triList.matMeta.albedo.name);

			//auto texMeta = Engine::assets.textureManager.getTextureMeta(triList.matMeta.albedo[i].name);
			//triList.matMeta.albedo[i].glTex = texMeta.texArr;
			//triList.matMeta.albedo[i].gpuIndex = texMeta.index;

			break;
		}
		case(aiTextureType_HEIGHT):
		case(aiTextureType_NORMALS):
		{
			triList.matMeta.normal.name.setToChars(toks2.front().c_str());
			std::cout << "Model: \"" << name.getString() << "\" has normal texture: \"" << toks2.front() << "\"" << std::endl;

			triList.matMeta.normal.glTex = Engine::assets.get2DTexGL(triList.matMeta.normal.name);

			//auto texMeta = Engine::assets.textureManager.getTextureMeta(triList.matMeta.normal[i].name);
			//triList.matMeta.normal[i].glTex = texMeta.texArr;
			//triList.matMeta.normal[i].gpuIndex = texMeta.index;

			break;
		}
		case(aiTextureType_SPECULAR):
		case(aiTextureType_SHININESS):
		{
			triList.matMeta.specular.name.setToChars(toks2.front().c_str());
			std::cout << "Model: \"" << name.getString() << "\" has specular texture: \"" << toks2.front() << "\"" << std::endl;

			triList.matMeta.specular.glTex = Engine::assets.get2DTexGL(triList.matMeta.specular.name);

			//auto texMeta = Engine::assets.textureManager.getTextureMeta(triList.matMeta.specular[i].name);
			//triList.matMeta.specular[i].glTex = texMeta.texArr;
			//triList.matMeta.specular[i].gpuIndex = texMeta.index;

			break;
		}
		}

	}
}

void ModelInstance::makePhysicsObject(btCollisionShape * collisionShape, float mass)
{
	physicsObject = new PhysicsObject();
	physicsObject->create(sgNode->transform.getTranslation(), sgNode->transform.getQRotation(), collisionShape, mass);
	physicsObject->instance = this;
	Engine::physics.addRigidBody(physicsObject);
}
