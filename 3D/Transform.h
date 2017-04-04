#pragma once
#include <iostream>
#include <GL\glew.h>
#include <GL\GLU.h>
#include <glm\glm.hpp>
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\matrix_transform_2d.hpp"
#include <fstream>

class Transform
{
public:
	Transform() : translation(glm::fvec3(0, 0, 0)), roll(0), pitch(0), yaw(0), origin(glm::fvec3(0.f, 0.f, 0.f)), scalem(glm::fvec3(1.f, 1.f, 1.f)), needUpdate(true) {}
	~Transform() {}

	inline glm::fmat4 getTransformMat()
	{
		if (needUpdate)
		{
			updateMatrix();
		}

		return transform;
	}

	inline glm::fmat4 getInverseTransformMat()
	{
		if (needUpdate)
		{
			updateMatrix();
		}

		return glm::inverse(transform);
	}

	inline Transform& combine(Transform& other)
	{
		translation += other.getTranslation();
		scalem *= other.getScale();
	}

	inline Transform& setTranslation(glm::fvec3 p) { translation = p; needUpdate = true; return *this; }
	inline Transform& translate(glm::fvec3 p) {
		translation += p;
		needUpdate = true;
		return *this;
	}
	inline glm::fvec3 getTranslation() { return translation; }

	inline Transform& setRoll(float p) { roll = p; needUpdate = true; return *this; }
	inline Transform& setYaw(float p) { yaw = p; needUpdate = true; return *this; }
	inline Transform& setPitch(float p) { pitch = p; needUpdate = true; return *this; }
	inline Transform& setRotate(glm::fvec3 p) { roll = p.x; pitch = p.y; yaw = p.z; needUpdate = true; return *this; }
	inline Transform& rotate(glm::fvec3 p) { roll += p.x; pitch += p.y; yaw += p.z; needUpdate = true; return *this; }
	inline Transform& rotateAround(float p, glm::fvec3 axis)
	{
		//rotation = glm::rotate(rotation, p, axis);

		//TODO: IMPLEMENT

		glm::fvec4 r(roll, pitch, yaw, 1.f);
		r += glm::rotate(rotation, p, axis) * r;
		roll = r.x; pitch = r.y; yaw = r.z;
		needUpdate = true; return *this;
	}
	inline float getRoll() { return roll; }
	inline float getYaw() { return yaw; }
	inline float getPitch() { return pitch; }
	inline glm::fmat4 getRotation() { return rotation; }

	inline Transform& setScale(glm::fvec3 p) { scalem = p; needUpdate = true; return *this; }
	inline Transform& scale(glm::fvec3 p) { scalem *= p; needUpdate = true; return *this; }
	inline Transform& scale(float p) { scalem = scalem * p; needUpdate = true; return *this; }
	inline glm::fvec3 getScale() { return scalem; }

	inline Transform& setOrigin(glm::fvec3 p) { origin = p; return *this; }
	inline glm::fvec3 getOrigin() { return origin; }

	inline void saveToStream(std::ofstream& ofs)
	{
		ofs.write((char*)&translation, sizeof(translation));
		ofs.write((char*)&origin, sizeof(origin));
		ofs.write((char*)&rotation, sizeof(rotation));
		ofs.write((char*)&scalem, sizeof(scalem));
	}

	inline void loadFromStream(std::ifstream& ifs)
	{
		needUpdate = true;

		ifs.read((char*)&translation, sizeof(translation));
		ifs.read((char*)&origin, sizeof(origin));
		ifs.read((char*)&rotation, sizeof(rotation));
		ifs.read((char*)&scalem, sizeof(scalem));
	}

private:

	inline void updateMatrix()
	{
		needUpdate = false;

		glm::fmat4 ttranslate = glm::translate(glm::fmat4(), translation - origin);
		glm::fmat4 sscale = glm::scale(glm::fmat4(), glm::fvec3(scalem.x, scalem.y, scalem.z));
		glm::fmat4 matRoll = glm::rotate(glm::fmat4(), roll, glm::fvec3(0.0f, 0.0f, 1.0f));
		glm::fmat4 matPitch = glm::rotate(glm::fmat4(), pitch, glm::fvec3(1.0f, 0.0f, 0.0f));
		glm::fmat4 matYaw = glm::rotate(glm::fmat4(), yaw, glm::fvec3(0.0f, 1.0f, 0.0f));
		glm::fmat4 rrotate = matRoll * matPitch * matYaw;
		glm::fmat4 tttranslate = glm::translate(glm::fmat4(), origin);

		transform = ttranslate * rrotate * sscale * tttranslate;
	}

	glm::fvec3 translation;
	glm::fvec3 origin;
	glm::fmat4 rotation;
	float pitch, yaw, roll;
	glm::fvec3 scalem;

	bool needUpdate;
	glm::fmat4 transform;
};

