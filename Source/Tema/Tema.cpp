#include "Tema.h"

#include <vector>
#include <string>
#include <iostream>
#include <ctime>
#include <random>

#include <Core/Engine.h>

using namespace std;

#define PRIMARY_COLOR glm::vec3(1.0f, 0.55f, 0)
#define SECONDARY_COLOR glm::vec3(0.956f, 0.427f, 0.262f)
#define WHITE glm::vec3(1, 1, 1)

Tema::Tema()
{
}

Tema::~Tema()
{
}

void Tema::Init()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> rotation(0, 60);

	// Construct clouds
	int ozPos = 0;
	float angular = rotation(gen);
	for (int i = 0; i < CLOUDS_RENDERED * CUBES_IN_CLOUD; ++i) {
		if (!(i % CUBES_IN_CLOUD)) {
			ozPos++;
			angular = rotation(gen);
		}
		clouds.push_back(new Cloud(ozPos, angular));
	}
	

	// Construct fuel points
	float oxPos = 0;
	for (int i = 0; i < FUEL_FORMS_NO * FUEL_RENDERED; ++i) {

		// Manage Ox offset for every object
		oxPos += 0.1f;
		if (!(i % FUEL_FORMS_NO)) {
			oxPos += 2;
		}
		fuelPoints.push_back(new PlatonicSolid(oxPos));
	}
	fuelBarColor = glm::vec3(0, 1, 0);

	// Construct obstacles
	for (int i = 0; i < OBSTACLES_RENDERED; ++i) {
		obstacles.push_back(new PlatonicSolid());
	}

	// Initialize camera
	renderCameraTarget = false;
	camera = new Camera::Camera(glm::vec3(0, 2.0f, 3.5f), glm::vec3(0, 2.5f, 0), glm::vec3(0, 1, 0));
	projectionMatrix = glm::perspective(RADIANS(FOV), window->props.aspectRatio, 0.01f, 200.0f);

	// Set cull face
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	// Load needed meshes
	{
		// Add meshes for plane
		for (auto mesh : plane.GetMeshes()) {
			CreateMesh(mesh->GetMeshID(), mesh->vertices, mesh->indices);
		}

		// Add mesh for cloud construction
		Mesh* cube = clouds[0]->GetMesh();
		CreateMesh(cube->GetMeshID(), cube->vertices, cube->indices);

		// Add mesh for icosahedron (obstacle)
		Mesh* icoMesh = new Mesh("obstacle");
		icoMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "icosahedron.obj");
		meshes[icoMesh->GetMeshID()] = icoMesh;

		// Add mesh for dodecahedron (fuel point)
		Mesh* dodeMesh = new Mesh("fuelPoint");
		dodeMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "dodecahedron.obj");
		meshes[dodeMesh->GetMeshID()] = dodeMesh;

		glDisable(GL_CULL_FACE);
		// Add mesh for irregular cylinder
		Mesh* seaMesh = new Mesh("ocean");
		seaMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "water3.obj");
		meshes[seaMesh->GetMeshID()] = seaMesh;

		// Add mesh for life points
		Mesh* lifeMesh = new Mesh("life");
		lifeMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "life.obj");
		meshes[lifeMesh->GetMeshID()] = lifeMesh;

		// Add mesh for fuel bar
		Mesh* square = Object3D::CreateSquare("fuel_rectangle", glm::vec3(0, 0, 0), 1.5f, fuelBarColor, true);
		meshes[square->GetMeshID()] = square;

		Mesh* rectangle = Object3D::CreateSquare("rectangle", glm::vec3(0, 0, 0), 1.5f, glm::vec3(1, 1, 1), true);
		meshes[rectangle->GetMeshID()] = rectangle;
	}

	// Light and material properties
	//lightPosition = glm::vec3(-1.0f, 1.0f, 1.85f);
	lightPosition = glm::vec3(-0.410f, 2.05f, 2.05f);
	materialShininess = 10;
	materialKd = 0.95f;
	materialKs = 0.95f;

	// Create a shader program for managing lighting
	{
		Shader* shader = new Shader("MyShader");
		shader->AddShader("Source/Tema/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Tema/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
}

void Tema::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0.529f, 0.807f, 0.921f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema::GameOver() {
	const GLfloat plane_centerx = -0.5f;
	const GLfloat plane_centery = 2.05f + plane.planeOyTranslation;
	GLfloat plane_rotate = plane.planeOyTranslation * 0.1f * RADIANS(1);
	GLfloat obstacle_radius_a = 0.1f;
	GLfloat obstacle_radius_b = 0.1f;
	GLfloat eps = 0.00001f;
	GLfloat a = 0.12f;
	GLfloat b = 0.15f;
	GLfloat e1 = plane_centerx;
	GLfloat f1 = plane_centery;

	for (GLfloat t = 0; t < 2 * M_PI; t += 0.3f) {
		GLfloat x1 = e1 + a * cos(t);
		GLfloat y1 = f1 + b * sin(t);

		GLfloat x = (x1 - e1) * cos(plane_rotate) - (y1 - f1) * sin(plane_rotate) + e1;
		GLfloat y = (y1 - f1) * cos(plane_rotate) + (x1 - e1) * sin(plane_rotate) + f1;

		for (auto& obstacle : obstacles) {
			if (obstacle->planeHit) {
				continue;
			}

			for (GLfloat u = 0; u < 2 * M_PI; u += 0.3f) {
				GLfloat px = obstacle->position.x + obstacle_radius_a * cos(u);
				GLfloat py = obstacle->position.y + obstacle_radius_b * sin(u);

				if (plane_centerx < obstacle->position.x) {
					if (obstacle->position.y > plane_centery) {
						if (px - x <= eps && py - y <= eps) {
							obstacle->planeHit = true;
							lives--;
							return;
						}
					}
					else {
						if (px - x <= eps && y - py <= eps) {
							obstacle->planeHit = true;
							lives--;
							return;
						}
					}
				}
				else {
					if (obstacle->position.y > plane_centery) {
						if (x - px <= eps && py - y <= eps) {
							obstacle->planeHit = true;
							lives--;
							return;
						}
					}
					else {
						if (x - px <= eps && y - py <= eps) {
							obstacle->planeHit = true;
							lives--;
							return;
						}
					}
				}
			}
		}

		for (auto& fuelPoint : fuelPoints) {
			if (fuelPoint->planeHit) {
				continue;
			}

			for (GLint i = 0; i < 5; i++) {
				GLfloat lb_x = fuelPoint->position.x - 0.05f;
				GLfloat lb_y = fuelPoint->position.y - 0.05f;
				GLfloat ru_x = fuelPoint->position.x + 0.05f;
				GLfloat ru_y = fuelPoint->position.y + 0.05f;

				if ((x >= lb_x && x <= ru_x) && (y >= lb_y && y <= ru_y)) {
					fuelPoint->planeHit = true;

					if (fuel <= 1 - fuelEfficiency) {
						fuel += fuelEfficiency;
						fuelBarColor.x -= fuelEfficiency;
						fuelBarColor.y += fuelEfficiency;
					}
				}

			}
		}
	}
	if (fuel < eps || lives <= 0) {
		plane.plummetToDeath = true;
		gameLost = true;
	}
}

void Tema::Update(float deltaTimeSeconds)
{
	GameOver();

	lightPosition.y += plane.planeOyTranslation;

	RenderScene(deltaTimeSeconds);

	if (fuel > 0) {
		fuel -= fuelEfficiency / 25.0f;
		fuelBarColor.x += fuelEfficiency / 25.0f;
		fuelBarColor.y -= fuelEfficiency / 25.0f;
	}
}

void Tema::FrameEnd() {
}

void Tema::RenderScene(float deltaTimeSeconds) {
	// Render plane
	RenderMesh(meshes["directionalTail"], shaders["MyShader"], plane.Fly(braveTry, deltaTimeSeconds), PRIMARY_COLOR);
	RenderMesh(meshes["tail"], shaders["MyShader"], plane.Fly(braveTry, deltaTimeSeconds), PRIMARY_COLOR);
	RenderMesh(meshes["box"], shaders["MyShader"], plane.Fly(braveTry, deltaTimeSeconds), PRIMARY_COLOR);
	RenderMesh(meshes["box1"], shaders["MyShader"], plane.Fly(braveTry, deltaTimeSeconds), WHITE);
	RenderMesh(meshes["wheel"], shaders["MyShader"], plane.Fly(braveTry, deltaTimeSeconds), glm::vec3(0, 0, 0));
	RenderMesh(meshes["proppelerSupport"], shaders["MyShader"], plane.Fly(braveTry, deltaTimeSeconds), SECONDARY_COLOR);
	RenderMesh(meshes["propeller1"], shaders["MyShader"], plane.RotatePropeller(deltaTimeSeconds), SECONDARY_COLOR);
	RenderMesh(meshes["propeller2"], shaders["MyShader"], plane.RotatePropeller(deltaTimeSeconds), SECONDARY_COLOR);
	RenderMesh(meshes["wing1"], shaders["MyShader"], plane.Fly(braveTry, deltaTimeSeconds), PRIMARY_COLOR);
	RenderMesh(meshes["wing2"], shaders["MyShader"], plane.Fly(braveTry, deltaTimeSeconds), PRIMARY_COLOR);

	// Render clouds
	for (int i = 0; i < CLOUDS_RENDERED * CUBES_IN_CLOUD; ++i) {
		RenderMesh(meshes["cloud"], shaders["MyShader"], clouds[i]->getModelMatrix(deltaTimeSeconds), SECONDARY_COLOR);
	}

	// Render fuel points
	for (int i = 0; i < FUEL_FORMS_NO * FUEL_RENDERED; ++i) {
		glm::mat4 modelMatrix = fuelPoints[i]->getModelMatrix(deltaTimeSeconds);
		if (!fuelPoints[i]->planeHit) {
			RenderMesh(meshes["fuelPoint"], shaders["MyShader"], modelMatrix, glm::vec3(0, 1, 1));
		}
	}

	// Render obstacle
	for (int i = 0; i < OBSTACLES_RENDERED; ++i) {
		glm::mat4 modelMatrix = obstacles[i]->getModelMatrix(deltaTimeSeconds);
		if (!obstacles[i]->planeHit) {
			RenderMesh(meshes["obstacle"], shaders["MyShader"], modelMatrix, glm::vec3(0.6f, 0, 0));
		}
	}

	// Render ocean
	glm::mat4 modelMatrix(1);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1.25f));
	modelMatrix = glm::rotate(modelMatrix, oceanAngular, glm::vec3(0, 0, 1));
	RenderMesh(meshes["ocean"], shaders["MyShader"], modelMatrix, glm::vec3(0.105f, 0.611f, 0.776f));
	
	oceanAngular += deltaTimeSeconds * 0.5f;

	// Render fuel bar
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(fuel, 0.25f, 0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(3.3f * (1.f / fuel), 17.75f, 13.5f));
	modelMatrix = glm::rotate(modelMatrix, RADIANS(180), glm::vec3(0, 0, 1));
	RenderMesh(meshes["fuel_rectangle"], shaders["VertexColor"], modelMatrix, fuelBarColor);

	modelMatrix = glm::mat4(1);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 0.25f, 0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(1.f, 6.f, 13.5f));
	modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(0, 1, 0));
	RenderMesh(meshes["rectangle"], shaders["MyShader"], modelMatrix, glm::vec3(1, 1, 1));

	// Render life points
	float oxOffset = -2.0f;
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(oxOffset, 3.25f, 1.5f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01f));

	oxOffset += 25.0f;
	for (int i = 0; i < lives; ++i) {
		modelMatrix = glm::translate(modelMatrix, glm::vec3(oxOffset, 0, 0));
		RenderMesh(meshes["life"], shaders["MyShader"], modelMatrix, glm::vec3(1, 0, 0));
	}
}

