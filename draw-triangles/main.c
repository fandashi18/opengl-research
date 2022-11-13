/*
 * gcc main.c -o main -lglfw -lGL
 */
#define GL_GLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <GL/gl.h>
#include <GL/glcorearb.h>
#include <GLFW/glfw3.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#define handle_error(message) \
    perror(message);          \
    exit(EXIT_FAILURE);

void init();
void displayTriangles();

void init()
{
    GLuint vArrays[1];
    glGenVertexArrays(1, vArrays);
    glBindVertexArray(vArrays[0]);

    GLuint vBuffers[1];
    glCreateBuffers(1, vBuffers);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffers[0]);

    GLfloat vertices[6][2] = {
        {-0.90f, -0.90f}, {0.85f, -0.90f}, {-0.90f, 0.85f}, {0.90f, -0.85f}, {0.90f, 0.90f}, {-0.85f, 0.90f}};

    glNamedBufferStorage(vBuffers[0], sizeof(vertices), vertices, 0);

    int fd = open("/home/my/opengl/glsl/triangles.vert", O_RDONLY);
    if (fd == -1)
    {
        handle_error("Can not open triangles glsl file");
    }
    struct stat fInfo;
    if (fstat(fd, &fInfo) == -1)
    {
        close(fd);
        handle_error("Can get info of triangles glsl file");
    }
    GLchar vData[fInfo.st_size + 1];
    int endPos = 0;
    char cBuffer[1024];
    int cSize = 0;

    do
    {
        cSize = read(fd, cBuffer, 1024);
        if (cSize == -1)
        {
            close(fd);
            handle_error("failed to read glsl");
        }

        if (cSize == 0)
        {
            break;
        }

        for (int i = 0; i < cSize; i++)
        {
            vData[endPos++] = cBuffer[i];
        }

    } while (1);
    vData[fInfo.st_size] = 0;

    close(fd);

    GLuint vProgram = glCreateProgram();
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar *cvData = vData;
    glShaderSource(vShader, 1, &cvData, NULL);
    glCompileShader(vShader);
    GLuint vsCompiled;
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &vsCompiled);
    if (!vsCompiled)
    {
        GLuint length;
        glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &length);
        GLchar log[length + 1];
        glGetInfoLogARB(vShader, length, NULL, log);
        handle_error(log);
    }

    glAttachShader(vProgram, vShader);
    glUseProgram(vProgram);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
}

void displayTriangles()
{
    static const float black[] = {0.0f, 1.0f, 0.0f, 0.0f};

    glClearBufferfv(GL_COLOR, 0, black);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

int main(int argc, char const *argv[])
{
    glfwInit();
    GLFWwindow *window = glfwCreateWindow(1640, 1480, "Triangles", NULL, NULL);
    glfwMakeContextCurrent(window);

    init();

    while (!glfwWindowShouldClose(window))
    {
        displayTriangles();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
