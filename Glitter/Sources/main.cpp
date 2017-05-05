#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/ext.hpp"          // to print vectors and matrices
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>

#define RED 1.0f, 0.0f, 0.0f            // define macros for convenience
#define BLUE 0.0f, 0.0f, 1.0f
#define GREEN 0.0f, 1.0f, 0.0f
#define ORANGE 1.0F, 0.6F, 0.0F
#define PURPLE 1.0f, 0.0f, 1.0f
#define YELLOW 1.0f, 1.0f, 0.0f

// device coordinates
#define LEFT -0.5f
#define TOP 0.5f
#define RIGHT 0.5f
#define BOTTOM -0.5f
#define MIDDLE 0.0f
#define FRONT 0.5f
#define BACK -0.5f

// texture coordinates
#define TOP_LEFT 0.0f, 1.0f
#define BOTTOM_LEFT 0.0f, 0.0f
#define TOP_RIGHT 1.0f, 1.0f
#define BOTTOM_RIGHT 1.0f, 0.0f

const GLchar* vertexSource =
"#version 150 core\n"             // glsl version
"in vec3 position;"               // expects 2 values for position
"in vec3 color;"                  // and 3 values for color
"in vec2 texCoord;"								// and 2 texture coordinates
"out vec3 Color;"                 // will pass color along pipeline
"out vec2 TexCoord;"							// and texture coordinates
"uniform mat4 model;"             // uniform = the same for all vertices
"uniform mat4 view;"
"void main()"
"{"
"    Color = color;"              // just pass color along without modifying it
"    TexCoord = texCoord;"
"    gl_Position = view * model * vec4(position, 1.0);"   // gl_Position is special variable for final position
"}";                                                    // must be in homogeneous coordinates -- put in 0 for z and 1 for w
// multiply by model matrix to transform
const GLchar* fragmentSource =
"#version 150 core\n"
"in vec3 Color;"
"in vec2 TexCoord;"
"out vec4 outColor;"
"uniform sampler2D theTexture;"
"void main()"
"{"
"    outColor = texture(theTexture, TexCoord);"
"}";

// vertex data
GLfloat vertices [] = {
  LEFT, TOP, FRONT, RED, TOP_LEFT,       // front
  LEFT, BOTTOM, FRONT, RED, BOTTOM_LEFT,
  RIGHT, BOTTOM, FRONT, RED, BOTTOM_RIGHT,
  
  LEFT, TOP, FRONT, RED, TOP_LEFT,
  RIGHT, BOTTOM, FRONT, RED, BOTTOM_RIGHT,
  RIGHT, TOP, FRONT, RED, TOP_RIGHT,
 
	 LEFT, TOP, BACK, BLUE, TOP_LEFT,       // left
  LEFT, BOTTOM, BACK, BLUE, BOTTOM_LEFT,
  LEFT, BOTTOM, FRONT, BLUE, BOTTOM_RIGHT,
  
  LEFT, TOP, BACK, BLUE, TOP_LEFT,
  LEFT, BOTTOM, FRONT, BLUE, BOTTOM_RIGHT,
  LEFT, TOP, FRONT, BLUE, TOP_RIGHT,
  
  RIGHT, TOP, BACK, GREEN, TOP_LEFT,     // back
  RIGHT, BOTTOM, BACK, GREEN, BOTTOM_LEFT,
  LEFT, BOTTOM, BACK, GREEN, BOTTOM_RIGHT,
  
  RIGHT, TOP, BACK, GREEN, TOP_LEFT,
  LEFT, BOTTOM, BACK, GREEN, BOTTOM_RIGHT,
  LEFT, TOP, BACK, GREEN, TOP_RIGHT,
  
  RIGHT, TOP, FRONT, ORANGE, TOP_LEFT,     // right
  RIGHT, BOTTOM, FRONT, ORANGE, BOTTOM_LEFT,
  RIGHT, BOTTOM, BACK, ORANGE, BOTTOM_RIGHT,
 
  RIGHT, TOP, FRONT, ORANGE, TOP_LEFT,
  RIGHT, BOTTOM, BACK, ORANGE, BOTTOM_RIGHT,
  RIGHT, TOP, BACK, ORANGE, TOP_RIGHT,
  
  LEFT, TOP, BACK, PURPLE, TOP_LEFT,   // top
  LEFT, TOP, FRONT, PURPLE, BOTTOM_LEFT,
  RIGHT, TOP, FRONT, PURPLE, BOTTOM_RIGHT,
  
  LEFT, TOP, BACK, PURPLE, TOP_LEFT,
  RIGHT, TOP, FRONT, PURPLE, BOTTOM_RIGHT,
  RIGHT, TOP, BACK, PURPLE, TOP_RIGHT,
  
  LEFT, BOTTOM, FRONT, YELLOW, TOP_LEFT,   // bottom
  LEFT, BOTTOM, BACK, YELLOW, BOTTOM_LEFT,
  RIGHT, BOTTOM, BACK, YELLOW, BOTTOM_RIGHT,
  
  LEFT, BOTTOM, FRONT, YELLOW, TOP_LEFT,
  RIGHT, BOTTOM, BACK, YELLOW, BOTTOM_RIGHT,
  RIGHT, BOTTOM, FRONT, YELLOW, TOP_RIGHT,
};

