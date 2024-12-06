#version 330 core

in vec2 uvCoord;

uniform sampler2D tex;
uniform bool useInvert;
uniform bool useBlur;
uniform vec2 texSize; //width, height

out vec4 fragColor;

void main() {

    fragColor = vec4(1);

    vec4 textureColor = texture(tex, uvCoord);

    if (useBlur) {
            vec2 blurSize = 1.0 / texSize;
            vec4 sum = vec4(0.0);

            //5x5
            for (int x = -2; x <= 2; x++) {
                for (int y = -2; y <= 2; y++) {
                    vec2 offset = vec2(float(x), float(y)) * blurSize; //offset in terms of tex coords
                    sum += texture(tex, uvCoord + offset); //uvCoord is the current pixel's tex coordinate, offset is the position of the neighboring tex pixel
                }
            }

            textureColor = sum / 25.0;
    }

    if (useInvert) {
        textureColor = vec4(1.0 - textureColor.rgb, textureColor.a);
    }

    fragColor = textureColor;
}
