/** Author: sg
 * Created on 2. Februar 2017, 13:38  */

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <libraries/curses/Window.hpp>
#include "NcursesUI.hpp" //class NcursesUI

namespace Curses {
  class MainWindow 
    : public Curses::Window
  {
    NcursesUI & m_ncursesUI;
  public:
    MainWindow(NcursesUI & ncursesUI) : m_ncursesUI(ncursesUI) {
      Curses::Window::s_inputProcessorStack.add(this); }
    int HandleAction(const int ch);
  };
}

#endif /* MAINWINDOW_HPP */
