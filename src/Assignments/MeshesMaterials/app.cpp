//
// Created by pbialas on 25.09.2020.
//

#include "app.h"
// #include "camera.h"

#include <iostream>
#include <vector>
#include <tuple>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Application/utils.h"
#include "Engine/Mesh.h"
#include "Engine/Material.h"

void SimpleShapeApplication::init() {
    // A utility function that reads the shader sources, compiles them and creates the program object
    auto program = xe::utils::create_program(
            {{GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
             {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}});

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }

    xe::ColorMaterial::init();

    set_camera(new Camera);
    set_controler(new CameraControler(camera()));

    // A vector containing the x,y,z vertex coordinates for the pyramid.
    std::vector<GLfloat> vertices = {
        0.5f, -0.5f, 0.0f,
        0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,

        -0.5f, -0.5f, 0.0f,
        0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.0f,

        -0.5f, 0.5f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.0f,

        0.5f, 0.5f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f,

        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,

        0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f
    };

    std::vector<GLushort> indexes = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17
    };

    auto pyramid = new xe::Mesh;
    pyramid->allocate_vertex_buffer(vertices.size() * sizeof(GLfloat), GL_STATIC_DRAW);
    pyramid->load_vertices(0, vertices.size() * sizeof(GLfloat), vertices.data());
    pyramid->vertex_attrib_pointer(0, 3, GL_FLOAT, 3 * sizeof(GLfloat), 0);
    // pyramid->vertex_attrib_pointer(1, 4, GL_FLOAT, 7 * sizeof(GLfloat), 3);
    pyramid->allocate_index_buffer(indexes.size() * sizeof(GLfloat), GL_STATIC_DRAW);
    pyramid->load_indices(0, indexes.size() * sizeof(GLfloat), indexes.data());

    pyramid->add_submesh(0, 3, new xe::ColorMaterial({0.22f, 0.59f, 0.51f, 1.0f}));
    pyramid->add_submesh(3, 6, new xe::ColorMaterial({0.69f, 0.64f, 0.59f, 1.0f}));
    pyramid->add_submesh(6, 9, new xe::ColorMaterial({0.45f, 0.58f, 0.68f, 1.0f}));
    pyramid->add_submesh(9, 12, new xe::ColorMaterial({0.33f, 0.48f, 0.58f, 1.0f}));
    pyramid->add_submesh(12, 18, new xe::ColorMaterial({0.36f, 0.36f, 0.38f, 1.0f}));
    add_submesh(pyramid);

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

    GLfloat aspect = (float)w/h;
    GLfloat fov = glm::pi<float>()/4.0;
    GLfloat near = 0.1f;
    GLfloat far = 100.0f;

    camera_->perspective(fov, aspect, near, far);

    camera_->look_at(glm::vec3(0, 0, 2), glm::vec3(0, 0, 0), glm::vec3(0, -1, 0));

    glGenBuffers(1, &UBO_PVM_);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, UBO_PVM_);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO_PVM_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Setting the background color of the rendering window,
    // I suggest not to use white or black for better debuging.
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    // This setups an OpenGL viewport of the size of the whole rendering window.
    glViewport(0, 0, w, h);

    glUseProgram(program);
}

//This functions is called every frame and does the actual rendering.
void SimpleShapeApplication::frame() {
    auto PVM = camera_->projection() * camera_->view();
    glBindBuffer(GL_UNIFORM_BUFFER, UBO_PVM_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    for (auto m: meshes_)
        m->draw();
}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    glViewport(0, 0, w, h);
    camera_->set_aspect((float) w / h);
}

void SimpleShapeApplication::scroll_callback(double xoffset, double yoffset) {
         Application::scroll_callback(xoffset, yoffset);
         camera()->zoom(yoffset / 30.0f);
}

void SimpleShapeApplication::mouse_button_callback(int button, int action, int mods) {
    Application::mouse_button_callback(button, action, mods);

    if (controler_) {
        double x, y;
        glfwGetCursorPos(window_, &x, &y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            controler_->LMB_pressed(x, y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
            controler_->LMB_released(x, y);
    }

}

void SimpleShapeApplication::cursor_position_callback(double x, double y) {
    Application::cursor_position_callback(x, y);
    if (controler_) {
        controler_->mouse_moved(x, y);
    }
}