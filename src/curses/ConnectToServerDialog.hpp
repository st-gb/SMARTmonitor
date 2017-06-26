/** File:   ConnectToServerDialog.hpp
 * Author: sg
 * Created on 26. Juni 2017, 10:27 */

#ifndef CONNECTTOSERVERDIALOG_HPP
#define CONNECTTOSERVERDIALOG_HPP

#include <libraries/curses/MessageBox.hpp> //class Curses::MessageBox

class ConnectToServerDialog
  : public Curses::MessageBox
{
  int m_connectToServerSocketFileDescriptor;
public:
  ConnectToServerDialog();
  ConnectToServerDialog(const ConnectToServerDialog& orig);
  virtual ~ConnectToServerDialog();
  void EventLoop();
  void SetSocketFileDescriptor(int socketFileDescriptor) { 
    m_connectToServerSocketFileDescriptor = socketFileDescriptor; }

  void ShowMessage(const char message [], WINDOW * p_windowToShowMessageIn);
private:
};

#endif /* CONNECTTOSERVERDIALOG_HPP */

