// ================================================
// | Grafica pe calculator - Domino 2D           |
// ================================================
// Simulare domino: dreptunghiuri care cad in cascada
// Primul domino cade si impinge fizic urmatorul domino

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "loadShaders.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <SOIL.h> 
GLuint VaoId, VboId, ColorBufferId, ProgramId;
GLuint myMatrixLocation, codColLocation;
GLuint TexCoordBufferId;
GLuint dominoTextureId, tableTextureId;
GLint dominoTextureLocation, tableTextureLocation;

GLfloat winWidth = 1200, winHeight = 600;
glm::mat4 resizeMatrix;

const float a = 1200.0f; // latime fereastra
const float b = 600.0f; // inaltime fereastra

// Parametri domino
const int NUM_DOMINOS = 17;
const float DOMINO_WIDTH = 30.0f;
const float DOMINO_HEIGHT = 100.0f;
const float DOMINO_SPACING = 60.0f;
const float START_X = 150.0f;
const float GROUND_Y = 150.0f;

// Stare pentru fiecare domino
struct Domino {
    float angle;           // unghi de rotatie (radiani)
    float angularVelocity; // viteza unghiulara
    bool isFalling;        // daca este in cadere
    float posX;            // pozitie X (coltul stanga-jos)
    float pivotX;          // punct de rotatie X (colt stanga-jos)
    float pivotY;          // punct de rotatie Y (jos)
    bool hasHitNext;       // daca a lovit deja urmatorul
};

Domino dominos[NUM_DOMINOS];
bool animationStarted = false;
float time_elapsed = 0.0f;

void CreateShaders(void)
{
    ProgramId = LoadShaders("Shader.vert", "Shader.frag");
    glUseProgram(ProgramId);
}

// Coordonate de textura
GLfloat TexCoords[] = {
    // Domino (relativ la pivot)
    0.0f, 0.0f, // stanga-jos (pivot)
    1.0f, 0.0f, // dreapta-jos
    1.0f, 1.0f, // dreapta-sus
    0.0f, 1.0f, // stanga-sus

    // Fundal (nu se foloseste)
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,

    // Linie pamant (nu se foloseste)
    0.0f, 0.0f,
    1.0f, 0.0f,

    // Masa (Table) - coordonate textura
    0.0f, 0.0f, // stanga-jos
    1.0f, 0.0f, // dreapta-jos
    1.0f, 1.0f, // dreapta-sus
    0.0f, 1.0f  // stanga-sus
};

void CreateVBO(void)
{
    GLfloat Vertices[] = {
        // Domino (relativ la pivot)
        0.0f, 0.0f, 0.0f, 1.0f,                    // stanga-jos (pivot)
        DOMINO_WIDTH, 0.0f, 0.0f, 1.0f,            // dreapta-jos
        DOMINO_WIDTH, DOMINO_HEIGHT, 0.0f, 1.0f,   // dreapta-sus
        0.0f, DOMINO_HEIGHT, 0.0f, 1.0f,           // stanga-sus

        // Fundal 
        0.0f, 0.0f, 0.0f, 1.0f,
        a, 0.0f, 0.0f, 1.0f,
        a, b, 0.0f, 1.0f,
        0.0f, b, 0.0f, 1.0f,

        // Linie de pamant
        0.0f, GROUND_Y, 0.0f, 1.0f,
        a, GROUND_Y, 0.0f, 1.0f,

        // Masa - acopera întreaga suprafara de jos pân? la GROUND_Y
        0.0f, 0.0f, 0.0f, 1.0f,     // stanga-jos
        a, 0.0f, 0.0f, 1.0f,        // dreapta-jos
        a, GROUND_Y, 0.0f, 1.0f,    // dreapta-sus
        0.0f, GROUND_Y, 0.0f, 1.0f  // stanga-sus
    };

    GLfloat Colors[] = {
        // Domino
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        // Fundal gri deschis
        0.9f, 0.95f, 1.0f, 1.0f,
        0.9f, 0.95f, 1.0f, 1.0f,
        0.9f, 0.95f, 1.0f, 1.0f,
        0.9f, 0.95f, 1.0f, 1.0f,

        // Linie pamant neagra
        0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f,

        // Masae,e suprascrisa de textura
        0.5f, 0.3f, 0.2f, 1.0f,
        0.5f, 0.3f, 0.2f, 1.0f,
        0.5f, 0.3f, 0.2f, 1.0f,
        0.5f, 0.3f, 0.2f, 1.0f
    };

    // Creare VAO
    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);

    // Creare VBO pentru varfuri
    glGenBuffers(1, &VboId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    // Creare VBO pentru culori
    glGenBuffers(1, &ColorBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

    // Creare VBO pentru coordonate textura
    glGenBuffers(1, &TexCoordBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, TexCoordBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords), TexCoords, GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void DestroyShaders(void) { glDeleteProgram(ProgramId); }

void DestroyVBO(void)
{
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &TexCoordBufferId);
    glDeleteBuffers(1, &ColorBufferId);
    glDeleteBuffers(1, &VboId);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoId);
}

