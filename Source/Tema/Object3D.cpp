#include "Object3D.h"

#include <Core/Engine.h>
#include <iostream>

#define SECONDARY_COLOR glm::vec3(0.956f, 0.427f, 0.262f)

std::vector<glm::vec3> Object3D::ComputeTriangleNormals(std::vector<VertexFormat>& vertices, std::vector<unsigned short>& indices) {
	std::vector<glm::vec3> trianglesNormals;

	//This assumes that A->B->C is a counter-clockwise ordering
	for (int i = 0; i < indices.size(); i += 3) {
		glm::vec3 A = vertices[indices[i]].position;
		glm::vec3 B = vertices[indices[i + 1]].position;
		glm::vec3 C = vertices[indices[i + 2]].position;

		glm::vec3 N = glm::cross(B - A, C - A);
		float sin_alpha = glm::length(N) / (glm::length(B - A) * glm::length(C - A));

		trianglesNormals.push_back(glm::normalize(N) * asin(sin_alpha));
	}

	return trianglesNormals;
}

void Object3D::ComputeVerticesNormals(std::vector<VertexFormat>& vertices, std::vector<unsigned short>& indices) {
	std::vector<glm::vec3> trianglesNormals = ComputeTriangleNormals(vertices, indices);
	std::vector<glm::vec3> verticesNormals(vertices.size(), glm::vec3(0));

	for (int i = 0; i < indices.size(); ++i) {
		verticesNormals[indices[i]] += trianglesNormals[i / 3];
	}

	for (int i = 0; i < verticesNormals.size(); ++i) {
		verticesNormals[i] = glm::normalize(verticesNormals[i]);

		vertices[i].normal = verticesNormals[i];
	}
}

Mesh* Object3D::CreateCuboid(std::string name, glm::vec3 leftBottomCorner, glm::vec3 lengths) {
	float lengthOx = lengths.x;
	float lengthOy = lengths.y;
	float lengthOz = lengths.z;

	std::vector<VertexFormat> vertices = {
		VertexFormat(leftBottomCorner),
		VertexFormat(leftBottomCorner + glm::vec3(lengthOx, 0, 0)),
		VertexFormat(leftBottomCorner + glm::vec3(lengthOx, lengthOy, 0)),
		VertexFormat(leftBottomCorner + glm::vec3(0, lengthOy, 0)),
		VertexFormat(leftBottomCorner + glm::vec3(0, lengthOy, -lengthOz)),
		VertexFormat(leftBottomCorner + glm::vec3(lengthOx, lengthOy, -lengthOz)),
		VertexFormat(leftBottomCorner + glm::vec3(0, 0, -lengthOz)),
		VertexFormat(leftBottomCorner + glm::vec3(lengthOx, 0, -lengthOz))
	};

	std::vector<unsigned short> indices = {
		// Front face
		0, 2, 3,
		0, 1, 2,
		// Left-sided face
		0, 3, 6,
		3, 4, 6,
		// Bottom face
		1, 0, 7,
		0, 6, 7,
		// Right-sided face
		7, 5, 2,
		7, 2, 1,
		// Upper face
		2, 5, 4,
		2, 4, 3,
		// Back face
		6, 5, 7,
		6, 4, 5
	};

	ComputeVerticesNormals(vertices, indices);

	Mesh* cube = new Mesh(name);
	cube->SetDrawMode(GL_TRIANGLES);
	cube->InitFromData(vertices, indices);
	return cube;
}

Mesh* Object3D::CreateSquare(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill) {
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(length, 0, 0), color),
		VertexFormat(corner + glm::vec3(length, length, 0), color),
		VertexFormat(corner + glm::vec3(0, length, 0), color)
	};

	std::vector<unsigned short> indices = { 0, 1, 2, 3 };
	Mesh* square = new Mesh(name);

	if (!fill) {
		square->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		// draw 2 triangles. Add the remaining 2 indices
		indices.push_back(0);
		indices.push_back(2);
	}

	square->InitFromData(vertices, indices);
	return square;
}
