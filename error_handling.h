/*
 *       Filename:  error_handling.h
 *
 *    Description:  Provides a macro for opengl error handling.
 *
 *        Version:  1.0
 *        Created:  12.02.2008 16:34:51
 *
 *         Author:  Jan Funke, TU Dresden
 */

#ifndef SG_GUI_ERROR_HANDLING_H
#define SG_GUI_ERROR_HANDLING_H

#include <assert.h>
#include "util/Logger.h"

  #ifndef NDEBUG

    #define GL_ASSERT assert(sg_gui::glCheckHealth())

  #else

    #define GL_ASSERT

  #endif // NDEBUG

namespace sg_gui {

  bool glCheckHealth();

} // namespace sg_gui

#endif // SG_GUI_ERROR_HANDLING_H

