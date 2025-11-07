#version 330 core

in vec4 ex_Color;
in vec2 ex_TexCoord;

uniform int codCol; // 0 = color, 1 = domino texture, 2 = table texture
uniform sampler2D dominoTexture;
uniform sampler2D tableTexture;

out vec4 out_Color;

void main(void)
{
    if (codCol == 1) {
        out_Color = texture(dominoTexture, ex_TexCoord);
    } else if (codCol == 2) {
        out_Color = texture(tableTexture, ex_TexCoord);
    } else {
        // fallback to color
        out_Color = ex_Color;
    }
}