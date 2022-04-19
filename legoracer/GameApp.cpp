#include <math.h>
#include "MyApp.h"
#include "loaders.hpp"

CMyApp::CMyApp(void)
{
    //Buffers
    sphere_vaoID = 0;
    sphere_vboID = 0;
    sphere_indID = 0;
	disc_vaoID = 0;
	disc_vboID = 0;
	disc_indID = 0;

    //Shader program
    shaderProgramID = 0;
}
CMyApp::~CMyApp(void)
{

}

//Main
bool CMyApp::Init()
{
    glClearColor(0.125f, 0.25f, 0.5f, 1.0f);
    glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    glPolygonMode(GL_BACK, GL_LINE);

	//Generate VAO, VBO pairs
    generateSphere(sphere_vaoID, sphere_vboID, sphere_indID);
	generateDisc(disc_vaoID, disc_vboID, disc_indID);
	generateLego(lego_vaoID, lego_vboID);
	generateCylinder(cylinder_vaoID, cylinder_vboID);

    //Compile shaders
    GLuint vs_ID = loadShader(GL_VERTEX_SHADER, "solid.vert");
    GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER, "solid.frag");

    shaderProgramID = glCreateProgram();

    glAttachShader(shaderProgramID, vs_ID);
    glAttachShader(shaderProgramID, fs_ID);

    glBindAttribLocation(shaderProgramID, 0, "vs_in_pos");
    glBindAttribLocation(shaderProgramID, 1, "vs_in_col");

	//Link shader
    glLinkProgram(shaderProgramID);

    //Validate link
    GLint infoLogLength = 0, result = 0;
    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &result);
    glGetProgramiv(shaderProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if ( GL_FALSE == result )
    {
        std::vector<char> ProgramErrorMessage( infoLogLength );
        glGetProgramInfoLog(shaderProgramID, infoLogLength, NULL, &ProgramErrorMessage[0]);
        fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
        
        char* aSzoveg = new char[ProgramErrorMessage.size()];
        memcpy( aSzoveg, &ProgramErrorMessage[0], ProgramErrorMessage.size());

        std::cout << "[app.Init()] Sáder Huba panasza: " << aSzoveg << std::endl;

        delete aSzoveg;
    }

	//Unload shader
    glDeleteShader( vs_ID );
    glDeleteShader( fs_ID );

    //Matrices
    m_matProj = glm::perspective( 45.0f, 640/480.0f, 1.0f, 1000.0f );

    m_loc_mvp = glGetUniformLocation( shaderProgramID, "MVP");
    m_loc_w = glGetUniformLocation( shaderProgramID, "world" );
    m_loc_wit = glGetUniformLocation( shaderProgramID, "WorldIT" );
    m_loc_eye = glGetUniformLocation(shaderProgramID, "eye_pos");
	m_col = glGetUniformLocation(shaderProgramID, "Kd");

	Lego l;
	l.pos = glm::vec3(0, (1.0/3), 0);
	l.col = glm::vec3(0.7, 0.3, 0.3);
	l.size = glm::vec3(4, 1.0/3, 6);
	l.Yrot = 0;
	legos.push_back(l);

	currentLego.pos = glm::vec3(0, (2.0/3), 0);
	currentLego.col = glm::vec3(0.8, 0, 0.2);
	currentLego.size = glm::vec3(1, (1.0/3), 2);
	currentLego.Yrot = 0;
	legos.push_back(currentLego);

	fieldColor = glm::vec3(0.6, 0.6, 0.6);

    return true;
}
void CMyApp::Clean()
{
    glDeleteBuffers(1, &sphere_vboID);
    glDeleteBuffers(1, &sphere_indID);
    glDeleteVertexArrays(1, &sphere_vaoID);

    glDeleteProgram(shaderProgramID);
}
void CMyApp::Update()
{
    //Look
    m_matView = glm::lookAt(m_eye, m_at, glm::vec3(0, 1, 0));

    //Move
    glm::vec3 forward = glm::normalize(m_at - m_eye);
    if (moveForward)
    {
        m_eye += forward * glm::vec3(0.1);
        m_at = m_eye + toDesc(m_fi, m_theta);
    }
    if (moveBack)
    {
        m_eye -= forward * glm::vec3(0.1);
        m_at = m_eye + toDesc(m_fi, m_theta);
    }
    if (moveLeft)
    {
        m_eye += glm::normalize(glm::cross(glm::vec3(0, 1, 0), forward)) * glm::vec3(0.1);
        m_at = m_eye + toDesc(m_fi, m_theta);
    }
    if (moveRight)
    {
        m_eye -= glm::normalize(glm::cross(glm::vec3(0, 1, 0), forward)) * glm::vec3(0.1);
        m_at = m_eye + toDesc(m_fi, m_theta);
    }
}
void CMyApp::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Turn on shader
    glUseProgram(shaderProgramID);

	//glm::rotate<float>(deg, glm::vec3(x, y, z));
	//glm::translate<float>(glm::vec3(x, y, z));
	//glm::scale<float>(glm::vec3(x, y, z));

	glUniform3fv(m_loc_eye, 1, &m_eye[0]); //Camera location

	//BEGIN
	//glm::vec4 lightPos = glm::vec4(0, 10, 0, 2);
	//glLightfv(GL_LIGHT0, GL_POSITION, &lightPos[0]);

	//Cylinder
	glBindVertexArray(cylinder_vaoID);
	m_matWorld = glm::translate<float>(glm::vec3(3, 3, 3));
	updateMVP();
	glDrawArrays(GL_QUAD_STRIP, 0, 120 * 2 + 2);

	//Field
	glBindVertexArray(lego_vaoID);
	m_matWorld = glm::scale<float>(glm::vec3(30, 0, 30)) *
				 glm::rotate<float>(M_PI, glm::vec3(1, 0, 0));
	updateMVP();
	glUniform3fv(m_col, 1, &fieldColor[0]);
	glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_SHORT, 0);

	//Legos
	
	for (int i = 0; i < legos.size(); i++)
	{
		m_matWorld = glm::translate<float>(legos[i].pos) *
			         glm::rotate<float>(legos[i].Yrot, glm::vec3(0, 1, 0)) *
			         glm::scale<float>(legos[i].size);
		updateMVP();
		glUniform3fv(m_col, 1, &legos[i].col[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3 * 2 * 6);
	}

	//Selected lego
	m_matWorld = glm::translate<float>(currentLego.pos) *
		         glm::rotate<float>(currentLego.Yrot, glm::vec3(0, 1, 0)) *
		         glm::scale<float>(currentLego.size);
	updateMVP();
	glUniform3fv(m_col, 1, &currentLego.col[0]);
	glDrawArrays(GL_TRIANGLES, 0, 3 * 2 * 6);
    
	//END

    glBindVertexArray(0); //Deactivate VBO
    glUseProgram(0); //Turn off shader
}

