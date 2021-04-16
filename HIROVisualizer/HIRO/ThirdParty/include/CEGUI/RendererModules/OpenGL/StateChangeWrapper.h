/***********************************************************************
    created:    Wed, 8th Feb 2012
    author:     Lukas E Meindl
*************************************************************************/
/***************************************************************************
     Copyright (C) 2004 - 2012 Paul D Turner & The CEGUI Development Team

     Permission is hereby granted, free of charge, to any person obtaining
     a copy of this software and associated documentation files (the
     "Software"), to deal in the Software without restriction, including
     without limitation the rights to use, copy, modify, merge, publish,
     distribute, sublicense, and/or sell copies of the Software, and to
     permit persons to whom the Software is furnished to do so, subject to
     the following conditions:

     The above copyright notice and this permission notice shall be
     included in all copies or substantial portions of the Software.

     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
     EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
     MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
     IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
     OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
     ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
     OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/

#ifndef _CEGUIOpenGL3StateChangeWrapper_h_
#define _CEGUIOpenGL3StateChangeWrapper_h_

#include "CEGUI/RendererModules/OpenGL/GL.h"
#include "CEGUI/RendererModules/OpenGL/GL3Renderer.h"

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable : 4251)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{

  /*!
    \brief
    OpenGL3StateChangeWrapper - wraps OpenGL calls and checks for redundant calls beforehand
  */
  class OPENGL_GUIRENDERER_API OpenGL3StateChangeWrapper :
    public AllocatedObject<OpenGL3StateChangeWrapper>
  {
  protected:
    struct BlendFuncParams
    {
      BlendFuncParams();
      void reset();
      bool equal(gl::GLenum sFactor, gl::GLenum dFactor);
      gl::GLenum d_sFactor, d_dFactor;
    };
    struct BlendFuncSeperateParams
    {
      BlendFuncSeperateParams();
      void reset();
      bool equal(gl::GLenum sfactorRGB, gl::GLenum dfactorRGB, gl::GLenum sfactorAlpha, gl::GLenum dfactorAlpha);
      gl::GLenum d_sfactorRGB, d_dfactorRGB, d_sfactorAlpha, d_dfactorAlpha;
    };
    struct PortParams
    {
      PortParams();
      void reset();
      bool equal(gl::GLint x, gl::GLint y, gl::GLsizei width, gl::GLsizei height);
      gl::GLint d_x, d_y;
      gl::GLsizei d_width, d_height;
    };
    struct BindBufferParams
    {
      BindBufferParams();
      void reset();
      bool equal(gl::GLenum target, gl::GLuint buffer);
      gl::GLenum d_target;
      gl::GLuint d_buffer;
    };

  public:

    OpenGL3StateChangeWrapper();
    OpenGL3StateChangeWrapper(OpenGL3Renderer &owner);
    virtual ~OpenGL3StateChangeWrapper();

    void reset();

    void bindVertexArray(gl::GLuint vertexArray);
    void blendFunc(gl::GLenum sfactor, gl::GLenum dfactor);
    void blendFuncSeparate(gl::GLenum sfactorRGB, gl::GLenum dfactorRGB, gl::GLenum sfactorAlpha, gl::GLenum dfactorAlpha);
    void viewport(gl::GLint x, gl::GLint y, gl::GLsizei width, gl::GLsizei height);
    void scissor(gl::GLint x, gl::GLint y, gl::GLsizei width, gl::GLsizei height);
    void bindBuffer(gl::GLenum target, gl::GLuint buffer);

  protected:
    gl::GLuint                      d_vertexArrayObject;
    BlendFuncParams             d_blendFuncParams;
    BlendFuncSeperateParams     d_blendFuncSeperateParams;
    PortParams                  d_viewPortParams;
    PortParams                  d_scissorParams;
    BindBufferParams            d_bindBufferParams;
  };

} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

#endif
