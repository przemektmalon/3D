#pragma once
#include "Include.h"
#include "Transform.h"

class SGNode
{
public:

	SGNode* addChild(SGNode child)
	{
		children.push_back(child);
		auto itr = children.begin();
		std::advance(itr, children.size()-1);
		itr->parent = this;
		return &(*itr);
	}

	void updateLocal()
	{
		
	}

	void updateAll()
	{
		//transform.setOrigin(glm::fvec3(parent->transform.getTransformMat()[3]));
		transform.updateMatrix();
		transform.setTransformMat(parent->transform.getTransformMat() * transform.getTransformMat());

		fullTransform = transform;
		fullTransform *= parent->fullTransform;
		
		for (auto itr = children.begin(); itr != children.end(); ++itr)
		{
			itr->updateAll();
		}
	}

	Transform transform;
	Transform fullTransform;
	SGNode* parent;
	std::list<SGNode> children;
};

class SceneGraph
{
public:
	SceneGraph() {}
	~SceneGraph() {}

	SGNode rootNode;

	void updateAll()
	{
		for (auto itr = rootNode.children.begin(); itr != rootNode.children.end(); ++itr)
		{
			//itr->transform.setOrigin(glm::fvec3(parent->transform.getTransformMat()[3]));
			//itr->transform.updateMatrix();
			//itr->transform = transform * itr->transform;
			itr->updateAll();
		}
	}

};