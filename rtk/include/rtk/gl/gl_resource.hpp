//
// Created by fatih on 1/28/19.
//

#pragma once

#include <algorithm>
#include <glad/glad.h>
#include <type_traits>

namespace rtk::gl
{
    class no_copy{};

    template <class IdT, class CopyT, class DelT>
    class object : private DelT, private CopyT
    {
    private:

        explicit object(IdT id) noexcept : m_handle{id} {}

        object(object&& rhs) noexcept : m_handle(std::exchange(rhs.m_handle, 0)) {}

        object(const object& rhs)
            : m_handle(this->copy(rhs.m_handle))
        {
            static_assert(!std::is_same<CopyT, no_copy>{});
        }

        object& operator=(object&& rhs) noexcept
        {
            m_handle = std::exchange(rhs.m_handle, 0);
            return *this;
        }

        object& operator=(object obj)
        {
            swap(*this, obj);
            return *this;
        }

        void swap(object& rhs) noexcept
        {
            using std::swap;
            swap(m_handle, rhs.m_handle);
        }

        friend void swap(object& a, object& b) noexcept
        {
            a.swap(b);
        }

        IdT get_id() const {
            return m_handle;
        }

        ~object()
        {
            (*static_cast<DelT*>(this))(m_handle);
        }

    public:
        IdT m_handle;
    };

    struct texture_deleter
    {
        void operator()(GLuint id)
        {
            glDeleteTextures(1, &id);
        }
    };

    using tex_id = object<GLuint, no_copy, texture_deleter>;
}