void Tema::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
	int ret;
	if (!mesh || !shader || !shader->program)
		return;

	// Render an object using the specified shader and the specified position
	shader->Use();
	
	// Set shader uniform "View" to camera view matrix and get shader location for uniform mat4 "View"
	ret = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(ret, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));

	// Set shader uniform "Projection" to projectionMatrix and get shader location for uniform mat4 "Projection"
	ret = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(ret, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Set shader uniform "Model" to model matrix and get shader location for uniform mat4 "Model"
	ret = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(ret, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Send light position
	int lightPos = glGetUniformLocation(shader->program, "lightPosition");
	glUniform3fv(lightPos, 1, glm::value_ptr(lightPosition));

	// Send eye position
	int eyePos = glGetUniformLocation(shader->program, "eyePosition");
	glUniform3fv(eyePos, 1, glm::value_ptr(camera->position));

	// Send material shininess
	int materialShine = glGetUniformLocation(shader->program, "materialShininess");
	glUniform1i(materialShine, materialShininess);

	// Send material Kd
	int kd = glGetUniformLocation(shader->program, "materialKd");
	glUniform1f(kd, materialKd);

	// Send material Ks
	int ks = glGetUniformLocation(shader->program, "materialKs");
	glUniform1f(ks, materialKs);

	// Send material color
	int col = glGetUniformLocation(shader->program, "objectColor");
	glUniform3fv(col, 1, glm::value_ptr(color));

	mesh->Render();
}

Mesh* Tema::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned short>& indices)
{
	unsigned int VAO = 0;
	// Create the VAO and bind it
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Create the VBO and bind it
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Send vertices data into the VBO buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// Crete the IBO and bind it
	unsigned int IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// Send indices data into the IBO buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// ========================================================================
	// This section describes how the GPU Shader Vertex Shader program receives data

	// set vertex position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

	// set vertex normal attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

	// set texture coordinate attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

	// set vertex color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
	// ========================================================================

	// Unbind the VAO
	glBindVertexArray(0);

	// Check for OpenGL errors
	CheckOpenGLError();

	// Mesh information is saved into a Mesh object
	meshes[name] = new Mesh(name);
	meshes[name]->InitFromBuffer(VAO, static_cast<unsigned short>(indices.size()));
	meshes[name]->vertices = vertices;
	meshes[name]->indices = indices;
	return meshes[name];
}

