#pragma once
#include "Engine.h"

float randRange(float min, float max)
{
	return min + float(Engine::rand()) / float(std::mt19937_64::max() / (max - min));
}

float randFloat()
{
	return float(Engine::rand()) / float(std::mt19937_64::max());
}