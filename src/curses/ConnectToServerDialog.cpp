/** File:   ConnectToServerDialog.cpp
 * Author: sg
 * Created on 26. Juni 2017, 10:27 */

#include "ConnectToServerDialog.hpp"
#include <unistd.h> //close(...)

ConnectToServerDialog::ConnectToServerDialog()
  : MessageBox(1, 2)
{
}

//ConnectToServerDialog::ConnectToServerDialog(const ConnectToServerDialog& orig) {
//}

ConnectToServerDialog::~ConnectToServerDialog() {
}

void ConnectToServerDialog::EventLoop()
{
  int ch;
  do
  {
    ch = wgetch(m_p_MessageWindow);
//      hardwareFence();
    /** https://linux.die.net/man/3/wgetch :
     * "In no-delay mode, if no input is waiting, the value ERR is returned."
     * ERR = no valid input. */
//    if( ch != ERR )
  }while(ch != '\n' && m_showWindow );
  if( ch == '\n')
    /** Causes the connection attempt in the other thread to cancel. */
    close(m_connectToServerSocketFileDescriptor);
  Destroy();
}

void ConnectToServerDialog::ShowMessage(const char message [], WINDOW * p_windowToShowMessageIn)
{
  m_p_windowToShowMessageIn = p_windowToShowMessageIn;
  BuildUserInterface(message);
  CreateCenteredButton("cancel connecting to service");
//  touchwin(s_bodyWindow);
  wrefresh(m_p_MessageWindow);
}
