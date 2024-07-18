/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <filesystem>

#if defined(WIN32)
#include <gl/GL.h>
#else
#include <SDL_opengl.h>
#endif

#ifdef EMSCRIPTEN
#include "gl4es/include/GL/gl.h"
#include "gl4es/include/GL/glext.h"

extern "C" void gl4es_glActiveTexture(GLenum texture);
extern "C" void gl4es_glClientActiveTexture(GLenum texture);
extern "C" void gl4es_glMultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
extern "C" void gl4es_glMultiTexCoord4fARB(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
extern "C" void gl4es_glAlphaFunc(GLenum func, GLclampf ref);
extern "C" void gl4es_glBegin(GLenum mode);
extern "C" void gl4es_glBindTexture(GLenum target, GLuint texture);
extern "C" void gl4es_glBlendFunc(GLenum sfactor, GLenum dfactor);
extern "C" void gl4es_glClear(GLbitfield mask);
extern "C" void gl4es_glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
extern "C" void gl4es_glClearDepth(GLclampd depth);
extern "C" void gl4es_glColor3ub(GLubyte red, GLubyte green, GLubyte blue);
extern "C" void gl4es_glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
extern "C" void gl4es_glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
extern "C" void gl4es_glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
extern "C" void gl4es_glNormalPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
extern "C" void gl4es_glDeleteTextures(GLsizei n, const GLuint *textures);
extern "C" void gl4es_glDepthFunc(GLenum func);
extern "C" void gl4es_glDepthMask(GLboolean flag);
extern "C" void gl4es_glDepthRange(GLdouble near, GLdouble far);
extern "C" void gl4es_glDisable(GLenum cap);
extern "C" void gl4es_glDisableClientState(GLenum cap);
extern "C" void gl4es_glDrawArrays(GLenum mode, GLint first, GLsizei count);
extern "C" void gl4es_glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
extern "C" void gl4es_glDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
extern "C" void gl4es_glEnable(GLenum cap);
extern "C" void gl4es_glEnableClientState(GLenum array);
extern "C" void gl4es_glEnd(void);
extern "C" void gl4es_glFlush(void);
extern "C" void gl4es_glFogf(GLenum pname, GLfloat param);
extern "C" void gl4es_glFogfv(GLenum pname, const GLfloat *params);
extern "C" void gl4es_glFogi(GLenum pname, GLint param);
extern "C" GLenum gl4es_glGetError(void);
extern "C" const GLubyte *gl4es_glGetString(GLenum name);
extern "C" void gl4es_glHint(GLenum target, GLenum mode);
extern "C" void gl4es_glLoadIdentity(void);
extern "C" void gl4es_glLoadMatrixf(const GLfloat *m);
extern "C" void gl4es_glMatrixMode(GLenum mode);
extern "C" void gl4es_glOrtho(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble);
extern "C" void gl4es_glPixelStorei(GLenum pname, GLint param);
extern "C" void gl4es_glPixelTransferi(GLenum pname, GLint param);
extern "C" void gl4es_glPolygonOffset(GLfloat factor, GLfloat units);
extern "C" void gl4es_glReadPixels(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid *);
extern "C" void gl4es_glScissor(GLint x, GLint y, GLsizei width, GLsizei height);
extern "C" void gl4es_glShadeModel(GLenum mode);
extern "C" void gl4es_glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
extern "C" void gl4es_glTexEnvf(GLenum target, GLenum pname, GLfloat param);
extern "C" void gl4es_glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height,
                                   GLint border, GLenum format, GLenum type, const GLvoid *pixels);
extern "C" void gl4es_glTexParameteri(GLenum target, GLenum pname, GLint param);
extern "C" void gl4es_glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width,
                                      GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
extern "C" void gl4es_glVertex2i(GLint, GLint);
extern "C" void gl4es_glVertex3f(GLfloat, GLfloat, GLfloat);
extern "C" void gl4es_glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
extern "C" void gl4es_glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
extern "C" void gl4es_glColor4fv(const GLfloat *v);
extern "C" void gl4es_glVertex3fv(const GLfloat *v);
extern "C" void gl4es_glTexCoord4fv(const GLfloat *v);
#endif

#include "descent.h"
#include "module.h"

#if defined(WIN32)
#define GLFUNCCALL __stdcall
#elif defined(POSIX)
#include <unistd.h>
#define GLFUNCCALL
#endif

// ryan's adds. 04/18/2000.
//  There was a bunch of stuff that was, I guess, not supported by
//  Mesa when Outrage did the original port. So buttloads of code
//  involving glActiveTextureARB() were #ifdef'd to only WIN32.
//  To make use of the extension more generic, platforms that can handle
//  it (Linux and Win32, as far as I care), define
//  _USE_OGL_ACTIVE_TEXTURES, and check that define instead of WIN32 when
//  using the extension.
#if (!defined(_USE_OGL_ACTIVE_TEXTURES))
#if (defined(WIN32))
#define _USE_OGL_ACTIVE_TEXTURES
#endif
#endif
// end ryan's adds.

