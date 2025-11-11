# Domino Simulation
This is a code wirtten in C++ using the *OpenGL* library. It simulates multiple dominos falling onto each other with a complete physics system.
It's a project for the course _Computer Graphics_ and was made with [Ivan Emil-Ovidiu](https://github.com/Ivan-Ovidiu).

# Controls 
- **SPACE** begins the simulation
- **R** restarts the simulation

# Demo 
![demo](https://github.com/user-attachments/assets/3c3d4041-4629-4b7d-8abf-187b1fae1fa7)





# Implementation Details
- Each domino is an instance of the _class Domino_
```
struct Domino {
    float angle;           // unghi de rotatie (radiani)
    float angularVelocity; // viteza unghiulara
    bool isFalling;        // daca este in cadere
    float posX;            // pozitie X (coltul stanga-jos)
    float pivotX;          // punct de rotatie X (colt stanga-jos)
    float pivotY;          // punct de rotatie Y (jos)
    bool hasHitNext;       // daca a lovit deja urmatorul
};
```
- _UpdatePhysics_ is the function that is caled every frame inside _TimerFunction_ 
```
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
```
  Inside this function, for each domino we calculate the projection of the top right corner and the center of mass. After that we calculate the torque with the formula
  ```
  Torque = Gravity * Center of mass
  ```
  After this we check for collisions using the functions _CheckCollision_ and _GetTopRightCorner_
  ```
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
```

```
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
```
# Special Mentions
This code is original because we are using for the table texture a photo we took of a table from the coffee shop [ART-AN-TE](https://share.google/qH3TExDIjlD1EwQg3) where we worked side by side developing the project.



























