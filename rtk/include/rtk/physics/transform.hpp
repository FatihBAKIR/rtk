//
// Created by fatih on 1/19/19.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <memory>
#include <rtk/rtk_fwd.hpp>

namespace rtk
{
    enum class space
    {
        self,
        world
    };

    struct vectors
    {
        static inline const glm::vec3 left {1, 0, 0};
        static inline const glm::vec3 right = -left;

        static inline const glm::vec3 forward {0, 0, 1};
        static inline const glm::vec3 back = -forward;

        static inline const glm::vec3 up {0, 1, 0};
        static inline const glm::vec3 down = -up;
    };

    class RTK_PUBLIC transform : public std::enable_shared_from_this<transform>
    {
    public:
        void set_parent(const std::shared_ptr<transform>& t);
        void set_world_dirty();

        transform* get_parent() const {
            return m_parent.get();
        }

        void translate(const glm::vec3& dis, space s = space::self);

        void rotate(const glm::quat& q, space s = space::self);
        void rotate(const glm::vec3& euler, space s = space::self);

        void set_position(const glm::vec3& p);
        void set_scale(const glm::vec3& s);
        void set_rotation(const glm::quat& q);

        glm::mat4 get_world_mat4() const;
        glm::mat4 get_local_mat4() const;

        glm::vec3 get_up() const;
        glm::vec3 get_forward() const;
        glm::vec3 get_right() const;
        glm::vec3 get_pos() const;

        void look_at(const glm::vec3& to);

        const glm::vec3& get_local_pos() const { return m_pos; }
        const glm::vec3& get_local_scale() const { return scale; }
        const glm::quat& get_local_rot() const { return rot; }

        void start()
        {
            if (m_parent)
            {
                set_parent(m_parent);
            }
            set_local_dirty();
        }

    private:
        std::vector<std::shared_ptr<transform>> m_children;
        std::shared_ptr<transform> m_parent;

        glm::vec3 m_pos = {0, 0, 0};
        glm::vec3 scale = {1, 1, 1};
        glm::quat rot = glm::identity<glm::quat>();

        mutable bool m_local_mat_dirty = true;
        mutable glm::mat4 m_local_mat;

        mutable bool m_world_mat_dirty;
        mutable glm::mat4 m_world_mat;

        void set_local_dirty()
        {
            m_local_mat_dirty = true;
            set_world_dirty();
        }
    };
}