#version 450

layout(location = 0) out vec4 outColor;


vec4 idToColor(uint id) {
    // Используем золотое сечение для генерации HUE
    float hue = fract(float(id) * 0.618033988749895);
    
    // HSV to RGB
    vec3 rgb = clamp(
        abs(mod(hue * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0,
        0.0, 1.0
    );
    
    // Делаем цвета насыщенными и яркими
    return vec4(rgb * 0.8 + 0.2, 1.0);
}

void main() {
    outColor = idToColor(1);
}