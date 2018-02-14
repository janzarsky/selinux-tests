#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/libselinux/Sanity/selinux_sestatus-functions
#   Description: Test sestatus.c functions
#   Author: Jan Zarsky <jzarsky@redhat.com>
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   Copyright (c) 2016 Red Hat, Inc.
#
#   This program is free software: you can redistribute it and/or
#   modify it under the terms of the GNU General Public License as
#   published by the Free Software Foundation, either version 2 of
#   the License, or (at your option) any later version.
#
#   This program is distributed in the hope that it will be
#   useful, but WITHOUT ANY WARRANTY; without even the implied
#   warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#   PURPOSE.  See the GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program. If not, see http://www.gnu.org/licenses/.
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# Include Beaker environment
. /usr/share/beakerlib/beakerlib.sh || exit 1

PACKAGE="libselinux"

rlJournalStart
    rlPhaseStartSetup
        rlAssertRpm ${PACKAGE}
        rlAssertRpm ${PACKAGE}-devel
        rlAssertRpm "glibc"

        rlRun -l "gcc test.c -o test -lselinux -pedantic -Wall -Wextra -std=c99"
    rlPhaseEnd

    rlPhaseStartTest
        rlRun "./test > res.txt"
        rlRun -l "cat res.txt"

        SELINUX_MNT=$(cat /proc/mounts | grep selinux | cut -d " " -f 2)

        rlRun "grep \"(before open) selinux_status_getenforce -1\" res.txt"
        rlRun "grep \"(before open) selinux_status_policyload -1\" res.txt"
        rlRun "grep \"(before open) selinux_status_deny_unknown -1\" res.txt"
        rlRun "grep \"(before open) selinux_status_updated -1\" res.txt"
        
        rlRun "grep \"selinux_status_open 0\" res.txt"
        rlRun "grep \"selinux_status_getenforce $(cat $SELINUX_MNT/enforce)\" res.txt"
        rlRun "grep -E \"selinux_status_policyload [0-9]\" res.txt"
        rlRun "grep \"selinux_status_deny_unknown $(cat $SELINUX_MNT/deny_unknown)\" res.txt"
        rlRun "grep \"selinux_status_updated 0\" res.txt"
        rlRun "grep \"selinux_status_close void\" res.txt"
    rlPhaseEnd

    rlPhaseStartCleanup
        rlRun "rm -f test"
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd
