#pragma once
#include <glm/fwd.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace har
{
    struct transform_t
    {
    public:
        transform_t() = default;

        transform_t(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
            : position(position)
            , rotation(rotation)
            , scale(scale)
        {
        }

        transform_t(const glm::mat4& matrix)
        {
            glm::vec3 skew;
			glm::vec4 perspective;
			glm::quat orientation;
			glm::decompose(matrix, scale, orientation, position, skew, perspective);
			rotation = glm::degrees(glm::eulerAngles(orientation));
        }

    public:
        glm::mat4 get_transform_matrix() const
        {
            glm::mat4 matrix(1.f);

            matrix = glm::translate(matrix, position);

            glm::quat quaternion = glm::quat(glm::radians(rotation));
            matrix *= glm::mat4_cast(quaternion);

            matrix = glm::scale(matrix, scale);

            return matrix;
        }

    public:
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
    };
}