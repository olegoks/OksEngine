#include <GLFW\glfw3.h>


int main() {

    if (!glfwInit()) {
        // Handle initialization failure
        return -1;
    }

    GLFWwindow* window;

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    while (!glfwWindowShouldClose(window))
    {

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
}