#ifdef DECLARE_OPENGL
#define DYNAEXTERN(_ty, _name) _ty _name = NULL
#else
#define DYNAEXTERN(_ty, _name) extern _ty _name;
#endif

typedef void(GLFUNCCALL *glAlphaFunc_fp)(GLenum func, GLclampf ref);
typedef void(GLFUNCCALL *glBegin_fp)(GLenum mode);
typedef void(GLFUNCCALL *glBindTexture_fp)(GLenum target, GLuint texture);
typedef void(GLFUNCCALL *glBlendFunc_fp)(GLenum sfactor, GLenum dfactor);
typedef void(GLFUNCCALL *glClear_fp)(GLbitfield mask);
typedef void(GLFUNCCALL *glClearColor_fp)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
typedef void(GLFUNCCALL *glClearDepth_fp)(GLclampd depth);
typedef void(GLFUNCCALL *glColor3ub_fp)(GLubyte red, GLubyte green, GLubyte blue);
typedef void(GLFUNCCALL *glColor4ub_fp)(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
typedef void(GLFUNCCALL *glColor4f_fp)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void(GLFUNCCALL *glColorPointer_fp)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void(GLFUNCCALL *glNormalPointer_fp)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void(GLFUNCCALL *glDeleteTextures_fp)(GLsizei n, const GLuint *textures);
typedef void(GLFUNCCALL *glDepthFunc_fp)(GLenum func);
typedef void(GLFUNCCALL *glDepthMask_fp)(GLboolean flag);
typedef void(GLFUNCCALL *glDepthRange_fp)(GLdouble near, GLdouble far);
typedef void(GLFUNCCALL *glDisable_fp)(GLenum cap);
typedef void(GLFUNCCALL *glDisableClientState_fp)(GLenum cap);
typedef void(GLFUNCCALL *glDrawArrays_fp)(GLenum mode, GLint first, GLsizei count);
typedef void(GLFUNCCALL *glDrawElements_fp)(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
typedef void(GLFUNCCALL *glDrawPixels_fp)(GLsizei width, GLsizei height, GLenum format, GLenum type,
                                          const GLvoid *pixels);
typedef void(GLFUNCCALL *glEnable_fp)(GLenum cap);
typedef void(GLFUNCCALL *glEnableClientState_fp)(GLenum array);
typedef void(GLFUNCCALL *glEnd_fp)(void);
typedef void(GLFUNCCALL *glFlush_fp)(void);
typedef void(GLFUNCCALL *glFogf_fp)(GLenum pname, GLfloat param);
typedef void(GLFUNCCALL *glFogfv_fp)(GLenum pname, const GLfloat *params);
typedef void(GLFUNCCALL *glFogi_fp)(GLenum pname, GLint param);
typedef GLenum(GLFUNCCALL *glGetError_fp)(void);
typedef const GLubyte *(GLFUNCCALL *glGetString_fp)(GLenum name);
typedef void(GLFUNCCALL *glHint_fp)(GLenum target, GLenum mode);
typedef void(GLFUNCCALL *glLoadIdentity_fp)(void);
typedef void(GLFUNCCALL *glLoadMatrixf_fp)(const GLfloat *m);
typedef void(GLFUNCCALL *glMatrixMode_fp)(GLenum mode);
typedef void(GLFUNCCALL *glOrtho_fp)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble);
typedef void(GLFUNCCALL *glPixelStorei_fp)(GLenum pname, GLint param);
typedef void(GLFUNCCALL *glPixelTransferi_fp)(GLenum pname, GLint param);
typedef void(GLFUNCCALL *glPolygonOffset_fp)(GLfloat factor, GLfloat units);
typedef void(GLFUNCCALL *glReadPixels_fp)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid *);
typedef void(GLFUNCCALL *glScissor_fp)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void(GLFUNCCALL *glShadeModel_fp)(GLenum mode);
typedef void(GLFUNCCALL *glTexCoordPointer_fp)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void(GLFUNCCALL *glTexEnvf_fp)(GLenum target, GLenum pname, GLfloat param);
typedef void(GLFUNCCALL *glTexImage2D_fp)(GLenum target, GLint level, GLint internalformat, GLsizei width,
                                          GLsizei height, GLint border, GLenum format, GLenum type,
                                          const GLvoid *pixels);
typedef void(GLFUNCCALL *glTexParameteri_fp)(GLenum target, GLenum pname, GLint param);
typedef void(GLFUNCCALL *glTexSubImage2D_fp)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width,
                                             GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
typedef void(GLFUNCCALL *glVertex2i_fp)(GLint, GLint);
typedef void(GLFUNCCALL *glVertex3f_fp)(GLfloat, GLfloat, GLfloat);
typedef void(GLFUNCCALL *glVertexPointer_fp)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
typedef void(GLFUNCCALL *glViewport_fp)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void(GLFUNCCALL *glColor4fv_fp)(const GLfloat *v);
typedef void(GLFUNCCALL *glVertex3fv_fp)(const GLfloat *v);
typedef void(GLFUNCCALL *glTexCoord4fv_fp)(const GLfloat *v);
typedef void(GLFUNCCALL *glTexCoord2f_fp)(const GLfloat p1, const GLfloat p2);
typedef void(GLFUNCCALL *glGetIntegerv_fp)(GLenum pname, GLint *params);
typedef GLuint(GLFUNCCALL *glGenLists_fp)(GLsizei range);
typedef void(GLFUNCCALL *glNewList_fp)(GLuint list, GLenum mode);
typedef void(GLFUNCCALL *glEndList_fp)(void);
typedef void(GLFUNCCALL *glCallLists_fp)(GLsizei n, GLenum type, const GLvoid *lists);
typedef void(GLFUNCCALL *glListBase_fp)(GLuint base);
typedef void(GLFUNCCALL *glPushAttrib_fp)(GLbitfield mask);
typedef void(GLFUNCCALL *glPopAttrib_fp)(void);
typedef void(GLFUNCCALL *glColor3f_fp)(GLfloat red, GLfloat green, GLfloat blue);
typedef void(GLFUNCCALL *glRasterPos2i_fp)(GLint x, GLint y);
typedef void(GLFUNCCALL *glBitmap_fp)(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove,
                                      GLfloat ymove, const GLubyte *bitmap);

// FBO entry points for render-to-texture ...
typedef void(GLFUNCCALL *glGenFramebuffersEXT_fp)(GLsizei n, GLuint *framebuffers);
typedef void(GLFUNCCALL *glGenRenderbuffersEXT_fp)(GLsizei n, GLuint *renderbuffers);
typedef void(GLFUNCCALL *glBindFramebufferEXT_fp)(GLenum target, GLuint framebuffer);
typedef void(GLFUNCCALL *glBindRenderbufferEXT_fp)(GLenum target, GLuint renderbuffer);
typedef void(GLFUNCCALL *glRenderbufferStorageEXT_fp)(GLenum target, GLenum internalformat, GLsizei width,
                                                      GLsizei height);
typedef void(GLFUNCCALL *glFramebufferRenderbufferEXT_fp)(GLenum target, GLenum attachment, GLenum renderbuffertarget,
                                                          GLuint renderbuffer);
typedef GLenum(GLFUNCCALL *glCheckFramebufferStatusEXT_fp)(GLenum target);
typedef void(GLFUNCCALL *glDeleteRenderbuffersEXT_fp)(GLsizei n, const GLuint *renderbuffers);
typedef void(GLFUNCCALL *glDeleteFramebuffersEXT_fp)(GLsizei n, const GLuint *framebuffers);
typedef void(GLFUNCCALL *glBlitFramebufferEXT_fp)(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0,
                                                  GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask,
                                                  GLenum filter);

#if defined(WIN32)
typedef HGLRC(GLFUNCCALL *wglCreateContext_fp)(HDC);
typedef BOOL(GLFUNCCALL *wglDeleteContext_fp)(HGLRC);
typedef BOOL(GLFUNCCALL *wglMakeCurrent_fp)(HDC, HGLRC);
typedef PROC(GLFUNCCALL *wglGetProcAddress_fp)(LPCSTR);

DYNAEXTERN(wglCreateContext_fp, dwglCreateContext);
DYNAEXTERN(wglDeleteContext_fp, dwglDeleteContext);
DYNAEXTERN(wglMakeCurrent_fp, dwglMakeCurrent);
DYNAEXTERN(wglGetProcAddress_fp, dwglGetProcAddress);
#endif

