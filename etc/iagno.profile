# Firejail profile for iagno
# Description: Reversi clone for Gnome desktop
# This file is overwritten after every install/update
# Persistent local customizations
include iagno.local
# Persistent global definitions
include globals.local

include disable-common.inc
include disable-devel.inc
include disable-exec.inc
include disable-interpreters.inc
include disable-passwdmgr.inc
include disable-programs.inc

private

apparmor
caps.drop all
net none
#nodbus
nodvd
nogroups
nonewprivs
noroot
notv
nou2f
novideo
protocol unix
seccomp
shell none

disable-mnt
private-bin iagno
private-dev
private-tmp