//class Transformable
//{
//public:
//	Transformable() {}
//	~Transformable() {}
//
//	virtual inline void setPosition(glm::fvec3 pPos) { transform.setTranslation(pPos); }
//	inline void move(glm::fvec3 pMove) { transform.translate(pMove); }
//	inline glm::fvec3 getPosition() { return transform.getTranslation(); }
//
//	virtual inline void setRotation(float pRot) { transform.setRotation(pRot); }
//	inline void rotate(float pRot) { transform.rotate(pRot); }
//	inline float getRotation() { return transform.getRotation(); }
//
//	inline void setScale(glm::fvec3 pScale) { transform.setScale(pScale); }
//	inline void scale(glm::fvec3 pScale) { transform.scale(pScale); }
//	inline void scale(float pScale) { transform.scale(pScale); }
//	inline glm::fvec3 getScale() { return transform.getScale(); }
//
//	inline void setOrigin(glm::fvec3 p) { transform.setOrigin(p); }
//	inline glm::fvec3 getOrigin() { return transform.getOrigin(); }
//
//	inline glm::fmat3 getTransformMat() { return transform.getTransform(); }
//	inline glm::fmat4 getGLTransformMat() { return transform.getGLTransform(); }
//
//	inline glm::fmat3 getInverseTransformMat() { return transform.getInverseTransform(); }
//	inline glm::fmat4 getInverseGLTransformMat() { return transform.getInverseGLTransform(); }
//
//	inline Transform& getTransform() { return transform; }
//
//	inline void saveTransformToStream(std::ofstream& ofs) { transform.saveToStream(ofs); }
//	inline void loadTransformFromStream(std::ifstream& ifs) { transform.loadFromStream(ifs); }
//
//	inline void setTransfom(Transform p) { transform = p; }
//
//private:
//
//	Transform transform;
//
//};

class Transform2D
{
public:
	Transform2D() : translation(glm::fvec2(0, 0)), rotation(0), origin(glm::fvec2(0.f, 0.f)), scalem(glm::fvec2(1.f, 1.f)), needUpdate(true) {}
	~Transform2D() {}

	inline glm::fmat3 getTransform()
	{
		if (needUpdate)
		{
			updateMatrix();
		}

		return transform;
	}

	inline glm::fmat4 getGLTransform()
	{
		if (needUpdate)
		{
			updateMatrix();
		}

		return glTransform;
	}

	inline glm::fmat3 getInverseTransform()
	{
		if (needUpdate)
		{
			updateMatrix();
		}

		return glm::inverse(transform);
	}

	inline glm::fmat4 getInverseGLTransform()
	{
		if (needUpdate)
		{
			updateMatrix();
		}

		return glm::inverse(glTransform);
	}

	inline Transform2D& setTranslation(glm::fvec2 p) { translation = p; needUpdate = true; return *this; }
	inline Transform2D& translate(glm::fvec2 p) { translation += p; needUpdate = true; return *this; }
	inline glm::fvec2 getTranslation() { return translation; }

	inline Transform2D& setRotation(float p) { rotation = p; needUpdate = true; return *this; }
	inline Transform2D& rotate(float p) { rotation += p; needUpdate = true; return *this; }
	inline float getRotation() { return rotation; }

	inline Transform2D& setScale(glm::fvec2 p) { scalem = p; needUpdate = true; return *this; }
	inline Transform2D& scale(glm::fvec2 p) { scalem *= p; needUpdate = true; return *this; }
	inline Transform2D& scale(float p) { scalem = scalem * p; needUpdate = true; return *this; }
	inline glm::fvec2 getScale() { return scalem; }

	inline Transform2D& setOrigin(glm::fvec2 p) { origin = p; return *this; }
	inline glm::fvec2 getOrigin() { return origin; }

	inline void saveToStream(std::ofstream& ofs)
	{
		ofs.write((char*)&translation, sizeof(translation));
		ofs.write((char*)&origin, sizeof(origin));
		ofs.write((char*)&rotation, sizeof(rotation));
		ofs.write((char*)&scalem, sizeof(scalem));
	}

	inline void loadFromStream(std::ifstream& ifs)
	{
		needUpdate = true;

		ifs.read((char*)&translation, sizeof(translation));
		ifs.read((char*)&origin, sizeof(origin));
		ifs.read((char*)&rotation, sizeof(rotation));
		ifs.read((char*)&scalem, sizeof(scalem));
	}

private:

	inline void updateMatrix()
	{
		needUpdate = false;

		glm::fmat3 ttranslate = glm::translate(glm::fmat3(), glm::fvec2(translation.x - origin.x, translation.y - origin.y));
		glm::fmat3 sscale = glm::scale(glm::fmat3(), glm::fvec2(scalem.x, scalem.y));
		glm::fmat3 rrotate = glm::rotate(glm::fmat3(), rotation);
		glm::fmat3 tttranslate = glm::translate(glm::fmat3(), glm::fvec2(origin.x, origin.y));

		transform = ttranslate * rrotate * sscale * tttranslate;

		glTransform[0].x = transform[0].x;
		glTransform[0].y = transform[0].y;
		glTransform[1].x = transform[1].x;
		glTransform[1].y = transform[1].y;

		glTransform[3].x = transform[2].x;
		glTransform[3].y = transform[2].y;
	}

	glm::fvec2 translation;
	glm::fvec2 origin;
	float rotation;
	glm::fvec2 scalem;

	bool needUpdate;
	glm::fmat3 transform;
	glm::fmat4 glTransform;
};