DYNAEXTERN(glAlphaFunc_fp, dglAlphaFunc);
DYNAEXTERN(glBegin_fp, dglBegin);
DYNAEXTERN(glBindTexture_fp, dglBindTexture);
DYNAEXTERN(glBlendFunc_fp, dglBlendFunc);
DYNAEXTERN(glClear_fp, dglClear);
DYNAEXTERN(glClearColor_fp, dglClearColor);
DYNAEXTERN(glClearDepth_fp, dglClearDepth);
DYNAEXTERN(glColor3ub_fp, dglColor3ub);
DYNAEXTERN(glColor4ub_fp, dglColor4ub);
DYNAEXTERN(glColor4f_fp, dglColor4f);
DYNAEXTERN(glColorPointer_fp, dglColorPointer);
DYNAEXTERN(glNormalPointer_fp, dglNormalPointer);
DYNAEXTERN(glDeleteTextures_fp, dglDeleteTextures);
DYNAEXTERN(glDepthFunc_fp, dglDepthFunc);
DYNAEXTERN(glDepthMask_fp, dglDepthMask);
DYNAEXTERN(glDepthRange_fp, dglDepthRange);
DYNAEXTERN(glDisable_fp, dglDisable);
DYNAEXTERN(glDisableClientState_fp, dglDisableClientState);
DYNAEXTERN(glDrawArrays_fp, dglDrawArrays);
DYNAEXTERN(glDrawElements_fp, dglDrawElements);
DYNAEXTERN(glDrawPixels_fp, dglDrawPixels);
DYNAEXTERN(glEnable_fp, dglEnable);
DYNAEXTERN(glEnableClientState_fp, dglEnableClientState);
DYNAEXTERN(glEnd_fp, dglEnd);
DYNAEXTERN(glFlush_fp, dglFlush);
DYNAEXTERN(glFogf_fp, dglFogf);
DYNAEXTERN(glFogfv_fp, dglFogfv);
DYNAEXTERN(glFogi_fp, dglFogi);
DYNAEXTERN(glGetError_fp, dglGetError);
DYNAEXTERN(glGetString_fp, dglGetString);
DYNAEXTERN(glHint_fp, dglHint);
DYNAEXTERN(glLoadIdentity_fp, dglLoadIdentity);
DYNAEXTERN(glLoadMatrixf_fp, dglLoadMatrixf);
DYNAEXTERN(glMatrixMode_fp, dglMatrixMode);
DYNAEXTERN(glOrtho_fp, dglOrtho);
DYNAEXTERN(glPixelStorei_fp, dglPixelStorei);
DYNAEXTERN(glPixelTransferi_fp, dglPixelTransferi);
DYNAEXTERN(glPolygonOffset_fp, dglPolygonOffset);
DYNAEXTERN(glReadPixels_fp, dglReadPixels);
DYNAEXTERN(glScissor_fp, dglScissor);
DYNAEXTERN(glShadeModel_fp, dglShadeModel);
DYNAEXTERN(glTexCoordPointer_fp, dglTexCoordPointer);
DYNAEXTERN(glTexEnvf_fp, dglTexEnvf);
DYNAEXTERN(glTexImage2D_fp, dglTexImage2D);
DYNAEXTERN(glTexParameteri_fp, dglTexParameteri);
DYNAEXTERN(glTexSubImage2D_fp, dglTexSubImage2D);
DYNAEXTERN(glVertex2i_fp, dglVertex2i);
DYNAEXTERN(glVertex3f_fp, dglVertex3f);
DYNAEXTERN(glVertexPointer_fp, dglVertexPointer);
DYNAEXTERN(glViewport_fp, dglViewport);
DYNAEXTERN(glColor4fv_fp, dglColor4fv);
DYNAEXTERN(glVertex3fv_fp, dglVertex3fv);
DYNAEXTERN(glTexCoord4fv_fp, dglTexCoord4fv);

#ifdef DECLARE_OPENGL
static module OpenGLDLLInst;

static void *__SDL_mod_GetSymbol(const char *funcStr) {
  void *retVal = NULL;

  mprintf(0, "Looking up GL function [%s]... ", funcStr);

  retVal = SDL_GL_GetProcAddress(funcStr);

  if (retVal == NULL)
    fprintf(stderr, " Could not find GL symbol [%s]!\n\n", funcStr);
  else {
    mprintf(0, "Found at (%p).\n", retVal);
  } // else

  return (retVal);
} // __SDL_mod_GetSymbol

/****************** WARNING: NASTY HACK! ***********************/
#define mod_GetSymbol(x, funcStr, y) __SDL_mod_GetSymbol(funcStr)
/****************** WARNING: NASTY HACK! ***********************/

extern char loadedLibrary[_MAX_PATH];

