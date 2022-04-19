#pragma once

//GLEW
#include <GL/glew.h>

//SDL
#include <SDL.h>
#include <SDL_opengl.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

//STD
#include <vector>

class GameApp
{
public:
	GameApp(void);
	~GameApp(void);

	bool Init(void);
	void Clean(void);
	void Update(void);
	void Render(void);

	void KeyboardDown(SDL_KeyboardEvent&);
	void KeyboardUp(SDL_KeyboardEvent&);
	void MouseMove(SDL_MouseMotionEvent&);
	void MouseDown(SDL_MouseButtonEvent&);
	void MouseUp(SDL_MouseButtonEvent&);
	void MouseWheel(SDL_MouseWheelEvent&);
	void Resize(int, int);

	struct Vertex
	{
		glm::vec3 p;
		glm::vec3 n;
	};
	struct Lego
	{
		glm::vec3 pos;
		glm::vec3 col;
		glm::vec3 size;
		float Yrot;
	};

protected:
	//Helper methods
	glm::vec3 GetPos(float u, float v);
	glm::vec3 GetNorm(float u, float v);
	glm::vec3 toDesc(float fi, float theta);
	void generateSphere(GLuint&, GLuint&, GLuint&);
	void generateDisc(GLuint&, GLuint&, GLuint&);
	void generateCylinder(GLuint&, GLuint&);
	void generateLego(GLuint&, GLuint&);
	void updateMVP();
	void newLego();

	//Shader Programs
	GLuint shaderProgramID;

	//Buffer objects
	GLuint sphere_vaoID;
	GLuint sphere_vboID;
	GLuint sphere_indID;

	GLuint disc_vaoID;
	GLuint disc_vboID;
	GLuint disc_indID;

	GLuint lego_vaoID;
	GLuint lego_vboID;

	GLuint cylinder_vaoID;
	GLuint cylinder_vboID;

	//Transform matrices
	glm::mat4 m_matWorld;
	glm::mat4 m_matView;
	glm::mat4 m_matProj;

	//Shader matix positions
	GLuint	m_loc_mvp;
	GLuint  m_loc_w;
	GLuint  m_loc_wit;
	GLuint  m_loc_eye;
	GLuint  m_col;

	//Parametric surface approximation (NxM)
	static const int N	= 20;
	static const int M	= 20;

	//Camera
	float m_fi = M_PI / 2.0;
	float m_theta = M_PI / 2.0;
	glm::vec3 m_eye = glm::vec3(0, 0, 10);
	glm::vec3 m_at = m_eye + toDesc(m_fi, m_theta);

	bool moveForward = false;
	bool moveLeft = false;
	bool moveRight = false;
	bool moveBack = false;

	//Field
	std::vector<Lego> legos;
	Lego currentLego;
	glm::vec3 fieldColor;
};