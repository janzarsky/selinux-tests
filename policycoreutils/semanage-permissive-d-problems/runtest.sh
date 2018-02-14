#!/bin/bash
# vim: dict=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/policycoreutils/Regression/semanage-permissive-d-problems
#   Description: semanage permissive -d accepts more than domain types, its behavior is not reliable
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

rlJournalStart
    rlPhaseStartSetup
        rlAssertRpm ${PACKAGE}
        rlRun "rpm -qf /usr/sbin/semanage"
        OUTPUT_FILE=`mktemp`
        rlRun "sestatus"
    rlPhaseEnd

    if selinuxenabled ; then
        rlPhaseStartTest
            if rlIsRHEL 7 ; then
                rlFileBackup /usr/share/selinux/default/Makefile
                rlRun "rm -rf /usr/share/selinux/default/Makefile"
            fi
            rlRun "semanage permissive -l | grep fenced" 1
            rlRun "semanage permissive -a fenced_t"
            rlRun "semanage permissive -l | grep fenced"
            rlRun "semanage permissive -d fenced_t"
            rlRun "semanage permissive -l | grep fenced" 1
            if rlIsRHEL 7 ; then
                rlFileRestore
            fi
        rlPhaseEnd

        rlPhaseStartTest
            rlRun "semanage permissive -l 2>&1 | grep -e ypserv_t -e ypbind_t | tee ${OUTPUT_FILE}"
            rlRun "wc -l < ${OUTPUT_FILE} | grep ^0$"
            rlRun "semanage permissive -a ypbind_t"
            rlRun "semanage permissive -a ypserv_t"
            rlRun "semanage permissive -l 2>&1 | grep -e ypserv_t -e ypbind_t | tee ${OUTPUT_FILE}"
            rlRun "wc -l < ${OUTPUT_FILE} | grep ^2$"
            rlRun "semanage permissive -d yp" 1-255
            rlRun "semanage permissive -l 2>&1 | grep -e ypserv_t -e ypbind_t | tee ${OUTPUT_FILE}"
            rlRun "wc -l < ${OUTPUT_FILE} | grep ^2$"
            rlRun "semanage permissive -d ypbind_t"
            rlRun "semanage permissive -d ypserv_t"
            rlRun "semanage permissive -l 2>&1 | grep -e ypserv_t -e ypbind_t | tee ${OUTPUT_FILE}"
            rlRun "wc -l < ${OUTPUT_FILE} | grep ^0$"
        rlPhaseEnd

        rlPhaseStartTest
            rlRun -s "semanage permissive -d" 1
            rlAssertNotGrep 'traceback' $rlRun_LOG -iEq
            rlAssertGrep 'error: the following argument is required: type' $rlRun_LOG -iEq
            rm -f $rlRun_LOG
        rlPhaseEnd
    else
        rlPhaseStartTest
            rlRun "semanage permissive -l >& ${OUTPUT_FILE}" 0,1
            rlRun "grep -C 32 -i -e exception -e traceback -e error ${OUTPUT_FILE}" 1
        rlPhaseEnd
    fi

    rlPhaseStartCleanup
        rm -f ${OUTPUT_FILE}
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd

