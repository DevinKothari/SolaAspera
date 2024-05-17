#pragma once
#include "Object3D.h"
#include "Animation.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @brief Animates an object to orbit around a specified center point.
 */
class OrbitalAnimation : public Animation {
private:
    glm::vec3 m_center;    // Center of orbit
    glm::vec3 m_axis;      // Axis of orbit
    float m_duration;      // Duration of one full orbit
    float m_elapsedTime;   // Elapsed time tracker
    glm::vec3 m_initialRelativePosition; // Initial relative position of the moon

    void applyAnimation(float_t dt) override {
        // Update the elapsed time
        m_elapsedTime += dt;

        // Calculate the current angle of rotation based on elapsed time and duration
        float angle = (m_elapsedTime / m_duration) * glm::two_pi<float>();

        // Calculate the new position of the object
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, m_axis);
        glm::vec3 newPosition = glm::vec3(rotation * glm::vec4(m_initialRelativePosition, 1.0f)); // Apply rotation
        object().setPosition(m_center + newPosition); // Update position relative to center
    }

public:
    OrbitalAnimation(Object3D& object, float_t duration, const glm::vec3& center, const glm::vec3& axis) :
        Animation(object, duration), m_center(center), m_axis(axis), m_duration(duration), m_elapsedTime(0.0f) {
        m_initialRelativePosition = object.getPosition() - center;
    }
};
