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

#include "spdlog/spdlog.h"
#include "Application/utils.h"
#include "Engine/Mesh.h"
#include "Engine/ColorMaterial.h"
#include "Engine/PhongMaterial.h"
#include "Engine/mesh_loader.h"

void SimpleShapeApplication::init() {
    // A utility function that reads the shader sources, compiles them and creates the program object
    auto program = xe::utils::create_program(
            {{GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
             {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}});

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }

    // xe::ColorMaterial::init();
    xe::PhongMaterial::init();

    xe::PointLight light(
        // glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 0.3f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        1.0f,
        1.0f
    );

    add_light(light);
    // add_light(xe::PointLight(glm::vec3(1.0f, 1.0f, 0.3f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 1.0f));
    add_ambient(glm::vec3(0.9f,0.3f,0.6f));

    glGenBuffers(1, &lightBuffer_);
    glBindBuffer(GL_UNIFORM_BUFFER, lightBuffer_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec3) + sizeof(GLuint) + sizeof(xe::PointLight)*n_p_lights_, nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, lightBuffer_);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    set_camera(new Camera);
    set_controler(new CameraControler(camera()));

    // A vector containing the x,y,z vertex coordinates for the pyramid.
    std::vector<GLfloat> vertices = {
        0.5f, -0.5f, 0.0f, 0.5f, 0.809f,
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 0.191f, 0.5f,

        -0.5f, -0.5f, 0.0f, 0.191f, 0.5f,
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 0.5f, 0.191f,

        -0.5f, 0.5f, 0.0f, 0.5f, 0.191f,
        0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 0.809f, 0.5f,

        0.5f, 0.5f, 0.0f, 0.809f, 0.5f,
        0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.5f, 0.809f,

        0.5f, -0.5f, 0.0f, 0.5f, 0.809f,
        -0.5f, -0.5f, 0.0f, 0.191f, 0.5f,
        -0.5f, 0.5f, 0.0f, 0.5f, 0.191f,

        0.5f, -0.5f, 0.0f, 0.5f, 0.809f,
        -0.5f, 0.5f, 0.0f, 0.5f, 0.191f,
        0.5f, 0.5f, 0.0f, 0.809f, 0.5f
    };

    std::vector<GLushort> indexes = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17
    };

    auto pyramid = new xe::Mesh;
    pyramid = xe::load_mesh_from_obj(std::string(ROOT_DIR) + "/Models/square.obj",
                                          std::string(ROOT_DIR) + "/Models");

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
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4) + 3 * sizeof(glm::vec4), nullptr, GL_STATIC_DRAW);
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
    glm::mat4 M = glm::mat4(1.0f);
    auto VM = camera_->view()*M;
    auto R = glm::mat3(VM);
    auto N = glm::mat3(glm::cross(R[1], R[2]), glm::cross(R[2], R[0]), glm::cross(R[0], R[1]));
    glBindBuffer(GL_UNIFORM_BUFFER, UBO_PVM_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &VM[0]);
    // According to the std140 we send each column separately. Each column must be separated by a float.
    for (int column = 0; column < 3; column++) {
        auto offset = column * (sizeof(glm::vec3) + sizeof(float));
        auto columnOffset = 2 * sizeof(glm::mat4) + offset;
        glBufferSubData(GL_UNIFORM_BUFFER, columnOffset, sizeof(glm::vec3), &N[column][0]);
        columnOffset += sizeof(glm::vec3);
        glBufferSubData(GL_UNIFORM_BUFFER, columnOffset, sizeof(float), nullptr);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, lightBuffer_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec3), &ambient_);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::vec3), sizeof(GLuint), &n_p_lights_);

    for (GLuint i = 0; i < n_p_lights_; i++) {
        xe::PointLight& light = p_lights_[i];
        light.position_in_vs = glm::vec3(VM * glm::vec4(light.position_in_ws, 1.0f));

        auto lightOffset = sizeof(glm::vec3) + sizeof(GLuint) + i * sizeof(xe::PointLight);
        glBufferSubData(GL_UNIFORM_BUFFER, lightOffset, sizeof(xe::PointLight), &light);
    }

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