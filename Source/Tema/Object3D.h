#pragma once

#include <string>
#include <include/glm.h>
#include <Core/GPU/Mesh.h>


namespace Object3D
{
	std::vector<glm::vec3> ComputeTriangleNormals(std::vector<VertexFormat>& vertices, std::vector<unsigned short>& indices);

	void ComputeVerticesNormals(std::vector<VertexFormat>& vertices, std::vector<unsigned short>& indices);

	Mesh* CreateCuboid(std::string name, glm::vec3 leftBottomCorner, glm::vec3 lengths);

	Mesh* CreateSquare(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill);
}