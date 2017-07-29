# Persistent global definitions go here
include /etc/firejail/globals.local

# This file is overwritten during software install.
# Persistent customizations should go in a .local file.
include /etc/firejail/pingus.local

################################
# Pinugs profile
################################

noblacklist ~/.pingus
mkdir ~/.pingus
whitelist ~/.pingus
include /etc/firejail/whitelist-common.inc

include /etc/firejail/disable-common.inc
include /etc/firejail/disable-programs.inc
include /etc/firejail/disable-passwdmgr.inc

caps.drop all
nonewprivs
noroot
protocol unix,netlink
seccomp

#
# depending on your usage, you can enable some of the commands below:
#
net none
nogroups
shell none
#private-bin pingus
# private-etc none
private-dev
private-tmp
# nosound




