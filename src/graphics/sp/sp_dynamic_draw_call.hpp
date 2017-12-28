//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2017 SuperTuxKart-Team
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef HEADER_SP_DYNAMIC_DRAW_CALL_HPP
#define HEADER_SP_DYNAMIC_DRAW_CALL_HPP

#include "graphics/sp/sp_mesh_buffer.hpp"
#include "graphics/sp/sp_per_object_uniform.hpp"

#include <IMeshBuffer.h>

#include <array>
#include <cassert>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

using namespace irr;
using namespace scene;

class Material;

namespace SP
{
class SPShader;


class SPDynamicDrawCall : public SPMeshBuffer, public SPPerObjectUniform
{
private:
    core::matrix4 m_trans;

    SPShader* m_shader;

    core::vector2df m_texture_trans;

    scene::E_PRIMITIVE_TYPE m_primitive_type;

    unsigned m_gl_vbo_size = 4;

    int m_update_offset = -1;

    bool m_visible = true;

    bool m_update_trans = false;

    // ------------------------------------------------------------------------
    bool initTextureDyDc();
    // ------------------------------------------------------------------------
    void recreateDynamicVAO();

public:
    SPDynamicDrawCall(scene::E_PRIMITIVE_TYPE pt, SPShader* shader,
                      Material* m);
    // ------------------------------------------------------------------------
    ~SPDynamicDrawCall() {}
    // ------------------------------------------------------------------------
    virtual void draw(DrawCallType dct, int material_id = -1,
                      bool bindless_texture = false) const
    {
#ifndef SERVER_ONLY
        glBindVertexArray(m_vao[0]);
        glDrawArraysInstanced(
            m_primitive_type == EPT_TRIANGLES ? GL_TRIANGLES :
            m_primitive_type == EPT_TRIANGLE_STRIP ? GL_TRIANGLE_STRIP :
            GL_TRIANGLE_FAN, 0, getVertexCount(), 1);
#endif
    }
    // ------------------------------------------------------------------------
    virtual void uploadInstanceData()
    {
#ifndef SERVER_ONLY
        if (m_texture_trans.X != 0.0f || m_texture_trans.Y != 0.0f ||
            m_update_trans)
        {
            m_update_trans = false;
            SPInstancedData id = SPInstancedData(m_trans,
                m_texture_trans.X, m_texture_trans.Y, 0.0f, 0);
            glBindBuffer(GL_ARRAY_BUFFER, m_ibo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, 32, &id);
        }
        if (m_update_offset >= 0 && !m_vertices.empty())
        {
            if ((unsigned)m_vertices.capacity() > m_gl_vbo_size)
            {
                m_update_offset = 0;
                glDeleteBuffers(1, &m_vbo);
                glGenBuffers(1, &m_vbo);
                glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
                m_gl_vbo_size = (unsigned)m_vertices.capacity();
                glBufferData(GL_ARRAY_BUFFER, m_gl_vbo_size * 48, NULL,
                    GL_DYNAMIC_DRAW);
                recreateDynamicVAO();
            }
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            const int length = ((int)m_vertices.size() - m_update_offset) * 48;
            assert(length > 0);
            glBufferSubData(GL_ARRAY_BUFFER, m_update_offset * 48, length,
                m_vertices.data() + m_update_offset);
            m_update_offset = -1;
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
    }
    // ------------------------------------------------------------------------
    virtual void uploadGLMesh() {}
    // ------------------------------------------------------------------------
    virtual void enableTextureMatrix(unsigned mat_id) {}
    // ------------------------------------------------------------------------
    SPShader* getShader() const                            { return m_shader; }
    // ------------------------------------------------------------------------
    std::vector<video::S3DVertexSkinnedMesh>& getVerticesVector()
                                                         { return m_vertices; }
    // ------------------------------------------------------------------------
    core::vector2df& getTextureTrans()              { return m_texture_trans; }
    // ------------------------------------------------------------------------
    void setUpdateOffset(int offset)              { m_update_offset = offset; }
    // ------------------------------------------------------------------------
    bool isVisible() const                                { return m_visible; }
    // ------------------------------------------------------------------------
    void setVisible(bool val)                              { m_visible = val; }
    // ------------------------------------------------------------------------
    const core::matrix4& getAbsoluteTransformation() const  { return m_trans; }
    // ------------------------------------------------------------------------
    void setAbsoluteTransformation(core::matrix4& mat)
    {
        m_trans = mat;
        m_update_trans = true;
    }
    // ------------------------------------------------------------------------
    void setPosition(const core::vector3df pos)
    {
        m_trans.setTranslation(pos);
        m_update_trans = true;
    }
    // ------------------------------------------------------------------------
    void setRotationRadians(const core::vector3df rot)
    {
        m_trans.setRotationRadians(rot);
        m_update_trans = true;
    }
    // ------------------------------------------------------------------------
    void setRotationDegrees(const core::vector3df rot)
    {
        m_trans.setRotationDegrees(rot);
        m_update_trans = true;
    }
    // ------------------------------------------------------------------------
    void setScale(const core::vector3df scale)
    {
        m_trans.setScale(scale);
        m_update_trans = true;
    }

};

}

#endif

#include <S3DVertex.h>