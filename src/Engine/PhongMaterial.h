#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "Application/utils.h"
#include "Material.h"

namespace xe {
    class PhongMaterial : public Material {
    public:
        glm::vec4 Kd;
        glm::vec4 Ka;
        glm::vec4 Ks;
        GLuint map_Kd;
        const GLuint map_Kd_unit;
        float Ns;

        PhongMaterial(const glm::vec4 color, GLuint texture, GLuint texture_unit) : Kd(color), map_Kd(texture),
                                                                                    map_Kd_unit(0) {}

        PhongMaterial(const glm::vec4 color, GLuint texture) : PhongMaterial(color, texture, 0) {}

        PhongMaterial(const glm::vec4 color) : PhongMaterial(color, 0) {}

        void bind();

        static void init();

        static GLuint program() { return shader_; }

        GLuint texture() const { return map_Kd; }

        void set_texture(GLuint texture) { map_Kd = texture; }

    private:
        static GLuint shader_;
        static GLuint color_uniform_buffer_;
        static GLint uniform_map_Kd_location_;
    };

}
