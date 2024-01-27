#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "GL\glew.h"
#include "GL\freeglut.h"
#include <vector>
#include <string>
#include <numeric>
#include "shaderLoader.h" //narzedzie do ladowania i kompilowania shaderow z pliku

//funkcje algebry liniowej
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective

#pragma warning(disable : 4996) //ignorowanie bledu funkcji wczytywania plikow

//Wymiary okna
int screen_width = 640;
int screen_height = 480;

int pozycjaMyszyX; // na ekranie
int pozycjaMyszyY;
int mbutton; // wcisiety klawisz myszy
int model = 1; // ktory model ma byc wyswietlany

double kameraX = 60.0;
double kameraZ = 30.0;
double kameraD = -3.0;
double kameraPredkosc;
double kameraKat = 20;
double kameraPredkoscObrotu;
double poprzednie_kameraX;
double poprzednie_kameraZ;
double poprzednie_kameraD;
double rotation = 0;


//macierze
glm::mat4 MV; //modelview - macierz modelu i świata
glm::mat4 P;  //projection - macierz projekcji, czyli naszej perspektywy


glm::vec3 lightPos(0.0f, 10.0f, 0.0f);
GLuint objectColor_id = 0;
GLuint lightColor_id = 0;
GLuint lightPos_id = 0;
GLuint viewPos_id = 0;


//deklaracja kolorow
const glm::vec3 chocolate(0.168f, 0.09f, 0.0f);
const glm::vec3 brown(0.6f, 0.3f, 0.0f);
const glm::vec3 red(1.0f, 0.0f, 0.0f);
const glm::vec3 orange(1.0f, 0.5f, 0.0f);
const glm::vec3 yellow(1.0f, 1.0f, 0.0f);
const glm::vec3 green(0.0f, 1.0f, 0.0f);
const glm::vec3 cyan(0.0f, 1.0f, 1.0f);
const glm::vec3 blue(0.0f, 0.0f, 1.0f);
const glm::vec3 indigo(0.3f, 0.0f, 0.25f);
const glm::vec3 violet(0.2f, 0.1f, 0.3f);


//deklaracja funkcji do wczytywania plikow obj
bool loadOBJ(const char* path,
	std::vector < glm::vec3 >& out_vertices,
	std::vector < glm::vec2 >& out_uvs, 
	std::vector < glm::vec3 >& out_normals) {

	//przechowywanie danych z pliku obj w tymczasowych zmiennych
	std::vector< unsigned int> vertexIndices, uvIndices, normalIndices;

	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;


	//otwarcie pliku obj
	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("Nie udalo sie otworzyc pliku !\n");
		return false;
	}

	while (1) {
		char lineHeader[200];

		// odczytanie pierwszej litery w lini
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = koniec pliku

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];
			//unsigned int normalIndex[4];
			
			int matches = fscanf(file, "%d//%d %d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2], &vertexIndex[3], &normalIndex[3]);
			//int matches = fscanf(file, "%d/%d %d/%d %d/%d %d/%d\n", &vertexIndex[0], &uvIndex[0], &vertexIndex[1], &uvIndex[1], &vertexIndex[2], &uvIndex[2], &vertexIndex[3], &uvIndex[3]);
			
			if (matches !=  8) {
				printf("Nie mozna wczytac pliku, plik zawiera nieobslugiwane dane\n");
				return false;
			}

			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			vertexIndices.push_back(vertexIndex[3]);
			//uvIndices.push_back(uvIndex[0]);
			//uvIndices.push_back(uvIndex[1]);
			//uvIndices.push_back(uvIndex[2]);
			//uvIndices.push_back(uvIndex[3]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
			normalIndices.push_back(normalIndex[3]);

		}
		for (unsigned int i = 0; i < vertexIndices.size(); i++) {
			unsigned int vertexIndex = vertexIndices[i];
			glm::vec3 vertex = temp_vertices[vertexIndex - 1];
			out_vertices.push_back(vertex);
		}

		for (unsigned int i = 0; i < uvIndices.size(); i++) {
			unsigned int uvIndex = uvIndices[i];
			glm::vec2 uvs = temp_uvs[uvIndex - 1];
			out_uvs.push_back(uvs);
		}

		for (unsigned int i = 0; i < normalIndices.size(); i++) {
			unsigned int normalIndex = normalIndices[i];
			glm::vec3 normal = temp_normals[normalIndex - 1];
			out_normals.push_back(normal);
		}
	}
}


