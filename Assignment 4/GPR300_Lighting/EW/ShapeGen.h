//Author: Eric Winebrenner

#pragma once
#include "Mesh.h"

namespace ew {
	void createPlane(float width, float height, MeshData& meshData, glm::vec2 uv);
	void createQuad(float width, float height, MeshData& meshData, glm::vec2 uv);
	void createCube(float width, float height, float depth, MeshData& meshData, glm::vec2 uv);
	void createSphere(float radius, int numSegments, MeshData& meshData, glm::vec2 uv);
	void createCylinder(float height, float radius, int numSegments, MeshData& meshData, glm::vec2 uv);
}