void Tema::OnInputUpdate(float deltaTime, int mods)
{
	// move the camera only if MOUSE_RIGHT button is pressed

	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float cameraSpeed = 2.0f;

		// Player specific movements
		if (window->KeyHold(GLFW_KEY_W)) {
			// Translate the camera forward
			camera->MoveForward(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_A)) {
			// Translate the camera to the left
			camera->MoveLeft(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_S)) {
			// Translate the camera backwards
			camera->MoveBackward(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_D)) {
			// Translate the camera to the right
			camera->MoveRight(cameraSpeed * deltaTime);
		}

		// Translates
		if (window->KeyHold(GLFW_KEY_I)) {
			// Translate the camera down
			camera->TranslateForward(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_O)) {
			// Translate the camera down
			camera->TranslateDownward(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_Q)) {
			// Translate the camera down
			camera->TranslateLeft(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_E)) {
			// Translate the camera down
			camera->TranslateRight(cameraSpeed * deltaTime);
		}
		
		if (window->KeyHold(GLFW_KEY_M)) {
			FOV *= 1.01f;
			projectionMatrix = glm::perspective(RADIANS(FOV), window->props.aspectRatio, 0.01f, 200.0f);
		}
		if (window->KeyHold(GLFW_KEY_N)) {
			FOV *= 0.99f;
			projectionMatrix = glm::perspective(RADIANS(FOV), window->props.aspectRatio, 0.01f, 200.0f);

		}
		if (window->KeyHold(GLFW_KEY_V)) {
			x *= 1.001f;
			y *= 1.001f;
			xx *= 1.001f;
			yy *= 1.001f;
			projectionMatrix = glm::ortho(x, y, xx, yy, 0.0f, 200.0f);

		}
		if (window->KeyHold(GLFW_KEY_B)) {
			x *= 0.999f;
			y *= 0.999f;
			xx *= 0.999f;
			yy *= 0.999f;
			projectionMatrix = glm::ortho(x, y, xx, yy, 0.0f, 200.0f);

		}
	}
}

