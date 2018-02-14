#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/libselinux/Sanity/realpath_not_final-function
#   Description: Test realpath_not_final function
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
        rlAssertRpm "gcc"

        rlRun -l "gcc test.c -o test -lselinux -Wall -Wextra -std=c99"
    rlPhaseEnd

    rlPhaseStartTest
        # syntax: ./test name [resolved_path]
        rlRun "./test NULL" 139
        rlRun "./test /somedir/somefile NULL" 255
        rlRun "./test NULL NULL" 139

        rlRun "./test /tmp | tee output"
        rlRun "grep 'realpath_not_final: /tmp' output"

        rlRun "./test //tmp | tee output"
        rlRun "grep -E 'realpath_not_final: /tmp|realpath_not_final: //tmp' output"

        rlRun "./test ///tmp | tee output"
        rlRun "grep -E 'realpath_not_final: /tmp|realpath_not_final: //tmp' output"
        
        rlRun "./test ////tmp | tee output"
        rlRun "grep -E 'realpath_not_final: /tmp|realpath_not_final: //tmp' output"
    rlPhaseEnd

    rlPhaseStartCleanup
        rlRun "rm -f test output"
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd
