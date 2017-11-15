#include "Camera.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"

void Camera::setFOV(float pFOV)
{
	fov = glm::radians(pFOV);
	recalculateProj();
	Engine::r->cameraProjUpdated();
}
