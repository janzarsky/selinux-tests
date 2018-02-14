#!/bin/bash
# vim: dict=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/policycoreutils/Sanity/sestatus
#   Description: tests everything about sestatus
#   Author: Milos Malik <mmalik@redhat.com>
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   Copyright (c) 2011 Red Hat, Inc. All rights reserved.
#
#   This copyrighted material is made available to anyone wishing
#   to use, modify, copy, or redistribute it subject to the terms
#   and conditions of the GNU General Public License version 2.
#
#   This program is distributed in the hope that it will be
#   useful, but WITHOUT ANY WARRANTY; without even the implied
#   warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#   PURPOSE. See the GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public
#   License along with this program; if not, write to the Free
#   Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
#   Boston, MA 02110-1301, USA.
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# Include rhts environment
. /usr/bin/rhts-environment.sh
. /usr/share/beakerlib/beakerlib.sh

PACKAGE="policycoreutils"
if rlIsRHEL 5 6 ; then
    SELINUX_FS_MOUNT="/selinux"
else # RHEL-7 and above
    SELINUX_FS_MOUNT="/sys/fs/selinux"
fi

rlJournalStart
    rlPhaseStartSetup
        rlAssertRpm ${PACKAGE}
        rlFileBackup /etc/sestatus.conf
        rlRun "mount | grep -i selinux" 0,1
        OUTPUT_FILE=`mktemp`
    rlPhaseEnd

    rlPhaseStartTest "basic use"
        rlRun "sestatus"
        rlRun "sestatus -b 2>&1 | tee ${OUTPUT_FILE}"
        rlAssertGrep "policy booleans" ${OUTPUT_FILE} -i
        rlRun "sestatus -v 2>&1 | tee ${OUTPUT_FILE}"
        rlAssertGrep "process contexts" ${OUTPUT_FILE} -i
        rlAssertGrep "file contexts" ${OUTPUT_FILE} -i
        rlAssertGrep "current context" ${OUTPUT_FILE} -i
        rlAssertGrep "init context" ${OUTPUT_FILE} -i
        rlAssertGrep "controlling term" ${OUTPUT_FILE} -i
        rlRun "sestatus --xyz 2>&1 | tee ${OUTPUT_FILE}"
        rlAssertGrep "invalid option" ${OUTPUT_FILE} -i
    rlPhaseEnd

    rlPhaseStartTest "extreme cases"
        # pretend that the config file contains an invalid section
        rlRun "sed -i 's/files/xyz/' /etc/sestatus.conf"
        rlRun "sestatus -v 2>&1 | tee ${OUTPUT_FILE}"
        rlAssertGrep "line not in a section" ${OUTPUT_FILE} -i
        rlRun "rm -f /etc/sestatus.conf"
        rlRun "mkdir /etc/sestatus.conf" # intentionally replaced a file with a directory
        rlRun "sestatus -v"
        # pretend that the config file is missing
        rlRun "rm -rf /etc/sestatus.conf"
        for OPTION in "-bv" "-v" ; do
            rlRun "sestatus ${OPTION} 2>&1 | tee ${OUTPUT_FILE}"
            rlAssertGrep "unable to open /etc/sestatus.conf" ${OUTPUT_FILE} -i
        done
        rlFileRestore
        # pretend that SELinux is disabled
        rlRun "umount ${SELINUX_FS_MOUNT}"
        for OPTION in "" "-b" "-v" "-bv" ; do
            rlRun "sestatus ${OPTION} 2>&1 | tee ${OUTPUT_FILE}"
            rlAssertGrep "selinux status.*disabled" ${OUTPUT_FILE} -i
        done
        rlRun "mount -t selinuxfs none ${SELINUX_FS_MOUNT}"
        # pretend that no booleans are defined
        rlRun "mkdir ./booleans"
        rlRun "mount --bind ./booleans ${SELINUX_FS_MOUNT}/booleans"
        rlRun "sestatus -b 2>&1 | tee ${OUTPUT_FILE}"
        rlRun "umount ${SELINUX_FS_MOUNT}/booleans"
        rlAssertNotGrep "booleans" ${OUTPUT_FILE} -i
        rlRun "rmdir ./booleans"
    rlPhaseEnd

    # This bug is not worth fixing in RHEL-5
    if ! rlIsRHEL 5 ; then
    rlPhaseStartTest
        rlRun "rpm -ql ${PACKAGE} | grep /usr/sbin/sestatus"
        rlRun "rpm -ql ${PACKAGE} | grep /usr/share/man/man8/sestatus.8"
        for OPTION in b v ; do
            rlRun "sestatus --help 2>&1 | grep -- -${OPTION}"
            rlRun "man sestatus | col -b | grep -- -${OPTION}"
        done
        if ! rlIsRHEL 6 ; then
            rlRun "man -w sestatus.conf"
        fi
    rlPhaseEnd
    fi

    rlPhaseStartCleanup
        rlFileRestore
        rm -f ${OUTPUT_FILE}
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd

