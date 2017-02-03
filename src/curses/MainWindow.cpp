/** Author: sg
 * Created on 2. Februar 2017, 13:38  */

#include "MainWindow.hpp"

namespace Curses
{
  int MainWindow::HandleAction(const int ch)
  {
//      if( ch != ERR )
//        mvprintw(7, 3, "%d %c", ch, ch );
    int ret = -2;
    bool consumedInput = true;
    switch(ch)
    {
      case KEY_RESIZE :
        m_ncursesUI.LayoutUserInterface();
        m_ncursesUI.ReBuildUserInterface();
        m_ncursesUI.UpdateSMARTvaluesUI();
        break;
      default:
        ret = Curses::Window::inputNotHandled;
    }
//    if( consumedInput )
//      wrefresh(windowToShowMenuIn);
    return ret;
  }
}