//funkcja do znajdywania min wysokosci
float findMin(std::vector < glm::vec3 >& in_vertices) {
	float min = in_vertices[0].y, tmp;

	long ilosc = in_vertices.size();

	for (unsigned int i = 0; i < ilosc; i++) {
		tmp = in_vertices[i].y;

		if (tmp < min) {
			min = tmp;
		}
	}
	return min;
}


//funkcja do znajdywania max wysokosci
float findMax(std::vector < glm::vec3 >& in_vertices) {
	float max = in_vertices[0].y, tmp;

	long ilosc = in_vertices.size();

	for (unsigned int i = 0; i < ilosc; i++) {
		tmp = in_vertices[i].y;

		if (tmp > max) {
			max = tmp;
		}
	}
	return max;
}




//funkcja do nadawania kolorow wierzcholkom
bool createRainbowColors(std::vector < glm::vec3 >& in_vertices ,std::vector < glm::vec3 >& out_colors) {
	float min, max;
	glm::vec3 color;
	long ilosc = in_vertices.size();
	float r, g, b;

	min = findMin(in_vertices);
	max = findMax(in_vertices);



	float d = (abs(min) + abs(max)) / 9;

	
	for (unsigned int i = 0; i < ilosc; i++){
		float height = in_vertices[i].y;
		float normHeight = ((height + 1) - (min + 1)) / ((max + 1) - (min + 1));

		/*
		if (height < min + d) {
			color = indigo;
		}
		else if (height < min + d * 2) {
			color = violet;
		}
		else if (height < min + d * 3) {
			color = blue;
		}
		else if (height < min + d * 4) {
			color = cyan;
		}
		else if (height < min + d * 5) {
			color = green;
		}
		else if (height < min + d * 6) {
			color = yellow;
		}
		else if (height < min + d * 7) {
			color = orange;
		}
		else if (height < min + d * 8) {
			color = red;
		}
		else if (height < min + d * 9) {
			color = brown;
		}
		else {
			color = chocolate;
		}
		*/

		r = sin(normHeight * 3.14 - 3.14 / 4);
		g = sin(normHeight * 3.14);
		b = sin(normHeight * 3.14 + 3.14 / 2);

		color = { r,g,b };

		out_colors.push_back(color);
	}
	return true;
}



// wczytanie pliku obj
std::vector< glm::vec3 > vertices;
std::vector< glm::vec2 > uvs;
std::vector< glm::vec3 > normals;
std::vector < glm::vec3 > colors;

bool res = loadOBJ("terrain2.obj", vertices, uvs, normals);


//zapisanie ile wierzcholkow ma zostac wygenerowane
const long ilosc_wierzcholkow = vertices.size();
const long ilosc_normalnych = normals.size();

bool res2 = createRainbowColors(vertices, colors);
const long ilosc_kolorow = (normals.size());

//shaders
GLuint programID = 0, programID2 = 0, programID3 = 0;

unsigned int VBO, VBO2;
unsigned int nVBO, nVBO2;
unsigned int kVBO, kVBO2;
unsigned int VAO[3];



