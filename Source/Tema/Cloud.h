#pragma once

#include <string>
#include <include/glm.h>
#include <Core/GPU/Mesh.h>
#include <iostream>
#include <random>

#include "Tema.h"
#include "Object3D.h"

#define PRIMARY_COLOR glm::vec3(1.0f, 0.55f, 0)
#define SECONDARY_COLOR glm::vec3(0.956f, 0.427f, 0.262f)
#define WHITE glm::vec3(1, 1, 1)

#define WIND_INTENSITY 1.5f
#define CHAOS_OFFSET (dis(gen))

class Cloud {
	public:
		Cloud() {}
		Cloud(int ozPos, float angular) {
			// Uniform distribution
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<> rotation(0, 120);
			std::uniform_real_distribution<> dis(0.1f, 0.2f);

			position = glm::vec3(SIDE_LENGTH, 2.25f, OZ_POS[ozPos % 6]);
			position += glm::vec3(CHAOS_OFFSET / 2, CHAOS_OFFSET, -CHAOS_OFFSET);

			chaosAngular = (float) rotation(gen);
			ozAngular = angular;
		}
		~Cloud() {}
		
		Mesh* Cloud::GetMesh() {
			float centerOffset = SIDE_LENGTH / 2.0f;
			return Object3D::CreateCuboid("cloud", glm::vec3(-centerOffset, -centerOffset, centerOffset), glm::vec3(SIDE_LENGTH));
		}

		glm::mat4 getModelMatrix(float deltaTimeSeconds) {
			// Uniform distribution
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<> axis(0.1f, 0.25f);
			glm::mat4 modelMatrix(1);

			// Around the world rotation
			modelMatrix = glm::rotate(modelMatrix, ozAngular, glm::vec3(0, 0, 1));

			// Manage positon in game
			modelMatrix = glm::translate(modelMatrix, position);

			// Rotate chaoticaly
			modelMatrix = glm::rotate(modelMatrix, chaosAngular, glm::vec3(1));

			chaosAngular += ROTATION_SPEED * deltaTimeSeconds;
			ozAngular = (ozAngular > 360) ? 0 : ozAngular + SPEED_FACTOR * deltaTimeSeconds;

			return modelMatrix;
		}

		
		const float SIDE_LENGTH = 0.05f;
		const float SPEED_FACTOR = 0.075f;
		const float ROTATION_SPEED = 75 * SPEED_FACTOR;
		const float OZ_POS[6] = { 1.25f, 1.75f, 2.0f, 2.25f, 2.5f, 2.75f };
		const float FAR_OZ = 1.0f;
		const float NEAR_OZ = 2.5f;

		glm::vec3 position;
		float ozAngular;
		float chaosAngular;
};
