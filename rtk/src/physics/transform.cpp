//
// Created by fatih on 1/19/19.
//

#include <rtk/physics/transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>


namespace rtk
{
    void transform::translate(const glm::vec3& dis, rtk::space s)
    {
        switch (s) {
            case space::self:
                m_pos += rot * dis;
                break;
            case space::world:
                m_pos += dis;
                break;
        }
        set_local_dirty();
    }

    void transform::rotate(const glm::quat& q, space s)
    {
        if (s == space::world)
        {
            rot = q * rot;
        }
        else
        {
            rot *= q;
        }

        rot = glm::normalize(rot);
        set_local_dirty();
    }

    void transform::rotate(const glm::vec3& euler, space s)
    {
        auto around_x = glm::angleAxis(glm::radians(euler.x), glm::vec3(1, 0, 0));
        auto around_y = glm::angleAxis(glm::radians(euler.y), glm::vec3(0, 1, 0));
        auto around_z = glm::angleAxis(glm::radians(euler.z), glm::vec3(0, 0, 1));
        rotate(around_x * around_y * around_z, s);
    }

    glm::mat4 transform::get_local_mat4() const
    {
        if (m_local_mat_dirty)
        {
            auto s = glm::scale(scale);
            auto r = glm::toMat4(rot);
            auto t = glm::translate(m_pos);
            m_local_mat = t * r * s;
            m_local_mat_dirty = false;
        }
        return m_local_mat;
    }

    glm::mat4 transform::get_world_mat4() const
    {
        if (m_parent == nullptr)
        {
            return get_local_mat4();
        }

        if (m_world_mat_dirty)
        {
            m_world_mat = m_parent->get_world_mat4() * get_local_mat4();
            m_world_mat_dirty = false;
        }

        return m_world_mat;
    }

    void transform::set_world_dirty()
    {
        m_world_mat_dirty = true;
        for (auto& child_p : m_children)
        {
            child_p->set_world_dirty();
        }
    }

    void transform::set_parent(const std::shared_ptr<transform> &t) {
        m_parent = t;
        t->m_children.push_back(this->shared_from_this());
        set_world_dirty();
    }

    void transform::set_scale(const glm::vec3& s)
    {
        scale = s;
        set_local_dirty();
    }

    glm::vec3 transform::get_up() const
    {
        return get_world_mat4() * glm::vec4(0, 1, 0, 0);
    }

    glm::vec3 transform::get_forward() const
    {
        return get_world_mat4() * glm::vec4(0, 0, 1, 0);
    }

    glm::vec3 transform::get_right() const
    {
        return get_world_mat4() * glm::vec4(1, 0, 0, 0);
    }

    void transform::set_rotation(const glm::quat& q)
    {
        rot = q;
        set_local_dirty();
    }

    void transform::set_position(const glm::vec3& p)
    {
        m_pos = p;
        set_local_dirty();
    }

    glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest){
        using namespace glm;
        start = normalize(start);
        dest = normalize(dest);

        float cosTheta = dot(start, dest);
        vec3 rotationAxis;

        if (cosTheta < -1 + 0.1f){
            // special case when vectors in opposite directions:
            // there is no "ideal" rotation axis
            // So guess one; any will do as long as it's perpendicular to start
            rotationAxis = cross(vec3(0.0f, 0.0f, 1.0f), start);
            if (length2(rotationAxis) < 0.01 ) // bad luck, they were parallel, try again!
                rotationAxis = cross(vec3(1.0f, 0.0f, 0.0f), start);

            rotationAxis = normalize(rotationAxis);
            return angleAxis(180.0f, rotationAxis);
        }

        rotationAxis = cross(start, dest);

        float s = sqrt( (1+cosTheta)*2 );
        float invs = 1 / s;

        return quat(
                s * 0.5f,
                rotationAxis.x * invs,
                rotationAxis.y * invs,
                rotationAxis.z * invs
        );
    }

    glm::quat safeQuatLookAt(
            glm::vec3 const& lookFrom,
            glm::vec3 const& lookTo,
            glm::vec3 const& up,
            glm::vec3 const& alternativeUp)
    {
        glm::vec3  direction       = lookTo - lookFrom;
        float      directionLength = glm::length(direction);

        // Check if the direction is valid; Also deals with NaN
        if(!(directionLength > 0.0001))
            return glm::quat(1, 0, 0, 0); // Just return identity

        // Normalize direction
        direction /= directionLength;

        // Is the normal up (nearly) parallel to direction?
        if(glm::abs(glm::dot(direction, up)) > .9999f) {
            // Use alternative up
            return glm::quatLookAtLH(direction, alternativeUp);
        }
        else {
            return glm::quatLookAtLH(direction, up);
        }
    }

    void transform::look_at(const glm::vec3& to, const glm::vec3& up, const glm::vec3& right)
    {
        using namespace glm;
        auto des = normalize(get_pos() - to);
        //auto rotation = RotationBetweenVectors(get_forward(), des);
        //set_rotation(quatLookAt(des, get_up()));
        set_rotation(safeQuatLookAt(get_pos(), to, up, right));
        //this->rotate(rotation, space::self);
    }

    glm::vec3 transform::get_pos() const
    {
        return get_world_mat4() * glm::vec4(0, 0, 0, 1);
    }
}