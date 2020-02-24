#pragma once

#include <string>
#include <include/glm.h>
#include <iostream>
#include <Core/GPU/Mesh.h>

#include "Object3D.h"
#include "Tema.h"

#define PRIMARY_COLOR glm::vec3(1.0f, 0.55f, 0)
#define SECONDARY_COLOR glm::vec3(0.956f, 0.427f, 0.262f)
#define WHITE glm::vec3(1, 1, 1)

#define EPS 0.05f

class Plane {
	public:
		Plane() {
			propellerModelMatrix = glm::mat4(1);
			propellerAngular = 0.0f;
			planeOyTranslation = 0;
			planeAngular = 0;
			oxPlummetingPos = 0;
			plummetToDeath = false;
		}
		~Plane() {}

		std::vector<Mesh*> GetMeshes() {
			return {
				Object3D::CreateCuboid("directionalTail", glm::vec3(-0.625f, 2.05f, 1.925f), glm::vec3(0.05f, 0.075f, 0.005f)),
				Object3D::CreateCuboid("tail", glm::vec3(-0.6f, 2.025f, 1.975f), glm::vec3(0.1f, 0.075f, 0.1f)),
				Object3D::CreateCuboid("box", glm::vec3(-0.5f, 2, 2), glm::vec3(0.05f, 0.15f, 0.15f)),
				Object3D::CreateCuboid("box1", glm::vec3(-0.45f, 2, 2), glm::vec3(0.025f, 0.15f, 0.15f)),
				Object3D::CreateCuboid("wheel", glm::vec3(-0.45f, 1.98f, 2), glm::vec3(0.025f, 0.02f, 0.1f)),
				Object3D::CreateCuboid("proppelerSupport", glm::vec3(-0.425f, 2.05f, 1.95f), glm::vec3(0.02f, 0.05f, 0.05f)),
				Object3D::CreateCuboid("propeller1", glm::vec3(-0.405f, 1.95f, 1.95f), glm::vec3(0.01f, 0.25f, 0.05f)),
				Object3D::CreateCuboid("propeller2", glm::vec3(-0.410f, 2.05f, 2.05f), glm::vec3(0.01f, 0.05f, 0.25f)),
				Object3D::CreateCuboid("wing1", glm::vec3(-0.55f, 2.075f, 2.13f), glm::vec3(0.05f, 0.01f, 0.15f)),
				Object3D::CreateCuboid("wing2", glm::vec3(-0.55f, 2.075f, 1.87f), glm::vec3(0.05f, 0.01f, 0.15f))
			};
		}

		glm::mat4 RotatePropeller(float deltaTimeSeconds) {
			glm::mat4 modelMatrix(1);

			// Translate on Oy axis according to plane rotation
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0, planeOyTranslation + sin(planeAngular) / 20, 0));

			// Translate back to it's original position
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.4025f, 2.075f, 1.925f));

			// Rotate according to whole plane angular rotation
			modelMatrix = glm::rotate(modelMatrix, planeAngular, glm::vec3(0, 0, 1));

			// Spin propeller
			modelMatrix = glm::rotate(modelMatrix, propellerAngular, glm::vec3(1, 0, 0));

			// Translate with center in origin
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.4025f, -2.075f, -1.925f));

			// Manage next angle
			propellerAngular += 2.5f * deltaTimeSeconds;

			return modelMatrix;
		}

		glm::mat4 Fly(float cursorOyPos, float deltaTimeSeconds) {
			glm::mat4 modelMatrix(1);

			if (plummetToDeath) {
				planeAngular -= 1.5f * deltaTimeSeconds;
				planeOyTranslation -= 0.05f * deltaTimeSeconds;
				oxPlummetingPos += 0.025f * deltaTimeSeconds;

				// Translate on Oy axis
				modelMatrix = glm::translate(modelMatrix, glm::vec3(oxPlummetingPos, planeOyTranslation, 0));

				// Translate back into position
				modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.55f, 2.05f, 2));

				// Rotate while falling
				modelMatrix = glm::rotate(modelMatrix, planeAngular, glm::vec3(1, 0, 1));

				// Translate with TAIL center in origin
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.55f, -2.05f, -2));

				return modelMatrix;
			}

			// Translate on Oy axis according to mouse pointer
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0, planeOyTranslation, 0));

			// Translate back into position
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.55f, 2.05f, 2));
			
			// Rotate according to mouse pointer
			modelMatrix = glm::rotate(modelMatrix, planeAngular, glm::vec3(0, 0, 1));

			// Translate with TAIL center in origin
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.55f, -2.05f, -2));

			// Manage rotation angle and Oy translation according to mouse pointer
			if (cursorOyPos < planeOyTranslation) {
				planeOyTranslation -= pointerFollowingSpeed * deltaTimeSeconds;
			}
			else {
				planeOyTranslation += pointerFollowingSpeed * deltaTimeSeconds;
			}

			planeAngular = cursorOyPos - planeOyTranslation;

			return modelMatrix;
		}

		glm::mat4 propellerModelMatrix;
		float planeOyTranslation;
		float planeAngular;
		float propellerAngular;
		float oxPlummetingPos;
		bool plummetToDeath;

		const float pointerFollowingSpeed = 0.15f;
		const float pointerRotationSpeed = 0.5f;
};
