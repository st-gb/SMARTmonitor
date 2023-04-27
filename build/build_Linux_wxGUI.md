# Buildung wxWidgets GUI of S. Gebauer's(TU Berlin matriculation number 361095)
SMARTmonitor application under Linux

- Debian Linux-/APT-based:
  - libwxbase[...]dev is for CMake variable "wxWidgets_LIBRARIES"
  - command line to install [wxWidgets](https://www.wxwidgets.org/) runtime
    libraries (as root/superuser, that means use "sudo " before command):

    ```
    apt-get install libwxbase3.0-0v5
    apt-get install libwxgtk3.0-0v5
    ```

    - header files:

    ```
    apt-get install libwxbase3.0-dev  
    apt-get install libwxgtk3.0-dev
    ```

    ([...]gtk[...] is "adv" library for Linux/GTK)
    
  - Ubuntu Desktop 22.4.1 64 bit:

    ```
    apt-get install libwxgtk3.0-gtk3-0v5
    apt-get install libwxbase3.0-dev
    ```

  - command line to build (assuming working directory is SMARTmonitor
  -  repository's root directory):

    ```
    cd src
    ../create/create_wxGUI_Linux_debug.sh "Unix Makefiles"
    make -j `nproc`
    ```
- Arch Linux

    Command line to install it:

        pacman -S wxgtk-common
        pacman -S wxgtk2

    wxgtk2: for findWixwidgets.cmake, although not shown with "pacman -Ql wxgtk2" 
    wxgtk-common: header files, wxbase .so files