//funkcja do rysowania modelu w postaci siatki
void rysuj(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Kasowanie ekranu

	 //uzyj programu, czyli naszego shadera	

	MV = glm::mat4(1.0f);  //macierz jednostkowa
	MV = glm::translate(MV, glm::vec3(0, 0, kameraD));
	MV = glm::rotate(MV, (float)glm::radians(kameraZ), glm::vec3(1, 0, 0));
	MV = glm::rotate(MV, (float)glm::radians(kameraX), glm::vec3(0, 1, 0));

	glm::mat4 MVP = P * MV;

	GLuint MVP_id = glGetUniformLocation(programID, "MVP"); // pobierz lokalizację zmiennej 'uniform' "MV" w programie
	glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &(MVP[0][0]));	   // wyślij tablicę mv do lokalizacji "MV", która jest typu mat4	

	//Wygenerowanie terenu za pomoca siatki
	glUseProgram(programID);
	GLfloat attrib[] = { 1.0f, 1.0f, 1.0f };
	//glVertexAttrib3fv(1, attrib);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glBindVertexArray(VAO[0]);
	//glDrawArrays(GL_QUADS, 0, ilosc_wierzcholkow);

	
	attrib[0] = 0.6f; attrib[1] = 0.6f; attrib[2] = 0.6f;
	glVertexAttrib3fv(1, attrib);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_QUADS, 0, ilosc_wierzcholkow);

	glFlush();
	glutSwapBuffers();
}


//funkcja do rysowania modelu z cieniowaniem
void rysuj2(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Kasowanie ekranu

	 //uzyj programu, czyli naszego shadera	

	MV = glm::mat4(1.0f);  //macierz jednostkowa
	MV = glm::translate(MV, glm::vec3(0, 0, kameraD));
	MV = glm::rotate(MV, (float)glm::radians(kameraZ), glm::vec3(1, 0, 0));
	MV = glm::rotate(MV, (float)glm::radians(kameraX), glm::vec3(0, 1, 0));

	glm::mat4 MVP = P * MV;

	GLuint MVP_id = glGetUniformLocation(programID, "MVP"); // pobierz lokalizację zmiennej 'uniform' "MV" w programie
	glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &(MVP[0][0]));	   // wyślij tablicę mv do lokalizacji "MV", która jest typu mat4	

	
	//Wygenerowanie terenu z cieniowaniem
	glUseProgram(programID2);

	glUniform3f(objectColor_id, 0.5f, 0.5f, 0.5f);
	glUniform3f(lightColor_id, 1.0f, 1.0f, 1.0f);
	glUniform3f(viewPos_id, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightPos_id, lightPos[0], lightPos[1], lightPos[2]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(VAO[1]);
	glDrawArrays(GL_QUADS, 0, ilosc_wierzcholkow);
	

	glFlush();
	glutSwapBuffers();
}


//funkcja do rysowania modelu w kolorowej skali
void rysuj3(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Kasowanie ekranu

	 //uzyj programu, czyli naszego shadera	

	MV = glm::mat4(1.0f);  //macierz jednostkowa
	MV = glm::translate(MV, glm::vec3(0, 0, kameraD));
	MV = glm::rotate(MV, (float)glm::radians(kameraZ), glm::vec3(1, 0, 0));
	MV = glm::rotate(MV, (float)glm::radians(kameraX), glm::vec3(0, 1, 0));

	glm::mat4 MVP = P * MV;

	GLuint MVP_id = glGetUniformLocation(programID, "MVP"); // pobierz lokalizację zmiennej 'uniform' "MV" w programie
	glUniformMatrix4fv(MVP_id, 1, GL_FALSE, &(MVP[0][0]));	   // wyślij tablicę mv do lokalizacji "MV", która jest typu mat4	

	//Wygenerowanie terenu w kolorowej skali
	glUseProgram(programID3);
	GLfloat attrib[] = { 0.5f, 0.5f, 0.5f };
	glVertexAttrib3fv(1, attrib);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(VAO[2]);
	glDrawArrays(GL_QUADS, 0, ilosc_wierzcholkow);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	attrib[0] = 0.2f; attrib[1] = 0.2f; attrib[2] = 0.2f;
	glVertexAttrib3fv(1, attrib);
	glDrawArrays(GL_QUADS, 0, ilosc_wierzcholkow);

	glFlush();
	glutSwapBuffers();

}


