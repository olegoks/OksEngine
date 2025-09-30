
mat4 RTS_to_mat4_optimized(vec3 translation, vec4 rotation, vec3 scale) {
    float w = rotation.x;
    float x = rotation.y;
    float y = rotation.z;
    float z = rotation.w;
    
    // Предварительные вычисления
    float x2 = x * x;
    float y2 = y * y;
    float z2 = z * z;
    float xy = x * y;
    float xz = x * z;
    float yz = y * z;
    float wx = w * x;
    float wy = w * y;
    float wz = w * z;
    
    // Создаём матрицу напрямую
    return mat4(
        // Первый столбец
        scale.x * (1.0 - 2.0 * (y2 + z2)),
        scale.x * (2.0 * (xy + wz)),
        scale.x * (2.0 * (xz - wy)),
        0.0,
        
        // Второй столбец
        scale.y * (2.0 * (xy - wz)),
        scale.y * (1.0 - 2.0 * (x2 + z2)),
        scale.y * (2.0 * (yz + wx)),
        0.0,
        
        // Третий столбец
        scale.z * (2.0 * (xz + wy)),
        scale.z * (2.0 * (yz - wx)),
        scale.z * (1.0 - 2.0 * (x2 + y2)),
        0.0,
        
        // Четвертый столбец
        translation.x,
        translation.y,
        translation.z,
        1.0
    );
}

// #define ASSERT_MSG(condition, message)\
//     if(!condition) {  debugPrintfEXT("ASSERT_MSG(condition, %s)", message) }