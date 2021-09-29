
#http://man7.org/linux/man-pages/man5/deb-postinst.5.html :
#"A package can perform several post-installation actions via maintainer
# scripts, by including an executable postinst file in its control archive
# (i.e. DEBIAN/postinst during package creation)."

#http://cmake.org/cmake/help/v3.6/module/CPackDeb.html :
#"This variable allow advanced user to add custom script to the
# control.tar.gz. Typical usage is for conffiles, postinst, postrm, prerm."
#"Note The original permissions of the files will be used in the final
# package unless the variable CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION
# is set. In particular, the scripts should have the proper executable flag
# prior to the generation of the package."
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
"${PROJECT_SOURCE_DIR}/../Linux/Debian/postinst")

#http://askubuntu.com/questions/838270/access-files-of-a-debian-package-from-postinst-script :
# $DPKG_MAINTSCRIPT_PACKAGE

#http://cmake.org/cmake/help/v3.6/module/CPackDeb.html :
#"Note This overrides the permissions on the original files, following the
# rules set by Debian policy
#https://www.debian.org/doc/debian-policy/ch-files.html#s-permissions-owners"
set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION TRUE)

