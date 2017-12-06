#include "Model.hpp"
#include "AssetManager.hpp"
#include "Engine.hpp"
#include <string>

#include "rapidxml.hpp"

void Model::importModel(std::string pPath, u32 lod)
{
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(pPath, 0);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}

	triLists.push_back(std::vector<TriangleList>());
	processNode(scene->mRootNode, scene, lod);
}

void Model::processNode(aiNode * node, const aiScene * scene, u32 lod)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, scene, triLists[lod]);
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, lod);
	}
}

void Model::processMesh(aiMesh * mesh, const aiScene * scene, std::vector<TriangleList>& triListVec)
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

	triListVec.push_back(TriangleList(vData, vDataSize, numVerts));
	auto& triList = triListVec.back();

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
		triList.matMeta.albedo.name = "null_D";
		triList.matMeta.normal.name = "null_N";
		triList.matMeta.specularMetallic.name = "null_S";

		triList.matMeta.albedo.glTex = Engine::assets.get2DTexGL(std::string("null_D"));
		triList.matMeta.normal.glTex = Engine::assets.get2DTexGL(std::string("null_N"));
		triList.matMeta.specularMetallic.glTex = Engine::assets.get2DTexGL(std::string("null_S"));
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
			triList.matMeta.albedo.name = toks2.front();
			//std::cout << "Model: \"" << name.getString() << "\" has diffuse texture: \"" << toks2.front() << "\"" << std::endl;

			triList.matMeta.albedo.glTex = Engine::assets.get2DTexGL(triList.matMeta.albedo.name);

			//auto texMeta = Engine::assets.textureManager.getTextureMeta(triList.matMeta.albedo[i].name);
			//triList.matMeta.albedo[i].glTex = texMeta.texArr;
			//triList.matMeta.albedo[i].gpuIndex = texMeta.index;

			break;
		}
		case(aiTextureType_HEIGHT):
		case(aiTextureType_NORMALS):
		{
			triList.matMeta.normal.name = toks2.front();
			//std::cout << "Model: \"" << name.getString() << "\" has normal texture: \"" << toks2.front() << "\"" << std::endl;

			triList.matMeta.normal.glTex = Engine::assets.get2DTexGL(triList.matMeta.normal.name);

			//auto texMeta = Engine::assets.textureManager.getTextureMeta(triList.matMeta.normal[i].name);
			//triList.matMeta.normal[i].glTex = texMeta.texArr;
			//triList.matMeta.normal[i].gpuIndex = texMeta.index;

			break;
		}
		case(aiTextureType_SPECULAR):
		case(aiTextureType_SHININESS):
		{
			triList.matMeta.specularMetallic.name = toks2.front();
			//std::cout << "Model: \"" << name.getString() << "\" has specular texture: \"" << toks2.front() << "\"" << std::endl;

			triList.matMeta.specularMetallic.glTex = Engine::assets.get2DTexGL(triList.matMeta.specularMetallic.name);

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
	physicsObject->create(sgNode->transform.getTranslation(), sgNode->transform.getQuat(), collisionShape, mass);
	physicsObject->instance = this;
	Engine::physicsWorld.addRigidBody(physicsObject);
}

void ModelInstance::makePhysicsObject()
{
	btCollisionShape* colShape = nullptr;
	float mass = 10.0, friction = 0.5, restitution = 0.2, linearDamping = 0.05, angularDamping = 0.05;

	auto nodeToFloat = [](rapidxml::xml_node<>* node) -> float {
		if (node)
			return std::stof(std::string(node->value()));
		else
			return 0.f; /// TODO: more appropriate default values and logging missing values
	};

	auto generateCollisionShape = [&nodeToFloat](rapidxml::xml_node<>* shapeNode) -> btCollisionShape* {
		std::string shapeType = shapeNode->first_attribute("type")->value();

		if (shapeType == "sphere")
		{
			float radius = nodeToFloat(shapeNode->first_node("radius"));
			return new btSphereShape(radius);
		}
		else if (shapeType == "box")
		{
			float halfx = nodeToFloat(shapeNode->first_node("halfx"));
			float halfy = nodeToFloat(shapeNode->first_node("halfy"));
			float halfz = nodeToFloat(shapeNode->first_node("halfz"));
			return new btBoxShape(btVector3(halfx, halfy, halfz));
		}
		else
		{
			/// TODO: more collision shapes
		}
	};

	if (model->physicsInfo.length())
	{
		File physicsInfoFile;
		physicsInfoFile.open(model->physicsInfo, File::in);
		
		std::string xmlString;

		physicsInfoFile.fstream().seekg(0, std::ios::end);
		xmlString.reserve(physicsInfoFile.fstream().tellg());
		physicsInfoFile.fstream().seekg(0, std::ios::beg);

		xmlString.assign((std::istreambuf_iterator<char>(physicsInfoFile.fstream())), std::istreambuf_iterator<char>());

		rapidxml::xml_document<> doc;
		doc.parse<0>(const_cast<char*>(xmlString.c_str()));

		rapidxml::xml_node<>* root = doc.first_node("physics");

		mass = nodeToFloat(root->first_node("mass"));
		friction = nodeToFloat(root->first_node("friction"));
		restitution = nodeToFloat(root->first_node("restitution"));
		linearDamping = nodeToFloat(root->first_node("lineardamping"));
		angularDamping = nodeToFloat(root->first_node("angulardamping"));

		std::string compound = std::string(root->first_node("compound")->value());
		if (compound == "true")
		{
			colShape = new btCompoundShape();
			btCompoundShape* compound = (btCompoundShape*)colShape;
			rapidxml::xml_node<>* shapeNode = root->first_node("shape");
			while (shapeNode)
			{
				auto shape = generateCollisionShape(shapeNode);

				btQuaternion rotation(0, 0, 0);
				btVector3 offset(0, 0, 0);

				if (shapeNode->first_node("posx"))
				{
					float posx, posy, posz;
					posx = nodeToFloat(shapeNode->first_node("posx"));
					posy = nodeToFloat(shapeNode->first_node("posy"));
					posz = nodeToFloat(shapeNode->first_node("posz"));
					offset = btVector3(posx, posy, posz);
				}

				if (shapeNode->first_node("rotx"))
				{
					/// TODO: get rotation values
				}

				btTransform transform(rotation, offset);

				compound->addChildShape(transform, shape);
				shapeNode = shapeNode->next_sibling();
			}
		}
		else
		{
			colShape = generateCollisionShape(root->first_node("shape"));
		}
	}
	else
	{
		colShape = new btBoxShape(btVector3(10, 10, 10));
	}

	physicsObject = new PhysicsObject();
	physicsObject->create(sgNode->transform.getTranslation(), sgNode->transform.getQuat(), colShape, mass);
	physicsObject->setDamping(linearDamping, angularDamping);
	physicsObject->setFriction(friction);
	physicsObject->setRestitution(restitution);
	physicsObject->instance = this;
	Engine::physicsWorld.addRigidBody(physicsObject);
}
