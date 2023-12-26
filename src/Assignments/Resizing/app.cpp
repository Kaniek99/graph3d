//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Application/utils.h"

void SimpleShapeApplication::init() {
    // A utility function that reads the shader sources, compiles them and creates the program object
    auto program = xe::utils::create_program(
            {{GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
             {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}});

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }

    // A vector containing the x,y,z vertex coordinates for the pyramid.
    std::vector<GLfloat> vertices = {
        0.5f, -0.5f, 0.0f, 0.22f, 0.59f, 0.51f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.22f, 0.59f, 0.51f, 1.0f,
        -0.5f, -0.5f, 0.0f, 0.22f, 0.59f, 0.51f, 1.0f,

        -0.5f, -0.5f, 0.0f, 0.69f, 0.64f, 0.59f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.69f, 0.64f, 0.59f, 1.0f,
        -0.5f, 0.5f, 0.0f, 0.69f, 0.64f, 0.59f, 1.0f,

        -0.5f, 0.5f, 0.0f, 0.45f, 0.58f, 0.68f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.45f, 0.58f, 0.68f, 1.0f,
        0.5f, 0.5f, 0.0f, 0.45f, 0.58f, 0.68f, 1.0f,

        0.5f, 0.5f, 0.0f, 0.33f, 0.48f, 0.58f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.33f, 0.48f, 0.58f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.33f, 0.48f, 0.58f, 1.0f,

        0.5f, -0.5f, 0.0f, 0.36f, 0.36f, 0.38f, 1.0f,
        -0.5f, -0.5f, 0.0f, 0.36f, 0.36f, 0.38f, 1.0f,
        -0.5f, 0.5f, 0.0f, 0.36f, 0.36f, 0.38f, 1.0f,
        0.5f, 0.5f, 0.0f, 0.36f, 0.36f, 0.38f, 1.0f
    };

    // Generating the buffer and loading the vertex data into it.
    GLuint VBO_vertices, VBO_indexes;
    glGenBuffers(1, &VBO_vertices); //
    glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::vector<GLushort> indexes = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 12, 14, 15
    };

    // Generating the buffer and loading the indexes data into it.
    glGenBuffers(1, &VBO_indexes);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_indexes);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLushort), indexes.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLuint UBO_modifier;
    float strength = 0.7f;
    std::array<float, 3> color = {1.0f, 1.0f, 1.0f};
    glGenBuffers(1, &UBO_modifier);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO_modifier);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO_modifier);
    glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(GLfloat), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLfloat), &strength);
    glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(GLfloat), 3 * sizeof(GLfloat), &color);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    int w,h;
    std::tie(w,h) = frame_buffer_size();

    aspect_ = (float)w/h;
    fov_ = glm::pi<float>()/4.0;
    near_ = 0.1f;
    far_ = 100.0f;

    P_ = glm::perspective(fov_, aspect_, near_, far_);
    V_ = glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 0), glm::vec3(0, -1, 0));
    M_ = glm::mat4(1.0f);

    glm::mat4 PVM = P_ * V_ * M_;

    glGenBuffers(1, &UBO_PVM_);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, UBO_PVM_);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO_PVM_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // This setups a Vertex Array Object (VAO) that  encapsulates
    // the state of all vertex buffers needed for rendering
    glGenVertexArrays(1, &vao_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);

    // This indicates that the data for attribute 0 should be read from a vertex buffer.
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    // and this specifies how the data is layout in the buffer.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_indexes);
    glBindVertexArray(0);
    //end of vao "recording"

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // Setting the background color of the rendering window,
    // I suggest not to use white or black for better debuging.
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    // This setups an OpenGL viewport of the size of the whole rendering window.
    glViewport(0, 0, w, h);

    glUseProgram(program);
}

//This functions is called every frame and does the actual rendering.
void SimpleShapeApplication::frame() {
    auto PVM = P_ * V_ * M_;
    glBindBuffer(GL_UNIFORM_BUFFER, UBO_PVM_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Binding the VAO will setup all the required vertex buffers.
    glBindVertexArray(vao_);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, nullptr);
    glBindVertexArray(0);
}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    glViewport(0, 0, w, h);
    aspect_ = (float) w / h;
    P_ = glm::perspective(fov_, aspect_, near_, far_);
}