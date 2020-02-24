#pragma once

#include <string>
#include <include/glm.h>
#include <Core/GPU/Mesh.h>
#include <iostream>
#include <random>

#include "Tema.h"
#include "Object3D.h"
#include <Core\Managers\ResourcePath.h>

#define PRIMARY_COLOR glm::vec3(1.0f, 0.55f, 0)
#define SECONDARY_COLOR glm::vec3(0.956f, 0.427f, 0.262f)
#define WHITE glm::vec3(1, 1, 1)

class PlatonicSolid {
public:

	// Fuel point constructor
	PlatonicSolid(float oxOffset) {
		// Uniform distribution
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> angular(0, 360);

		planeHit = false;
		speedFactor = 0.75f;
		chaosAngular = angular(gen);
		scalingFactor = glm::vec3(0.05f);
		position = glm::vec3(oxOffset, SIN_NEUTRAL_POS, 2);

		// For the "following-one-another" effect
		angleY = -oxOffset;
	}

	// Obstacle constructor
	PlatonicSolid() {
		// Uniform distribution
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> angular(0, 360);
		std::uniform_real_distribution<> oxOffset(2, 15);

		planeHit = false;
		speedFactor = 1;
		chaosAngular = angular(gen);
		scalingFactor = glm::vec3(0.075f);
		position = glm::vec3(oxOffset(gen), SIN_NEUTRAL_POS, 2);
		angleY = angular(gen);
	}

	~PlatonicSolid() {}

	glm::mat4 getModelMatrix(float deltaTimeSeconds) {
		glm::mat4 modelMatrix(1);

		// Move to position
		modelMatrix = glm::translate(modelMatrix, position);

		// Scale
		modelMatrix = glm::scale(modelMatrix, scalingFactor);

		// Continuously rotate
		modelMatrix = glm::rotate(modelMatrix, chaosAngular, glm::vec3(1));


		// ======= Update state values =======
		chaosAngular = (chaosAngular > 360) ? 0 : chaosAngular + ROTATION_SPEED * deltaTimeSeconds;

		// Move sinusoidaly on the oy axis
		position.y = SIN_NEUTRAL_POS + ATTENUATION * sin(angleY * 2 * speedFactor);
		angleY += deltaTimeSeconds * speedFactor;

		// Move towards the plane
		if (position.x < -OX_RESPAWN) {
			position.x = OX_RESPAWN;
			planeHit = false;
		}
		else {
			position.x -= speedFactor * deltaTimeSeconds;
		}

		return modelMatrix;
	}

	const float ROTATION_SPEED = 4.5f;
	const float ATTENUATION = 0.25f;
	const float SIN_NEUTRAL_POS = 2.5f;
	const float OX_RESPAWN = 3;

	glm::vec3 scalingFactor;
	glm::vec3 position;
	float chaosAngular;
	float speedFactor;
	float angleY;

	bool planeHit;
};