module *LoadOpenGLDLL(const char *dllname) {
  mprintf(0, "Loading OpenGL dll...\n");

  std::filesystem::path tmp = std::filesystem::current_path();
  std::filesystem::current_path(orig_pwd);
  int rc = SDL_GL_LoadLibrary(dllname[0] ? dllname : nullptr);
  std::filesystem::current_path(tmp);

  if (rc < 0) {
    const char *sdlErr = SDL_GetError();
    mprintf(0, "OpenGL: Couldn't open library [%s].\n", dllname[0] ? dllname : "system default");
    mprintf(0, "OpenGL:  SDL error is [%s].", sdlErr);
    return NULL;
  }

  strcpy(loadedLibrary, dllname);

#ifdef EMSCRIPTEN

  dglAlphaFunc = (glAlphaFunc_fp)&gl4es_glAlphaFunc;

  if (!dglAlphaFunc)
    goto dll_error;

  dglBegin = (glBegin_fp)&gl4es_glBegin;
  if (!dglBegin)
    goto dll_error;

  dglBindTexture = (glBindTexture_fp)&gl4es_glBindTexture;
  if (!dglBindTexture)
    goto dll_error;

  dglBlendFunc = (glBlendFunc_fp)&gl4es_glBlendFunc;
  if (!dglBlendFunc)
    goto dll_error;

  dglClear = (glClear_fp)&gl4es_glClear;
  if (!dglClear)
    goto dll_error;

  dglClearColor = (glClearColor_fp)&gl4es_glClearColor;
  if (!dglClearColor)
    goto dll_error;

  dglClearDepth = (glClearDepth_fp)&gl4es_glClearDepth;
  if (!dglClearDepth)
    goto dll_error;

  dglColor3ub = (glColor3ub_fp)&gl4es_glColor3ub;
  if (!dglColor3ub)
    goto dll_error;

  dglColor4ub = (glColor4ub_fp)&gl4es_glColor4ub;
  if (!dglColor4ub)
    goto dll_error;

  dglColor4f = (glColor4f_fp)&gl4es_glColor4f;
  if (!dglColor4f)
    goto dll_error;

  dglColorPointer = (glColorPointer_fp)&gl4es_glColorPointer;
  if (!dglColorPointer)
    goto dll_error;

  dglNormalPointer = (glNormalPointer_fp)&gl4es_glNormalPointer;
  if (!dglNormalPointer)
    goto dll_error;

  dglDeleteTextures = (glDeleteTextures_fp)&gl4es_glDeleteTextures;
  if (!dglDeleteTextures)
    goto dll_error;

  dglDepthFunc = (glDepthFunc_fp)&gl4es_glDepthFunc;
  if (!dglDepthFunc)
    goto dll_error;

  dglDepthMask = (glDepthMask_fp)&gl4es_glDepthMask;
  if (!dglDepthMask)
    goto dll_error;

  dglDepthRange = (glDepthRange_fp)&gl4es_glDepthRange;
  if (!dglDepthRange)
    goto dll_error;

  dglDisable = (glDisable_fp)&gl4es_glDisable;
  if (!dglDisable)
    goto dll_error;

  dglDisableClientState = (glDisableClientState_fp)&gl4es_glDisableClientState;
  if (!dglDisableClientState)
    goto dll_error;

  dglDrawArrays = (glDrawArrays_fp)&gl4es_glDrawArrays;
  if (!dglDrawArrays)
    goto dll_error;

  dglDrawElements = (glDrawElements_fp)&gl4es_glDrawElements;
  if (!dglDrawElements)
    goto dll_error;

  dglDrawPixels = (glDrawPixels_fp)&gl4es_glDrawPixels;
  if (!dglDrawPixels)
    goto dll_error;

  dglEnable = (glEnable_fp)&gl4es_glEnable;
  if (!dglEnable)
    goto dll_error;

  dglEnableClientState = (glEnableClientState_fp)&gl4es_glEnableClientState;
  if (!dglEnableClientState)
    goto dll_error;

  dglEnd = (glEnd_fp)&gl4es_glEnd;
  if (!dglEnd)
    goto dll_error;

  dglFlush = (glFlush_fp)&gl4es_glFlush;
  if (!dglFlush)
    goto dll_error;

  dglFogf = (glFogf_fp)&gl4es_glFogf;
  if (!dglFogf)
    goto dll_error;

  dglFogfv = (glFogfv_fp)&gl4es_glFogfv;
  if (!dglFogfv)
    goto dll_error;

  dglFogi = (glFogi_fp)&gl4es_glFogi;
  if (!dglFogi)
    goto dll_error;

  dglGetError = (glGetError_fp)&gl4es_glGetError;
  if (!dglGetError)
    goto dll_error;

  dglGetString = (glGetString_fp)&gl4es_glGetString;
  if (!dglGetString)
    goto dll_error;

  dglHint = (glHint_fp)&gl4es_glHint;
  if (!dglHint)
    goto dll_error;

  dglLoadIdentity = (glLoadIdentity_fp)&gl4es_glLoadIdentity;
  if (!dglLoadIdentity)
    goto dll_error;

  dglLoadMatrixf = (glLoadMatrixf_fp)&gl4es_glLoadMatrixf;
  if (!dglLoadMatrixf)
    goto dll_error;

  dglMatrixMode = (glMatrixMode_fp)&gl4es_glMatrixMode;
  if (!dglMatrixMode)
    goto dll_error;

  dglOrtho = (glOrtho_fp)&gl4es_glOrtho;
  if (!dglOrtho)
    goto dll_error;

  dglPixelStorei = (glPixelStorei_fp)&gl4es_glPixelStorei;
  if (!dglPixelStorei)
    goto dll_error;

  dglPixelTransferi = (glPixelTransferi_fp)&gl4es_glPixelTransferi;
  if (!dglPixelTransferi)
    goto dll_error;

  dglPolygonOffset = (glPolygonOffset_fp)&gl4es_glPolygonOffset;
  if (!dglPolygonOffset)
    goto dll_error;

  dglReadPixels = (glReadPixels_fp)&gl4es_glReadPixels;
  if (!dglReadPixels)
    goto dll_error;

  dglScissor = (glScissor_fp)&gl4es_glScissor;
  if (!dglScissor)
    goto dll_error;

  dglShadeModel = (glShadeModel_fp)&gl4es_glShadeModel;
  if (!dglShadeModel)
    goto dll_error;

  dglTexCoordPointer = (glTexCoordPointer_fp)&gl4es_glTexCoordPointer;
  if (!dglTexCoordPointer)
    goto dll_error;

  dglTexEnvf = (glTexEnvf_fp)&gl4es_glTexEnvf;
  if (!dglTexEnvf)
    goto dll_error;

  dglTexImage2D = (glTexImage2D_fp)&gl4es_glTexImage2D;
  if (!dglTexImage2D)
    goto dll_error;

  dglTexParameteri = (glTexParameteri_fp)&gl4es_glTexParameteri;
  if (!dglTexParameteri)
    goto dll_error;

  dglTexSubImage2D = (glTexSubImage2D_fp)&gl4es_glTexSubImage2D;
  if (!dglTexSubImage2D)
    goto dll_error;

  dglVertex2i = (glVertex2i_fp)&gl4es_glVertex2i;
  if (!dglVertex2i)
    goto dll_error;

  dglVertex3f = (glVertex3f_fp)&gl4es_glVertex3f;
  if (!dglVertex3f)
    goto dll_error;

  dglVertexPointer = (glVertexPointer_fp)&gl4es_glVertexPointer;
  if (!dglVertexPointer)
    goto dll_error;

  dglViewport = (glViewport_fp)&gl4es_glViewport;
  if (!dglViewport)
    goto dll_error;

  dglColor4fv = (glColor4fv_fp)&gl4es_glColor4fv;
  if (!dglColor4fv)
    goto dll_error;

  dglVertex3fv = (glVertex3fv_fp)&gl4es_glVertex3fv;
  if (!dglVertex3fv)
    goto dll_error;

  dglTexCoord4fv = (glTexCoord4fv_fp)&gl4es_glTexCoord4fv;
  if (!dglTexCoord4fv)
    goto dll_error;

#else

  dglAlphaFunc = (glAlphaFunc_fp)mod_GetSymbol(&OpenGLDLLInst, "glAlphaFunc", 255);
  if (!dglAlphaFunc)
    goto dll_error;

  dglBegin = (glBegin_fp)mod_GetSymbol(&OpenGLDLLInst, "glBegin", 255);
  if (!dglBegin)
    goto dll_error;

  dglBindTexture = (glBindTexture_fp)mod_GetSymbol(&OpenGLDLLInst, "glBindTexture", 255);
  if (!dglBindTexture)
    goto dll_error;

  dglBlendFunc = (glBlendFunc_fp)mod_GetSymbol(&OpenGLDLLInst, "glBlendFunc", 255);
  if (!dglBlendFunc)
    goto dll_error;

  dglClear = (glClear_fp)mod_GetSymbol(&OpenGLDLLInst, "glClear", 255);
  if (!dglClear)
    goto dll_error;

  dglClearColor = (glClearColor_fp)mod_GetSymbol(&OpenGLDLLInst, "glClearColor", 255);
  if (!dglClearColor)
    goto dll_error;

  dglClearDepth = (glClearDepth_fp)mod_GetSymbol(&OpenGLDLLInst, "glClearDepth", 255);
  if (!dglClearDepth)
    goto dll_error;

  dglColor3ub = (glColor3ub_fp)mod_GetSymbol(&OpenGLDLLInst, "glColor3ub", 255);
  if (!dglColor3ub)
    goto dll_error;

  dglColor4ub = (glColor4ub_fp)mod_GetSymbol(&OpenGLDLLInst, "glColor4ub", 255);
  if (!dglColor4ub)
    goto dll_error;

  dglColor4f = (glColor4f_fp)mod_GetSymbol(&OpenGLDLLInst, "glColor4f", 255);
  if (!dglColor4f)
    goto dll_error;

  dglColorPointer = (glColorPointer_fp)mod_GetSymbol(&OpenGLDLLInst, "glColorPointer", 255);
  if (!dglColorPointer)
    goto dll_error;

  dglNormalPointer = (glNormalPointer_fp)mod_GetSymbol(&OpenGLDLLInst, "glNormalPointer", 255);
  if (!dglNormalPointer)
    goto dll_error;

  dglDeleteTextures = (glDeleteTextures_fp)mod_GetSymbol(&OpenGLDLLInst, "glDeleteTextures", 255);
  if (!dglDeleteTextures)
    goto dll_error;

  dglDepthFunc = (glDepthFunc_fp)mod_GetSymbol(&OpenGLDLLInst, "glDepthFunc", 255);
  if (!dglDepthFunc)
    goto dll_error;

  dglDepthMask = (glDepthMask_fp)mod_GetSymbol(&OpenGLDLLInst, "glDepthMask", 255);
  if (!dglDepthMask)
    goto dll_error;

  dglDepthRange = (glDepthRange_fp)mod_GetSymbol(&OpenGLDLLInst, "glDepthRange", 255);
  if (!dglDepthRange)
    goto dll_error;

  dglDisable = (glDisable_fp)mod_GetSymbol(&OpenGLDLLInst, "glDisable", 255);
  if (!dglDisable)
    goto dll_error;

  dglDisableClientState = (glDisableClientState_fp)mod_GetSymbol(&OpenGLDLLInst, "glDisableClientState", 255);
  if (!dglDisableClientState)
    goto dll_error;

  dglDrawArrays = (glDrawArrays_fp)mod_GetSymbol(&OpenGLDLLInst, "glDrawArrays", 255);
  if (!dglDrawArrays)
    goto dll_error;

  dglDrawElements = (glDrawElements_fp)mod_GetSymbol(&OpenGLDLLInst, "glDrawElements", 255);
  if (!dglDrawElements)
    goto dll_error;

  dglDrawPixels = (glDrawPixels_fp)mod_GetSymbol(&OpenGLDLLInst, "glDrawPixels", 255);
  if (!dglDrawPixels)
    goto dll_error;

  dglEnable = (glEnable_fp)mod_GetSymbol(&OpenGLDLLInst, "glEnable", 255);
  if (!dglEnable)
    goto dll_error;

  dglEnableClientState = (glEnableClientState_fp)mod_GetSymbol(&OpenGLDLLInst, "glEnableClientState", 255);
  if (!dglEnableClientState)
    goto dll_error;

  dglEnd = (glEnd_fp)mod_GetSymbol(&OpenGLDLLInst, "glEnd", 255);
  if (!dglEnd)
    goto dll_error;

  dglFlush = (glFlush_fp)mod_GetSymbol(&OpenGLDLLInst, "glFlush", 255);
  if (!dglFlush)
    goto dll_error;

  dglFogf = (glFogf_fp)mod_GetSymbol(&OpenGLDLLInst, "glFogf", 255);
  if (!dglFogf)
    goto dll_error;

  dglFogfv = (glFogfv_fp)mod_GetSymbol(&OpenGLDLLInst, "glFogfv", 255);
  if (!dglFogfv)
    goto dll_error;

  dglFogi = (glFogi_fp)mod_GetSymbol(&OpenGLDLLInst, "glFogi", 255);
  if (!dglFogi)
    goto dll_error;

  dglGetError = (glGetError_fp)mod_GetSymbol(&OpenGLDLLInst, "glGetError", 255);
  if (!dglGetError)
    goto dll_error;

  dglGetString = (glGetString_fp)mod_GetSymbol(&OpenGLDLLInst, "glGetString", 255);
  if (!dglGetString)
    goto dll_error;

  dglHint = (glHint_fp)mod_GetSymbol(&OpenGLDLLInst, "glHint", 255);
  if (!dglHint)
    goto dll_error;

  dglLoadIdentity = (glLoadIdentity_fp)mod_GetSymbol(&OpenGLDLLInst, "glLoadIdentity", 255);
  if (!dglLoadIdentity)
    goto dll_error;

  dglLoadMatrixf = (glLoadMatrixf_fp)mod_GetSymbol(&OpenGLDLLInst, "glLoadMatrixf", 255);
  if (!dglLoadMatrixf)
    goto dll_error;

  dglMatrixMode = (glMatrixMode_fp)mod_GetSymbol(&OpenGLDLLInst, "glMatrixMode", 255);
  if (!dglMatrixMode)
    goto dll_error;

  dglOrtho = (glOrtho_fp)mod_GetSymbol(&OpenGLDLLInst, "glOrtho", 255);
  if (!dglOrtho)
    goto dll_error;

  dglPixelStorei = (glPixelStorei_fp)mod_GetSymbol(&OpenGLDLLInst, "glPixelStorei", 255);
  if (!dglPixelStorei)
    goto dll_error;

  dglPixelTransferi = (glPixelTransferi_fp)mod_GetSymbol(&OpenGLDLLInst, "glPixelTransferi", 255);
  if (!dglPixelTransferi)
    goto dll_error;

  dglPolygonOffset = (glPolygonOffset_fp)mod_GetSymbol(&OpenGLDLLInst, "glPolygonOffset", 255);
  if (!dglPolygonOffset)
    goto dll_error;

  dglReadPixels = (glReadPixels_fp)mod_GetSymbol(&OpenGLDLLInst, "glReadPixels", 255);
  if (!dglReadPixels)
    goto dll_error;

  dglScissor = (glScissor_fp)mod_GetSymbol(&OpenGLDLLInst, "glScissor", 255);
  if (!dglScissor)
    goto dll_error;

  dglShadeModel = (glShadeModel_fp)mod_GetSymbol(&OpenGLDLLInst, "glShadeModel", 255);
  if (!dglShadeModel)
    goto dll_error;

  dglTexCoordPointer = (glTexCoordPointer_fp)mod_GetSymbol(&OpenGLDLLInst, "glTexCoordPointer", 255);
  if (!dglTexCoordPointer)
    goto dll_error;

  dglTexEnvf = (glTexEnvf_fp)mod_GetSymbol(&OpenGLDLLInst, "glTexEnvf", 255);
  if (!dglTexEnvf)
    goto dll_error;

  dglTexImage2D = (glTexImage2D_fp)mod_GetSymbol(&OpenGLDLLInst, "glTexImage2D", 255);
  if (!dglTexImage2D)
    goto dll_error;

  dglTexParameteri = (glTexParameteri_fp)mod_GetSymbol(&OpenGLDLLInst, "glTexParameteri", 255);
  if (!dglTexParameteri)
    goto dll_error;

  dglTexSubImage2D = (glTexSubImage2D_fp)mod_GetSymbol(&OpenGLDLLInst, "glTexSubImage2D", 255);
  if (!dglTexSubImage2D)
    goto dll_error;

  dglVertex2i = (glVertex2i_fp)mod_GetSymbol(&OpenGLDLLInst, "glVertex2i", 255);
  if (!dglVertex2i)
    goto dll_error;

  dglVertex3f = (glVertex3f_fp)mod_GetSymbol(&OpenGLDLLInst, "glVertex3f", 255);
  if (!dglVertex3f)
    goto dll_error;

  dglVertexPointer = (glVertexPointer_fp)mod_GetSymbol(&OpenGLDLLInst, "glVertexPointer", 255);
  if (!dglVertexPointer)
    goto dll_error;

  dglViewport = (glViewport_fp)mod_GetSymbol(&OpenGLDLLInst, "glViewport", 255);
  if (!dglViewport)
    goto dll_error;

  dglColor4fv = (glColor4fv_fp)mod_GetSymbol(&OpenGLDLLInst, "glColor4fv", 255);
  if (!dglColor4fv)
    goto dll_error;

  dglVertex3fv = (glVertex3fv_fp)mod_GetSymbol(&OpenGLDLLInst, "glVertex3fv", 255);
  if (!dglVertex3fv)
    goto dll_error;

  dglTexCoord4fv = (glTexCoord4fv_fp)mod_GetSymbol(&OpenGLDLLInst, "glTexCoord4fv", 255);
  if (!dglTexCoord4fv)
    goto dll_error;
#endif

#if defined(WIN32)
  dwglCreateContext = (wglCreateContext_fp)&gl4es_wglCreateContext;
  if (!dwglCreateContext)
    goto dll_error;

  dwglDeleteContext = (wglDeleteContext_fp)&gl4es_wglDeleteContext;
  if (!dwglDeleteContext)
    goto dll_error;

  dwglMakeCurrent = (wglMakeCurrent_fp)&gl4es_wglMakeCurrent;
  if (!dwglMakeCurrent)
    goto dll_error;

  dwglGetProcAddress = (wglGetProcAddress_fp)&gl4es_wglGetProcAddress;
  if (!dwglGetProcAddress)
    goto dll_error;
#elif defined(POSIX)

  // bk000614 - have to fix globals
  extern glAlphaFunc_fp dglAlphaFunc;
  extern glBegin_fp dglBegin;
  extern glBindTexture_fp dglBindTexture;
  extern glBlendFunc_fp dglBlendFunc;
  extern glClear_fp dglClear;
  extern glClearColor_fp dglClearColor;
  extern glClearDepth_fp dglClearDepth;
  extern glColor3ub_fp dglColor3ub;
  extern glColor4f_fp dglColor4f;
  extern glColor4fv_fp dglColor4fv;
  extern glColor4ub_fp dglColor4ub;
  extern glColorPointer_fp dglColorPointer;
  extern glDeleteTextures_fp dglDeleteTextures;
  extern glDepthFunc_fp dglDepthFunc;
  extern glDepthMask_fp dglDepthMask;
  extern glDepthRange_fp dglDepthRange;
  extern glDisable_fp dglDisable;
  extern glDisableClientState_fp dglDisableClientState;
  extern glDrawArrays_fp dglDrawArrays;
  extern glDrawPixels_fp dglDrawPixels;
  extern glEnable_fp dglEnable;
  extern glEnableClientState_fp dglEnableClientState;
  extern glEnd_fp dglEnd;
  extern glFlush_fp dglFlush;
  extern glFogf_fp dglFogf;
  extern glFogfv_fp dglFogfv;
  extern glFogi_fp dglFogi;
  extern glGetError_fp dglGetError;
  extern glGetString_fp dglGetString;
  extern glGetIntegerv_fp dglGetIntegerv;
  extern glHint_fp dglHint;
  extern glLoadIdentity_fp dglLoadIdentity;
  extern glMatrixMode_fp dglMatrixMode;
  extern glOrtho_fp dglOrtho;
  extern glPixelStorei_fp dglPixelStorei;
  extern glPixelTransferi_fp dglPixelTransferi;
  extern glPolygonOffset_fp dglPolygonOffset;
  extern glReadPixels_fp dglReadPixels;
  extern glScissor_fp dglScissor;
  extern glShadeModel_fp dglShadeModel;
  extern glTexCoord2f_fp dglTexCoord2f;
  extern glTexCoord4fv_fp dglTexCoord4fv;
  extern glTexCoordPointer_fp dglTexCoordPointer;
  extern glTexEnvf_fp dglTexEnvf;
  extern glTexImage2D_fp dglTexImage2D;
  extern glTexParameteri_fp dglTexParameteri;
  extern glTexSubImage2D_fp dglTexSubImage2D;
  extern glVertex2i_fp dglVertex2i;
  extern glVertex3f_fp dglVertex3f;
  extern glVertex3fv_fp dglVertex3fv;
  extern glVertexPointer_fp dglVertexPointer;
  extern glViewport_fp dglViewport;

  module *LoadOpenGLDLL(const char *dllname);

#else
#endif
  mprintf(0, "OpenGL dll loading successful.\n");

  return &OpenGLDLLInst;

dll_error:
  mprintf(0, "Error loading opengl dll!\n");
  mod_FreeModule(&OpenGLDLLInst);
  return NULL;
}
#endif
