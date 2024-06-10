#ifndef COMMON_GL_DEBUG_H
#define COMMON_GL_DEBUG_H

#include <string>

#if defined (GLEW_SUPPORT)
#include "GL/glew.h"
#endif // GLEW_SUPPORT
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "gl/GL.h"
#else
#include "GL/gl.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Global_Macros.h"
#include "ace/Log_Msg.h"

class Common_GL_Debug
{
 public:
  static void Install ()
  {
    if (!glewIsSupported (ACE_TEXT_ALWAYS_CHAR ("GL_ARB_debug_output")))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("GL_ARB_debug_output not supported, returning\n")));
      return;
    } // end IF
    glEnable (GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    glDebugMessageCallbackARB (DebugCallback, 0);
    glDebugMessageControlARB (GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);

    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("installed OpenGL debug output handler\n")));
  }

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_GL_Debug ())
  ACE_UNIMPLEMENTED_FUNC (Common_GL_Debug (const Common_GL_Debug&))
  ACE_UNIMPLEMENTED_FUNC (Common_GL_Debug& operator= (const Common_GL_Debug&))

  static const char* Source (GLenum source)
  {
    switch (source)
    {
      case GL_DEBUG_SOURCE_API_ARB             : return ACE_TEXT_ALWAYS_CHAR ("API");
      case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB   : return ACE_TEXT_ALWAYS_CHAR ("WINDOW_SYSTEM");
      case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB : return ACE_TEXT_ALWAYS_CHAR ("SHADER_COMPILER");
      case GL_DEBUG_SOURCE_THIRD_PARTY_ARB     : return ACE_TEXT_ALWAYS_CHAR ("THIRD_PARTY");
      case GL_DEBUG_SOURCE_APPLICATION_ARB     : return ACE_TEXT_ALWAYS_CHAR ("APPLICATION");
      case GL_DEBUG_SOURCE_OTHER_ARB           : return ACE_TEXT_ALWAYS_CHAR ("OTHER");
      default                                  : return ACE_TEXT_ALWAYS_CHAR ("Unknown source");
    } // end SWITCH
  }

  static const char* Type (GLenum type)
  {
    switch (type)
    {
      case GL_DEBUG_TYPE_ERROR_ARB               : return ACE_TEXT_ALWAYS_CHAR ("ERROR");
      case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB : return ACE_TEXT_ALWAYS_CHAR ("DEPRECATED_BEHAVIOR");
      case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB  : return ACE_TEXT_ALWAYS_CHAR ("UNDEFINED_BEHAVIOR");
      case GL_DEBUG_TYPE_PORTABILITY_ARB         : return ACE_TEXT_ALWAYS_CHAR ("PORTABILITY");
      case GL_DEBUG_TYPE_PERFORMANCE_ARB         : return ACE_TEXT_ALWAYS_CHAR ("PERFORMANCE");
      case GL_DEBUG_TYPE_OTHER_ARB               : return ACE_TEXT_ALWAYS_CHAR ("OTHER");
      default                                    : return ACE_TEXT_ALWAYS_CHAR ("Unknown type");
    } // end SWITCH
  }

  static const char* Severity (GLenum severity)
  {
    switch( severity )
    {
      case GL_DEBUG_SEVERITY_HIGH_ARB   : return ACE_TEXT_ALWAYS_CHAR ("HIGH");
      case GL_DEBUG_SEVERITY_MEDIUM_ARB : return ACE_TEXT_ALWAYS_CHAR ("MEDIUM");
      case GL_DEBUG_SEVERITY_LOW_ARB    : return ACE_TEXT_ALWAYS_CHAR ("LOW");
      default                           : return ACE_TEXT_ALWAYS_CHAR ("Unknown severity");
    } // end SWITCH
  }

  static void APIENTRY DebugCallback (GLenum source,
                                      GLenum type,
                                      GLuint id,
                                      GLenum severity,
                                      GLsizei length,
                                      const GLchar* message,
                                      const void* userParam)
  { ACE_UNUSED_ARG (userParam);
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("GL_DEBUG: %s: %s: %s: \"%s\"\n"),
                ACE_TEXT (Source (source)),
                ACE_TEXT (Type (type)),
                ACE_TEXT (Severity (severity)),
                ACE_TEXT (message)));
  }
};

#endif // Common_GL_Debug_H
