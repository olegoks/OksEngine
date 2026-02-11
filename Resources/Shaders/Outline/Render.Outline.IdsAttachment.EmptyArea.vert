#version 450


void main()
{
        // Генерация полноэкранного треугольника из 3 вершин
    vec2 outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	gl_Position = vec4(outUV * 2.0f + -1.0f, 0.99f, 1.0f);


}