// callback for keyboard input
void key_callback(GLFWwindow* mWindow, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_X && action == GLFW_PRESS) glfwSetWindowShouldClose(mWindow, true);
}

int main(int argc, char * argv[]) {
  // Load GLFW and Create a Window
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
  auto mWindow = glfwCreateWindow(600, 600, "CISC3620", nullptr, nullptr);
  // Check for Valid Context
  if (mWindow == nullptr) {
    fprintf(stderr, "Failed to Create OpenGL Context");
    return EXIT_FAILURE;
  }
  
  // callbacks
  glfwSetKeyCallback(mWindow, key_callback);
  
  // Create Context and Load OpenGL Functions
  glfwMakeContextCurrent(mWindow);
  gladLoadGL();
  fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));
  
  glEnable(GL_DEPTH_TEST);
  
  // Create Vertex Array Object: this will store all the information about the vertex data that we are about to specify
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  
  // Create a Vertex Buffer Object and copy the vertex data to it
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  
  // Create and compile the vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexSource, NULL);
  glCompileShader(vertexShader);
  // Create and compile the fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
  glCompileShader(fragmentShader);
  // Link the vertex and fragment shader into a shader program
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glBindFragDataLocation(shaderProgram, 0, "outColor");
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);
  
  // Specify the layout of the vertex data
  // position
  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);  // attribute location, # values, value type, normalize?, stride, offset
  // color
  GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
  glEnableVertexAttribArray(colAttrib);
  glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	// texture coords
	GLint texAttrib = glGetAttribLocation(shaderProgram, "texCoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
  
  // model matrix
  GLint modelTransform = glGetUniformLocation(shaderProgram, "model");
  glm::mat4 rotate_model = glm::rotate(glm::mat4(1.0f), 15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
  
  // view matrix
  glm::mat4 ortho_model = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
  GLint viewTransform = glGetUniformLocation(shaderProgram, "view");
  glUniformMatrix4fv(viewTransform, 1, GL_FALSE, glm::value_ptr(ortho_model));

	// load texture
	int w, h, comp;
	unsigned char* image = stbi_load("bunny.jpg", &w, &h, &comp, STBI_rgb);
	if (image == nullptr) std::cout << stbi_failure_reason() << std::endl;

	// generate texture
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
  // Rendering Loop
  float r = 0.1f;
  while (glfwWindowShouldClose(mWindow) == false) {
    
    // Background Fill Color
    glClearColor(0.9f, 0.9f, 0.9f, 0.8f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glm::mat4 model = glm::rotate(rotate_model, r+=0.01, glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(modelTransform, 1, GL_FALSE, glm::value_ptr(model));

    // draw triangle
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices)/sizeof(vertices[0]));
    
    // Flip Buffers and Draw
    glfwSwapBuffers(mWindow);
    glfwPollEvents();
  }   glfwTerminate();
  
  // clean up
  glDeleteProgram(shaderProgram);
  glDeleteShader(fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
  
  return EXIT_SUCCESS;
}
