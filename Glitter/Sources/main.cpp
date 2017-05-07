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
#include <vector>

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
"#version 330 core\n"             // glsl version
"layout (location=0) in vec3 position;"               // expects 2 values for position
"layout (location=1) in vec3 color;"                  // and 3 values for color
"layout (location=2) in vec2 texCoord;"								// and 2 texture coordinates
"layout (location=3) in vec3 tangent;"
"layout (location=4) in vec3 bitangent;"

// output data: interpolated for each fragment
"out vec3 Color;"                 // will pass color along pipeline
"out vec3 FragPos;"								// position in world space (after model transformation)
"out vec2 TexCoord;"							// and texture coordinates
"out vec3 EyeDir_tangentspace;"
"out vec3 LightDir_tangentspace;"

"uniform mat4 model;"             // uniform = the same for all vertices
"uniform mat4 view;"
"uniform vec3 lightPos;"

"void main()"
"{"
"		 FragPos = vec3(model * vec4(position, 1.0f));"

// TODO: transform tangent, bitangent, position, eyedir, lightdir from model space (given) into camera space
// TODO: get camerspace normal from tangent and bitangent crossproduct. Construct TBN camerspace-->tangentspace matrix
// TODO: transform eyedir, lightdir from cameraspace to tangentspace

"    TexCoord = texCoord;"
"    gl_Position = view * model * vec4(position, 1.0);"   // gl_Position is special variable for final position
"}";

const GLchar* fragmentSource =
"#version 330 core\n"
"in vec3 Color;"
"in vec3 FragPos;"
"in vec2 TexCoord;"
"in vec3 EyeDir_tangentspace;"
"in vec3 LightDir_tangentspace;"

"uniform sampler2D textureMap;"
"uniform sampler2D normalMap;"
"uniform vec3 lightColor;"
"uniform vec3 objectColor;"

"out vec4 outColor;"

"void main()"
"{"
		 "vec3 ambientStrength = vec3(0.4, 0.4, 0.4);"
// TODO: get material diffuse color from textureMap (bunny) and normal from normalMap (bricks)
// TODO: calculate diffuse, specular and ambient components using tangentspace eyedir and lightdir and diffuse color and normal from textures
"		 outColor = vec4(diffuse + ambient + specular, 1.0);"
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

void computeTangentBasis(GLfloat* vertices, int nvertices, int stride, std::vector<glm::vec3>& tangents, std::vector<glm::vec3>& bitangents) {
	int i = 0, t=0;
	while (i < nvertices) {
		glm::vec3 v0(vertices[i+0], vertices[i+1], vertices[i+2]); i+=stride; // i+8 will skip to next vertex
		glm::vec3 v1(vertices[i+0], vertices[i+1], vertices[i+2]); i+=stride;
		glm::vec3 v2(vertices[i+0], vertices[i+1], vertices[i+2]); i+=stride;

		glm::vec3 tangent, bitangent;

		std::cout << "triangle number " << t << std::endl;
		// vertices are either in order
		// top/left - bottom/left - bottom/right (even triangles)
		// or top/left - bottom/right - top/right (odd triangles)
		// the x direction (tangents) should be left-right
		// the y direction (bitangents) should be bottom-top
		if (t % 2 == 0) {
			tangent = v2 /* bottom right */ - v1 /* bottom left */;
			bitangent = v0 /* top left */ - v1 /* bottom left */;
		} else {
			tangent = v2 /* top right*/ - v0 /* top left */;
			bitangent = v2 /* top right */ - v1 /* bottom right */;
		}

		for (int j = 0; j < 3; j++) {
			tangents.push_back(tangent);
			bitangents.push_back(bitangent);
		}
		t++;
	}
}

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
  GLuint verticesbuffer;
  glGenBuffers(1, &verticesbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, verticesbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Tangent data
	std::vector<glm::vec3> tangents, bitangents;
	computeTangentBasis(vertices, sizeof(vertices)/sizeof(vertices[0]),
		8, tangents, bitangents);
	GLfloat tangent_arr[tangents.size()*3];
	GLfloat bitangent_arr[bitangents.size()*3];
	for (int i = 0; i < tangents.size(); i++) {
		for (int j = 0; j < 3; j++) {
			tangent_arr[i*3+j] = tangents[i][j];
			bitangent_arr[i*3+j] = bitangents[i][j];
		}
	}
	GLuint tangentbuffer;
	glGenBuffers(1, &tangentbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tangent_arr), tangent_arr, GL_STATIC_DRAW);
	GLuint bitangentbuffer;
	glGenBuffers(1, &bitangentbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bitangent_arr), bitangent_arr, GL_STATIC_DRAW);
  
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
  glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, verticesbuffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);  // attribute location, # values, value type, normalize?, stride, offset
  // color
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	// texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	// tangents
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	// bitangents
	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  
  // model matrix
  GLint modelTransform = glGetUniformLocation(shaderProgram, "model");
  glm::mat4 rotate_model = glm::rotate(glm::mat4(1.0f), 15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
  // view matrix
  glm::mat4 ortho_model = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
  GLint viewTransform = glGetUniformLocation(shaderProgram, "view");
  glUniformMatrix4fv(viewTransform, 1, GL_FALSE, glm::value_ptr(ortho_model));

	// light color, position
	GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
	GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightPosLoc,  1.0f, 1.0f, 1.0f);

	// generate textures
	GLuint textures [2];
	glGenTextures(2, textures);
	// Texture 1 (diffuse color) (bunny)
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	// texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load, create texture
	int w0, h0, comp0;
	unsigned char* image = stbi_load("bunny.jpg", &w0, &h0, &comp0, STBI_rgb);
	if (image == nullptr) std::cout << stbi_failure_reason() << std::endl;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w0, h0, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glUniform1i(glGetUniformLocation(shaderProgram, "textureMap"), 0);
	// Texture 2 (bump map) (brick)
	// TODO: same as first texture
	
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
  glDeleteBuffers(1, &verticesbuffer);
  glDeleteVertexArrays(1, &vao);
  
  return EXIT_SUCCESS;
}
