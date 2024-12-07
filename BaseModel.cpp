#include "BaseModel.h"

#include <iostream>
#include <string>

BaseModel::~BaseModel() 
{
	// uvolnit z pameti vsechny meshe
	for (std::vector<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); it++)
	{
		delete (*it);
	}
}

void BaseModel::setMeshes(std::vector<Mesh*> meshes)
{
	this->meshes = meshes;

	vertexCount = 0;
	faceCount = 0;

	// spocitat vrcholy a facy
	for (std::vector<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); it++)
	{
		vertexCount += (*it)->getVertices().size();
		faceCount += (*it)->getFaces().size();
	}
}

std::vector<Mesh*> &BaseModel::getMeshes()
{
	return meshes;
}
