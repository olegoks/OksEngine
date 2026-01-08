#extension GL_ARB_gpu_shader_int64 : enable

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


vec4 quat_slerp(vec4 q1, vec4 q2, float t) {
    // Нормализуем входные кватернионы
    q1 = normalize(q1);
    q2 = normalize(q2);
    
    // Вычисляем косинус угла между кватернионами
    float cos_theta = dot(q1, q2);
    
    // Корректируем для кратчайшего пути
    if (cos_theta < 0.0) {
        q2 = -q2;
        cos_theta = -cos_theta;
    }
    
    // Если кватернионы очень близки, используем линейную интерполяцию
    const float epsilon = 0.0001;
    if (cos_theta > 1.0 - epsilon) {
        return normalize(mix(q1, q2, t));
    }
    
    // Вычисляем угол и коэффициенты SLERP
    float theta = acos(cos_theta);
    float sin_theta = sin(theta);
    
    float w1 = sin((1.0 - t) * theta) / sin_theta;
    float w2 = sin(t * theta) / sin_theta;
    
    return w1 * q1 + w2 * q2;
}

bool IsEqual(float first, float second) {
    return abs(first - second) < 0.0001;
}


//debugPrintfEXT:
//%d - only uint(not uint64)
#define ASSERT_FAIL_MSG(message)\
    debugPrintfEXT(message); 


#define ASSERT_FAIL_FMSG_1(message, arg1)\
    debugPrintfEXT(message, arg1); 

#define ASSERT_FAIL_FMSG_2(message, arg1, arg2)\
    debugPrintfEXT(message, arg1, arg2); 

#define ASSERT_MSG(expression, message)             \
    if(!(expression)) {                             \
        debugPrintfEXT(message);                    \
    }   

#define ASSERT_FMSG_1(expression, message, arg1)    \
    if(!(expression)) {                             \
        debugPrintfEXT(message, arg1);               \
    }   


#define ASSERT_FMSG_2(expression, message, arg1, arg2)  \
    if(!(expression)) {                                 \
        debugPrintfEXT(message, arg1, arg2);             \
    }   

#define ASSERT_FMSG_3(expression, message, arg1, arg2, arg3)  \
    if(!(expression)) {                                 \
        debugPrintfEXT(message, arg1, arg2, arg3);             \
    }   


#define LOG_MSG(message)\
    debugPrintfEXT(message); 

#define LOG_FMSG_1(message, arg1)\
    debugPrintfEXT(message, arg1); 

#define LOG_FMSG_2(message, arg1, arg2)\
    debugPrintfEXT(message, arg1, arg2); 

    #define LOG_FMSG_3(message, arg1, arg2, arg3)\
    debugPrintfEXT(message, arg1, arg2, arg3); 

    #define LOG_FMSG_4(message, arg1, arg2, arg3, arg4)\
    debugPrintfEXT(message, arg1, arg2, arg3, arg4); 

#define INVALID_ENTITY_ID uint64_t(-1)


// struct WorldPosition3D {
//     vec4 position_;
// };
// struct WorldRotation3D {
//     vec4 rotation_;
// };
// struct WorldScale3D {
//     vec4 scale_;
// };

// struct ModelNodeEntityIds {
//     uint64_t nodeIds_[512];
// };

// struct ModelEntityIds {
//     uint64_t modelIds_[1024];
// };