//Events

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
    switch (key.keysym.sym)
    {
    case SDLK_w:
        moveForward = true;
        break;
    case SDLK_s:
        moveBack = true;
        break;
    case SDLK_a:
        moveLeft = true;
        break;
    case SDLK_d:
        moveRight = true;
		break;
	case SDLK_0:
		newLego();
		break;
	case SDLK_4: //Left
		currentLego.pos += glm::vec3(-1, 0, 0);
		break;
	case SDLK_6: //Right
		currentLego.pos += glm::vec3(1, 0, 0);
        break;
	case SDLK_2: //Forward
		currentLego.pos += glm::vec3(0, 0, 1);
		break;
	case SDLK_8: //Back
		currentLego.pos += glm::vec3(0, 0, -1);
		break;
	case SDLK_3: //Down
		currentLego.pos += glm::vec3(0, -(1.0/3), 0);
		break;
	case SDLK_9: //Up
		currentLego.pos += glm::vec3(0, 1.0/3, 0);
		break;
	case SDLK_5: //Rotate 90deg
		currentLego.Yrot += M_PI / 2; 
		break;
	case SDLK_o:
		currentLego.col = glm::vec3(0, 0, 0);
		break;
	case SDLK_p:
		currentLego.col = glm::vec3(1, 1, 1);
		break;
	case SDLK_k:
		currentLego.col = glm::vec3(1, 0, 0);
		break;
	case SDLK_l:
		currentLego.col = glm::vec3(0, 1, 0);
		break;
	case SDLK_n:
		currentLego.col = glm::vec3(1, 1, 0);
		break;
	case SDLK_m:
		break;
    }
}
void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
    switch (key.keysym.sym) {
    case SDLK_w:
        moveForward = false;
        break;
    case SDLK_s:
        moveBack = false;
        break;
    case SDLK_a:
        moveLeft = false;
        break;
    case SDLK_d:
        moveRight = false;
        break;
    }
}
void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{
    if (mouse.state & SDL_BUTTON_LMASK)
    {
        m_theta -= mouse.xrel*0.005;
        m_fi += mouse.yrel*0.005;
        m_fi = glm::clamp(m_fi, 0.001f, 3.13f);
        m_at = m_eye + toDesc(m_fi, m_theta);
    }
}
void CMyApp::MouseDown(SDL_MouseButtonEvent& mouse)
{
}
void CMyApp::MouseUp(SDL_MouseButtonEvent& mouse)
{
}
void CMyApp::MouseWheel(SDL_MouseWheelEvent& wheel)
{
}
void CMyApp::Resize(int _w, int _h)
{
    glViewport(0, 0, _w, _h);
    m_matProj = glm::perspective(45.0f, _w/(float)_h, 0.01f, 100.0f);
}

//Helper methods

void CMyApp::generateSphere(GLuint& vao, GLuint &vbo, GLuint& ind)
{
    //Verticies
    Vertex vert[(N + 1)*(M + 1)];
	for (int i = 0; i <= N; ++i)
	{
		for (int j = 0; j <= M; ++j)
		{
			float u = i / (float)N;
			float v = j / (float)M;

			vert[i + j * (N + 1)].p = GetPos(u, v);
			vert[i + j * (N + 1)].n = GetNorm(u, v);
		}
	}

    //Indexbuffer
    GLushort indices[3 * 2 * N * M];
	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < M; ++j)
		{
			int temp = 6*i + j*3*2*(N);
			indices[temp + 0] = (i)   + (j)   * (N+1);
			indices[temp + 1] = (i+1) + (j)   * (N+1);
			indices[temp + 2] = (i)   + (j+1) * (N+1);
			indices[temp + 3] = (i+1) + (j)   * (N+1);
			indices[temp + 4] = (i+1) + (j+1) * (N+1);
			indices[temp + 5] = (i)   + (j+1) * (N+1);
		}
	}

    glGenVertexArrays(1, &vao); //Generate VAO
    glBindVertexArray(vao);     //Activate VAO

    glGenBuffers(1, &vbo);              //Generate VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo); //Activate VBO

	//Fill data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

	//VAO definition
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));

    //Index buffer
    glGenBuffers(1, &ind);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Unbind buffers
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    std::cout << "Generated: sphere (vao: " << vao <<
                                  ", vbo: " << vbo <<
                                  ", ind: " << ind << ")\n";
}
void CMyApp::generateDisc(GLuint& vao, GLuint &vbo, GLuint& ind)
{
	//Verticies
	float radius = 1.0f;

	Vertex vert[60];
	float step = M_PI * 2.0f / 60.0f;
	for (int i = 0; i < 60; i++)
	{
		float x = radius * cosf(step * i);
		float z = radius * sinf(step * i);
		//std::cout << x << " " << y << "\n";

		vert[i].p = glm::vec3(x, 0.0f, z);
		vert[i].n = glm::vec3(0.0f, 1.0f, 0.0f);
	}

	//Indexbuffer
	//GLushort* indices = new GLushort[count * 3];
	GLushort indices[60];
	for (int i = 0; i < 60; i++)
	{
		indices[i] = i;
	}

	glGenVertexArrays(1, &vao); //Generate VAO
	glBindVertexArray(vao);     //Activate VAO

	glGenBuffers(1, &vbo);              //Generate VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo); //Activate VBO

	//Fill data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

	//VAO definition
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));

	//Index buffer
	glGenBuffers(1, &ind);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ind);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Unbind buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	std::cout << "Generated: disc (vao: " << vao <<
		                        ", vbo: " << vbo <<
		                        ", ind: " << ind << ")\n";
}
void CMyApp::generateLego(GLuint& vao, GLuint& vbo)
{
	Vertex vert[]
	{
		//Left
		{ glm::vec3(-0.5,-0.5,-0.5), glm::vec3(1, 0, 0), },
		{ glm::vec3(-0.5,-0.5, 0.5), glm::vec3(1, 0, 0), },
		{ glm::vec3(-0.5, 0.5, 0.5), glm::vec3(1, 0, 0), },
		{ glm::vec3(-0.5,-0.5,-0.5), glm::vec3(1, 0, 0), },
		{ glm::vec3(-0.5, 0.5, 0.5), glm::vec3(1, 0, 0), },
		{ glm::vec3(-0.5, 0.5,-0.5), glm::vec3(1, 0, 0), },

		//Back
		{ glm::vec3(0.5, 0.5,-0.5), glm::vec3(0, 0, 1), },
		{ glm::vec3(0.5,-0.5,-0.5), glm::vec3(0, 0, 1), },
		{ glm::vec3(-0.5,-0.5,-0.5), glm::vec3(0, 0, 1), },
		{ glm::vec3(0.5, 0.5,-0.5), glm::vec3(0, 0, 1), },
		{ glm::vec3(-0.5,-0.5,-0.5), glm::vec3(0, 0, 1), },
		{ glm::vec3(-0.5, 0.5,-0.5), glm::vec3(0, 0, 1), },

		//Bottom
		{ glm::vec3(0.5,-0.5, 0.5), glm::vec3(0, 1, 0), },
		{ glm::vec3(-0.5,-0.5,-0.5), glm::vec3(0, 1, 0), },
		{ glm::vec3(0.5,-0.5,-0.5), glm::vec3(0, 1, 0), },
		{ glm::vec3(0.5,-0.5, 0.5), glm::vec3(0, 1, 0), },
		{ glm::vec3(-0.5,-0.5, 0.5), glm::vec3(0, 1, 0), },
		{ glm::vec3(-0.5,-0.5,-0.5), glm::vec3(0, 1, 0), },

		//Right
		{ glm::vec3(0.5, 0.5, 0.5), glm::vec3(1, 0, 0), },
		{ glm::vec3(0.5,-0.5,-0.5), glm::vec3(1, 0, 0), },
		{ glm::vec3(0.5, 0.5,-0.5), glm::vec3(1, 0, 0), },
		{ glm::vec3(0.5,-0.5,-0.5), glm::vec3(1, 0, 0), },
		{ glm::vec3(0.5, 0.5, 0.5), glm::vec3(1, 0, 0), },
		{ glm::vec3(0.5,-0.5, 0.5), glm::vec3(1, 0, 0), },

		//Top
		{ glm::vec3(0.5, 0.5, 0.5), glm::vec3(0, 1, 0), },
		{ glm::vec3(0.5, 0.5,-0.5), glm::vec3(0, 1, 0), },
		{ glm::vec3(-0.5, 0.5,-0.5), glm::vec3(0, 1, 0), },
		{ glm::vec3(0.5, 0.5, 0.5), glm::vec3(0, 1, 0), },
		{ glm::vec3(-0.5, 0.5,-0.5), glm::vec3(0, 1, 0), },
		{ glm::vec3(-0.5, 0.5, 0.5), glm::vec3(0, 1, 0), },

		//Front
		{ glm::vec3(-0.5, 0.5, 0.5), glm::vec3(0, 0, 1), },
		{ glm::vec3(-0.5,-0.5, 0.5), glm::vec3(0, 0, 1), },
		{ glm::vec3(0.5,-0.5, 0.5), glm::vec3(0, 0, 1), },
		{ glm::vec3(0.5, 0.5, 0.5), glm::vec3(0, 0, 1), },
		{ glm::vec3(-0.5, 0.5, 0.5), glm::vec3(0, 0, 1), },
		{ glm::vec3(0.5,-0.5, 0.5), glm::vec3(0, 0, 1), },	
	};

	glGenVertexArrays(1, &vao); //Generate VAO
	glBindVertexArray(vao);     //Activate VAO

	glGenBuffers(1, &vbo);              //Generate VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo); //Activate VBO

	//Fill data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

	//VAO definition
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));

	//Unbind buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	std::cout << "Generated: lego (vao: " << vao <<
		                        ", vbo: " << vbo << "\n";
}
void CMyApp::generateCylinder(GLuint& vao, GLuint& vbo)
{
	//Vertex vertices[480];
	std::vector<Vertex> v;

	const float radius = 0.5;
	const float step = M_PI * 2 / 60;
	const float height = 1.0f;
	float angle = 0.0f;

	while (angle < M_PI * 3)
	{
		float x = radius * cosf(angle);
		float y = radius * sinf(angle);
		v.push_back({ glm::vec3(x, y, height), glm::vec3(x, y, 0) });
		v.push_back({ glm::vec3(x, y, 0.0), glm::vec3(x, y, 0) });
		angle += step;
	}
	//v.push_back({ glm::vec3(radius, 0.0, height), glm::vec3(radius, 0, 0) });
	//v.push_back({ glm::vec3(radius, 0.0, 0.0), glm::vec3(radius, 0, 0) });

	glGenVertexArrays(1, &vao); //Generate VAO
	glBindVertexArray(vao);     //Activate VAO

	glGenBuffers(1, &vbo);              //Generate VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo); //Activate VBO

	//Fill data
	glBufferData(GL_ARRAY_BUFFER, sizeof(v) * v.size(), &v[0], GL_STATIC_DRAW);

	//VAO definition
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));

	//Unbind buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	std::cout << "Generated: cylinder (vao: " << vao <<
		", vbo: " << vbo << "\n";
}

glm::vec3 CMyApp::toDesc(float fi, float theta)
{
    return glm::vec3(sin(fi) * cos(theta), cos(fi), -sin(fi) * sin(theta));
}
//Get point in parametric surface of it's (u, v) parameter
glm::vec3 CMyApp::GetPos(float u, float v)
{
    //Y and Z coordinates are switched compared to mathematical approximations
    //Source: http://hu.wikipedia.org/wiki/G%C3%B6mb#Egyenletek 

    u *= 2 * 3.1415f;
    v *= 3.1415f;

    float cu = cosf(u);
    float su = sinf(u);
    float cv = cosf(v);
    float sv = sinf(v);
    float r = 2;

    return glm::vec3(r * cu * sv, r * cv, r * su * sv);
}
//Get normal vector in parametric surface of it's (u, v) parameter
glm::vec3 CMyApp::GetNorm(float u, float v)
{
    u *= 2 * 3.1415f;
    v *= 3.1415f;

    float cu = cosf(u);
    float su = sinf(u);
    float cv = cosf(v);
    float sv = sinf(v);

    return glm::vec3(cu*sv, cv, su*sv);
}

void CMyApp::updateMVP()
{
	glm::mat4 mvp = m_matProj * m_matView * m_matWorld;
	glm::mat4 WorldIT = glm::inverse(glm::transpose(m_matWorld));

	glUniformMatrix4fv(m_loc_mvp, 1, GL_FALSE, &(mvp[0][0]));
	glUniformMatrix4fv(m_loc_wit, 1, GL_FALSE, &(WorldIT[0][0]));
	glUniformMatrix4fv(m_loc_w, 1, GL_FALSE, &(m_matWorld[0][0]));
}

void CMyApp::newLego()
{
	legos.push_back(currentLego);
	currentLego.pos += glm::vec3(0, 1, 0);
}