void Tema::OnKeyPress(int key, int mods)
{
	// add key press event
	if (key == GLFW_KEY_T)
	{
		renderCameraTarget = !renderCameraTarget;
	}
	// Orthographic 
	if (key == GLFW_KEY_R) {
		projectionMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 200.0f);
	}
	if (key == GLFW_KEY_Y) {
		projectionMatrix = glm::perspective(RADIANS(FOV), window->props.aspectRatio, 0.01f, 200.0f);
	}
}

void Tema::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Tema::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
	this->braveTry = (-mouseY / 420.0f) + 1;
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float sensivityOX = 0.001f;
		float sensivityOY = 0.001f;

		if (window->GetSpecialKeyState() == 0) {
			renderCameraTarget = false;
			camera->RotateFirstPerson_OY(-deltaX * sensivityOX);
			camera->RotateFirstPerson_OX(-deltaY * sensivityOY);

			// Rotate the camera in First-person mode around OX and OY using deltaX and deltaY
			// use the sensitivity variables for setting up the rotation speed
		}

		if (window->GetSpecialKeyState() && GLFW_MOD_CONTROL) {
			renderCameraTarget = true;
			camera->RotateThirdPerson_OX(-deltaY * sensivityOY);
			camera->RotateThirdPerson_OY(-deltaX * sensivityOX);
			// Rotate the camera in Third-person mode around OX and OY using deltaX and deltaY
			// use the sensitivity variables for setting up the rotation speed
		}

	}
}

void Tema::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Tema::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Tema::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema::OnWindowResize(int width, int height)
{
}
