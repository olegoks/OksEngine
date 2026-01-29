#version 450


void main()
{
        // Генерация полноэкранного треугольника из 3 вершин
    float x = float((gl_VertexIndex & 1) << 2);
    float y = float((gl_VertexIndex & 2) << 1);
    
    vec2 outUV = vec2(x, y) * 2.0;
    gl_Position = vec4(outUV * 2.0 - 1.0, 0.0, 1.0);

}
