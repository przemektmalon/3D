#pragma once
#include "Include.h"
#include "Transform.h"
#include "Model.h"

class SGNode
{
public:


	virtual void update(glm::fmat4& transMat)
	{
		transform *= transMat;
		for (auto itr = children.begin(); itr != children.end(); ++itr)
		{
			itr->update(transform);
		}
	}

	s32 id;
	glm::fmat4 transform;
	std::vector<SGNode> children;
};

class SGTransformNode : public SGNode
{
public:

	void update(glm::fmat4& transMat)
	{
		SGNode::update(transform * nodeTransform.getTransformMat());
	}

	Transform nodeTransform;

};

class SGGeometryNode : public SGNode
{
public:

	void update(glm::fmat4& transMat)
	{
		//model.transform.
		SGNode::update(model.transform.getTransformMat());
	}

	Model& model;
};

class SceneGraph
{
public:
	SceneGraph() {}
	~SceneGraph() {}


};