#version 330 core

in vec4 ex_Color;
out vec4 out_Color;

uniform int codCol;

void main(void)
{
    if (codCol == 0) {
        // Fundal - gradient bleu
        out_Color = vec4(0.9, 0.95, 1.0, 1.0);
    }
    else if (codCol == 1) {
        //primul domino
        out_Color = vec4(0.9, 0.2, 0.2, 1.0);
    }
    else if (codCol == 2) {
        //celelalte dominouri
        out_Color = vec4(0.3, 0.5, 0.9, 1.0);
    }
    else if (codCol == 3) {
        //contur domino
        out_Color = vec4(0.1, 0.1, 0.1, 1.0);
    }
    else if (codCol == 4) {
        //culoare linie pamant
        out_Color = vec4(0.4, 0.3, 0.2, 1.0);
    }
    else {
        // Default
        out_Color = ex_Color;
    }
}