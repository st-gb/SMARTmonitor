set(CmnSrcCurses ${COMMON_SOURCECODE_ROOT_PATH}/libraries/curses)
set(SOURCE_FILE_PATHS
  ${SOURCE_FILE_PATHS}
  ${PROJECT_SOURCE_DIR}/curses/NcursesUI.cpp
  ${PROJECT_SOURCE_DIR}/curses/ConnectToServerDialog.cpp
  ${PROJECT_SOURCE_DIR}/curses/MainWindow.cpp
  ${CmnSrcCurses}/color.c
  ${CmnSrcCurses}/EventQueue.cpp
  ${CmnSrcCurses}/GetChar.cpp
  ${CmnSrcCurses}/UIcontrols/MessageBox.cpp
  ${CmnSrcCurses}/textfield.cpp
#  ${COMMON_SOURCECODE_ROOT_PATH}/libraries/ncurses/form/Form.cpp
  ${COMMON_SOURCECODE_ROOT_PATH}/libraries/ncurses/menu/Menu.cpp
  ${CmnSrcCurses}/InputProcessorStack.cpp
  ${CmnSrcCurses}/windows/Window.cpp
)
set( LIBRARIES
  ${LIBRARIES}
  ncurses menu form)
set(SMARTmonClient ON)
