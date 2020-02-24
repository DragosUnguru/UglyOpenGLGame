#pragma once
#include <Component/SimpleScene.h>
#include "Camera.h"
#include "Plane.h"
#include "Cloud.h"
#include "Object3D.h"
#include "PlatonicSolid.h"

#define WORLD_SPEED 0.02f

class Tema : public SimpleScene
{
	public:
		Tema();
		~Tema();

		void Init() override;
		Mesh* CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned short>& indices);
		void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color);

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void EndlessRun();
		void RenderScene(float deltaTimeSeconds);

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

		void GameOver();

	protected:
		// Cubes constants
		const int CUBES_IN_CLOUD = 5;
		const int CLOUDS_RENDERED = 64;

		// Fuel points constants
		const int FUEL_FORMS_NO = 5;
		const int FUEL_RENDERED = 3;

		// Obstacle constants
		const int OBSTACLES_RENDERED = 8;

		// Camera
		Camera::Camera* camera;
		glm::mat4 projectionMatrix;
		bool renderCameraTarget;

		// Objects
		Plane plane;
		std::vector<PlatonicSolid*> fuelPoints;
		std::vector<PlatonicSolid*> obstacles;
		std::vector<Cloud*> clouds;

		// LightSource
		glm::vec3 lightPosition;
		int materialShininess = 30;
		float materialKd = 0.5f;
		float materialKs = 0.5f;

		// Ocean
		float oceanAngular = 0;

		// Game logic
		const float fuelEfficiency = 0.07f;
		short lives = 3;
		float fuel = 1;
		glm::vec3 fuelBarColor;
		bool gameLost = false;


		// Cursor and camera coordination
		float braveTry;
		float altceva = 0.5f;
		float x, y, xx, yy;
		float FOV = 60;
};