void Cleanup(void)
{
    DestroyShaders();
    DestroyVBO();
}

void Initialize(void)
{
    glClearColor(0.9f, 0.95f, 1.0f, 1.0f);
    CreateVBO();
    CreateShaders();

    codColLocation = glGetUniformLocation(ProgramId, "codCol");
    myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");

    resizeMatrix = glm::ortho(0.0f, a, 0.0f, b);

    // Initializare dominos
    for (int i = 0; i < NUM_DOMINOS; i++) {
        dominos[i].angle = 0.0f;
        dominos[i].angularVelocity = 0.0f;
        dominos[i].isFalling = false;
        dominos[i].posX = START_X + i * DOMINO_SPACING;
        dominos[i].pivotX = dominos[i].posX;
        dominos[i].pivotY = GROUND_Y;
        dominos[i].hasHitNext = false;
    }

    // Incarcare texturi
    dominoTextureId = SOIL_load_OGL_texture("assets/domino.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    tableTextureId = SOIL_load_OGL_texture("assets/table.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

    dominoTextureLocation = glGetUniformLocation(ProgramId, "dominoTexture");
    tableTextureLocation = glGetUniformLocation(ProgramId, "tableTexture");

    // Seteaza unitatile de textura
    glUseProgram(ProgramId);
    glUniform1i(dominoTextureLocation, 0); // GL_TEXTURE0
    glUniform1i(tableTextureLocation, 1);  // GL_TEXTURE1
}

// Calculeaza pozitia varfului superior drept al dominoului
// Primeste ca parametrii:
// index : indexul dominoului pentru care calculeaza coltul din dreapta sus
// outX si outY sunt variabilele in care functia pune coordonatele coltului 
void GetTopRightCorner(int index, float& outX, float& outY)
{
    float angle = dominos[index].angle;
    float pivotX = dominos[index].pivotX;
    float pivotY = dominos[index].pivotY;

    // Coltul dreapta-sus inainte de rotatie: (DOMINO_WIDTH, DOMINO_HEIGHT)
    // Dupa rotatie in jurul (0,0):
    float localX = DOMINO_WIDTH * cos(angle) - DOMINO_HEIGHT * sin(angle);
    float localY = DOMINO_WIDTH * sin(angle) + DOMINO_HEIGHT * cos(angle);

    outX = pivotX + localX;
    outY = pivotY + localY;
}



// Verifica daca varful dominoului loveste urmatorul domino
// Parametrii:
// currentIndex : indexul dominoului curent
// nextIndex : indexul dominoului urmator
bool CheckCollision(int currentIndex, int nextIndex)
{
    // declararea variabilelor de pozitie in care se pun valorile calculate in GetTopRightCorner
    float pivotNou[] = { 0.0, 0.0 };
    GetTopRightCorner(currentIndex, pivotNou[0], pivotNou[1]);

    // Pozitia fata dominoului urmator (stanga)
    float nextLeftX = dominos[nextIndex].pivotX; // ia pozitia X a pivotului (colt stanga-jos)
    // nextRightX = pozitia pivotului care nu se muta + cat de lat este dominoul * cat s a rotit
    float nextRightX = nextLeftX + DOMINO_WIDTH * cos(dominos[nextIndex].angle);

    // Verifica daca varful intra in zona urmatorului domino
    float minY = GROUND_Y + 10.0f; // minim deasupra pamantului
    float maxY = GROUND_Y + DOMINO_HEIGHT - 10.0f;

    bool hitX = (pivotNou[0] >= nextLeftX - 5.0f) && (pivotNou[0] <= nextRightX + 5.0f);
    bool hitY = (pivotNou[1] >= minY) && (pivotNou[1] <= maxY);

    return hitX && hitY; // vede daca s a lovit
}

// Variabilele constante pentru functia de UpdatePhysics

const float GRAVITY = 500.0f;
// Min_angle = 60 grade clockwise
const float MIN_ANGLE = -1.04f;// = 60 x pi / 180 = pi / 3  
const float DAMPING = 0.98f;   // Amortizare


void UpdatePhysics(float deltaTime)
{
    if (!animationStarted) return;


    for (int i = 0; i < NUM_DOMINOS; i++) {
        Domino& currentDomino = dominos[i];
        if (currentDomino.isFalling && currentDomino.angle > MIN_ANGLE) {
            // Calcul moment de inertie pentru dreptunghi in jurul coltului
            float Inertia = (DOMINO_WIDTH * DOMINO_WIDTH + DOMINO_HEIGHT * DOMINO_HEIGHT) / 3.0f;
            // Centrul de greutate (in coordonate locale)
            float centerOfMass[] = { DOMINO_WIDTH / 2.0f, DOMINO_HEIGHT / 2.0f };

            // aici calculam (eu si ovidu) proiectia ( cat se duce spre dreapta) pe axa X a centrului de greutate
            float newCenterOfMassX = centerOfMass[0] * cos(currentDomino.angle) - centerOfMass[1] * sin(currentDomino.angle);
            // X Value                              // Y Value

    //Calculam torsiune dupa formula F * d unde 
    //F = GRAVITY ( constanta definita la inceput) si d = newCenterOfMassX
    // Inmultim cu -1 pentru ca sensul dorit (clockwise) este invers sensului trigonometric
            float torque = GRAVITY * newCenterOfMassX;
            float angularAccel = -torque / Inertia; // semn minus -> cade spre unghiuri negative

            // updatez datele din currentDomino
            currentDomino.angularVelocity += angularAccel * deltaTime;
            currentDomino.angularVelocity *= DAMPING;
            currentDomino.angle += dominos[i].angularVelocity * deltaTime;

            // Limiteaza unghiul minim (negativ)
            if (currentDomino.angle < MIN_ANGLE) {
                currentDomino.angle = MIN_ANGLE;
                currentDomino.angularVelocity *= 0.3f;
            }

            // Verifica coliziunea cu urmatorul domino
            if (i < NUM_DOMINOS - 1 && !currentDomino.hasHitNext) {
                if (CheckCollision(i, i + 1)) {
                    currentDomino.hasHitNext = true;
                    dominos[i + 1].isFalling = true;
                    // Impuls initial pentru urmatorul domino (semn pastrat pentru cadere spre dreapta)
                    dominos[i + 1].angularVelocity = currentDomino.angularVelocity * 0.7f - 0.5f;
                }
            }
        }
    }
}
void RenderFunction(void)
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Fundal (deasupra mesei)
    int codCol = 0;
    glUniform1i(codColLocation, codCol);
    glm::mat4 bgMatrix = resizeMatrix;
    glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &bgMatrix[0][0]);
    glDrawArrays(GL_QUADS, 4, 4);

    //textura mesei
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tableTextureId);

    codCol = 2; //textura mesei (codCol = 2)
    glUniform1i(codColLocation, codCol);

    //masa incepe de la (0,0) pana la (a,GROUND_Y)
    glm::mat4 tableMatrix = resizeMatrix;
    glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &tableMatrix[0][0]);
    glDrawArrays(GL_QUADS, 10, 4); // Vertex-urile 10-13 sunt pentru masA

    // Linie de demarca?ie la GROUND_Y (op?ional)
    codCol = 0;
    glUniform1i(codColLocation, codCol);
    glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 8, 2);

    //texturare dominouri
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dominoTextureId);

    for (int i = 0; i < NUM_DOMINOS; i++) {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(dominos[i].pivotX, dominos[i].pivotY, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, dominos[i].angle, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 finalMatrix = resizeMatrix * modelMatrix;
        glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &finalMatrix[0][0]);

        codCol = 1; //folosesc textura domino
        glUniform1i(codColLocation, codCol);
        glDrawArrays(GL_QUADS, 0, 4);

        // Contur domino
        codCol = 0;
        glUniform1i(codColLocation, codCol);
        glLineWidth(2.5f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glutSwapBuffers();
}

void TimerFunction(int value) //~60 fps
{
    float deltaTime = 0.016f;
    time_elapsed += deltaTime;

    UpdatePhysics(deltaTime);
    glutPostRedisplay();
    glutTimerFunc(16, TimerFunction, 0);
}

void KeyboardFunction(unsigned char key, int x, int y)
{
    if (!animationStarted) {
        animationStarted = true;
        dominos[0].isFalling = true;
        dominos[0].angularVelocity = -1.0f; // impuls initial spre dreapta (negative -> clockwise)
    }

    if (key == ' ' || key == 13) { // SPACE sau ENTER
        if (!animationStarted) {
            animationStarted = true;
            dominos[0].isFalling = true;
            dominos[0].angularVelocity = 1.0f; // Impuls initial mai puternic
        }
    }
    else if (key == 'r' || key == 'R') { // RESET
        animationStarted = false;
        time_elapsed = 0.0f;
        for (int i = 0; i < NUM_DOMINOS; i++) {
            dominos[i].angle = 0.0f;
            dominos[i].angularVelocity = 0.0f;
            dominos[i].isFalling = false;
            dominos[i].hasHitNext = false;
        }
        glutPostRedisplay();
    }
    else if (key == 27) { // ESC
        exit(0);
    }
}
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(winWidth, winHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Domino 2D - Apasa SPACE pentru start, R pentru reset");

    glewInit();

    Initialize();
    glutDisplayFunc(RenderFunction);
    glutKeyboardFunc(KeyboardFunction);
    glutTimerFunc(16, TimerFunction, 0);
    glutCloseFunc(Cleanup);
    glutMainLoop();

    return 0;
}