#include "Plane.h"
#include "Object3D.h"

#include <Core/Engine.h>
#include <Component\SimpleScene.h>
#include <Component\SimpleScene.cpp>

#define PRIMARY_COLOR glm::vec3(1.0f, 0.55f, 0)
#define SECONDARY_COLOR glm::vec3(0.956f, 0.427f, 0.262f)
#define WHITE glm::vec3(0, 0, 0)

std::vector<Mesh*> Plane::GetMeshes() {
	return {
		Object3D::CreateCuboid("directionalTail", glm::vec3(-0.625f, 2.05f, 1.925f), glm::vec3(0.05f, 0.075f, 0.005f), PRIMARY_COLOR),
		Object3D::CreateCuboid("tail", glm::vec3(-0.6f, 2.025f, 1.975f), glm::vec3(0.1f, 0.075f, 0.1f), PRIMARY_COLOR),
		Object3D::CreateCuboid("box", glm::vec3(-0.5f, 2, 2), glm::vec3(0.05f, 0.15f, 0.15f), PRIMARY_COLOR),
		Object3D::CreateCuboid("box1", glm::vec3(-0.45f, 2, 2), glm::vec3(0.025f, 0.15f, 0.15f), WHITE),
		Object3D::CreateCuboid("wheel", glm::vec3(-0.45f, 1.98f, 2), glm::vec3(0.025f, 0.02f, 0.1f), glm::vec3(0, 0, 0)),
		Object3D::CreateCuboid("proppelerSupport", glm::vec3(-0.425f, 2.05f, 1.95f), glm::vec3(0.02f, 0.05f, 0.05f), PRIMARY_COLOR),
		Object3D::CreateCuboid("propeller1", glm::vec3(-0.405f, 1.95f, 1.95f), glm::vec3(0.01f, 0.25f, 0.05f), SECONDARY_COLOR),
		Object3D::CreateCuboid("propeller2", glm::vec3(-0.410f, 2.05f, 2.05f), glm::vec3(0.01f, 0.05f, 0.25f), SECONDARY_COLOR)
	};
}