//funkcja do obslugi klawiszy
void klawisz(GLubyte key, int x, int y)
{
	switch (key) {

	case 27:    // Esc - koniec
		exit(1);
		break;

	case '1':
		glutDisplayFunc(rysuj);
		break;
	case '2':
		glutDisplayFunc(rysuj2);
		break;
	case '3':
		glutDisplayFunc(rysuj3);
		break;
	}
}


void rozmiar(int width, int height)
{
	screen_width = width;
	screen_height = height;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screen_width, screen_height);

	P = glm::perspective(glm::radians(60.0f), (GLfloat)screen_width / (GLfloat)screen_height, 1.0f, 1000.0f);

	glutPostRedisplay(); // Przerysowanie sceny
}


void idle()
{
	glutPostRedisplay();
}


void timer(int value) {

	glutTimerFunc(20, timer, 0);
}


void mysz(int button, int state, int x, int y)
{
	mbutton = button;
	switch (state)
	{
	case GLUT_UP:
		break;
	case GLUT_DOWN:
		pozycjaMyszyX = x;
		pozycjaMyszyY = y;
		poprzednie_kameraX = kameraX;
		poprzednie_kameraZ = kameraZ;
		poprzednie_kameraD = kameraD;
		break;

	}
}


void mysz_ruch(int x, int y)
{
	if (mbutton == GLUT_LEFT_BUTTON)
	{
		kameraX = poprzednie_kameraX - (pozycjaMyszyX - x) * 0.1;
		kameraZ = poprzednie_kameraZ - (pozycjaMyszyY - y) * 0.1;
	}
	if (mbutton == GLUT_RIGHT_BUTTON)
	{
		kameraD = poprzednie_kameraD + (pozycjaMyszyY - y) * 0.1;
	}

}




int main(int argc, char** argv)
{
	std::cout << ilosc_wierzcholkow << std::endl;
	//std::cout << ilosc_normalnych << std::endl;
	//std::cout << ilosc_kolorow << std::endl;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screen_width, screen_height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("NMT");

	glewInit(); //init rozszerzeszeń OpenGL z biblioteki GLEW

	glutDisplayFunc(rysuj);			// def. funkcji rysuj¦cej
	glutIdleFunc(idle);			// def. funkcji rysuj¦cej w czasie wolnym procesoora (w efekcie: ci¦gle wykonywanej)
	glutTimerFunc(100, timer, 0);
	glutReshapeFunc(rozmiar); // def. obs-ugi zdarzenia resize (GLUT)

	glutKeyboardFunc(klawisz);		// def. obsługi klawiatury
	glutMouseFunc(mysz); 		// def. obsługi zdarzenia przycisku myszy (GLUT)
	glutMotionFunc(mysz_ruch); // def. obsługi zdarzenia ruchu myszy (GLUT)


	glEnable(GL_DEPTH_TEST);

	glGenVertexArrays(2, VAO);


	//Wygenerowanie modelu za pomoca siatki
	programID = loadShaders("vertex_shader.glsl", "fragment_shader.glsl");

	glUseProgram(programID);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glBindVertexArray(VAO[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);




	//Wygenerowanie modelu z cieniowaniem
	programID2 = loadShaders("vertex_shader2.glsl", "fragment_shader2.glsl");
	glUseProgram(programID2);

	objectColor_id = glGetUniformLocation(programID2, "objectColor");
	lightColor_id = glGetUniformLocation(programID2, "lightColor");
	lightPos_id = glGetUniformLocation(programID2, "lightPos");
	viewPos_id = glGetUniformLocation(programID2, "viewPos");

	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);


	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// normal attribute
	glGenBuffers(1, &nVBO);
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, nVBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);



	//model z nalozona skala barw
	programID3 = loadShaders("vertex_shader3.glsl", "fragment_shader3.glsl");
	glUseProgram(programID3);

	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glBindVertexArray(VAO[2]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


	glGenBuffers(1, &kVBO);
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, kVBO);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	glutMainLoop();

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &nVBO);

	glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &nVBO2);

	glDeleteBuffers(1, &kVBO);
	glDeleteBuffers(1, &kVBO2);

	glDeleteBuffers(3, VAO);
	return(0);
}

