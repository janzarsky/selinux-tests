#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/libsemanage/Sanity/semanage-handle-functions
#   Description: Test functions from handle.h
#   Author: Jan Zarsky <jzarsky@redhat.com>
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   Copyright (c) 2017 Red Hat, Inc.
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
. /usr/bin/rhts-environment.sh || exit 1
. /usr/share/beakerlib/beakerlib.sh || exit 1

PACKAGE="libsemanage"

rlJournalStart
    rlPhaseStartSetup
        rlAssertRpm ${PACKAGE}
        rlAssertRpm ${PACKAGE}-devel
        rlAssertRpm "glibc"
        rlAssertRpm "gcc"

        if rlIsRHEL ">=7" || rlIsFedora; then
            rlRun -l "gcc test_root.c           -o test_root            -lsemanage -Wall -Wextra -std=c99"
        fi

        rlRun -l "gcc test_handle_create.c  -o test_handle_create   -lsemanage -Wall -Wextra -Wno-unused-parameter -std=c99"
        rlRun -l "gcc test_access_check.c   -o test_access_check    -lsemanage -Wall -Wextra -std=c99"
        rlRun -l "gcc test_is_managed.c     -o test_is_managed      -lsemanage -Wall -Wextra -std=c99"
        rlRun -l "gcc test_connect.c        -o test_connect         -lsemanage -Wall -Wextra -std=c99"
        rlRun -l "gcc test_is_connected.c   -o test_is_connected    -lsemanage -Wall -Wextra -std=c99"
        rlRun -l "gcc test_mls_enabled.c    -o test_mls_enabled     -lsemanage -Wall -Wextra -std=c99"
        rlRun -l "gcc test_transaction.c    -o test_transaction     -lsemanage -Wall -Wextra -std=c99"

        ERR_FAIL=1
        ERR_ABORT=134
    rlPhaseEnd

    if rlIsRHEL ">=7" || rlIsFedora; then
    rlPhaseStartTest "semanage_root, semanage_test_root"
        rlRun "./test_root init"
        rlRun "./test_root handle"
        rlRun "./test_root conn"
        rlRun "./test_root init /somepath"
        rlRun "./test_root handle /somepath"
        rlRun "./test_root conn /somepath"
    rlPhaseEnd
    fi

    rlPhaseStartTest "semanage_handle_create, semanage_handle_destroy"
        rlRun "./test_handle_create init"
    rlPhaseEnd

    rlPhaseStartTest "semanage_access_check"
        rlRun "./test_access_check init" $ERR_ABORT
        rlRun "./test_access_check handle 2"
        rlRun "./test_access_check conn 2"
    rlPhaseEnd

    rlPhaseStartTest "semanage_is_managed"
        rlRun "./test_is_managed init" $ERR_ABORT
        rlRun "./test_is_managed handle 1"
        rlRun "./test_is_managed conn" $ERR_FAIL
    rlPhaseEnd

    rlPhaseStartTest "semanage_connect, semanage_disconnect"
        rlRun "./test_connect init" $ERR_ABORT
        rlRun "./test_connect init reversed" $ERR_ABORT
        rlRun "./test_connect handle"
        rlRun "./test_connect handle twice"
        rlRun "./test_connect handle reversed" $ERR_ABORT
        # why does it work??
        rlRun "./test_connect conn"
    rlPhaseEnd

    rlPhaseStartTest "semanage_is_connected"
        rlRun "./test_is_connected init" $ERR_ABORT
        rlRun "./test_is_connected handle 0"
        rlRun "./test_is_connected conn 1"
    rlPhaseEnd

    rlPhaseStartTest "semanage_mls_enabled"
        rlRun "./test_mls_enabled init" $ERR_ABORT
        rlRun "./test_mls_enabled handle" $ERR_ABORT
        rlRun "./test_mls_enabled conn 1"
    rlPhaseEnd

    rlPhaseStartTest "semanage_begin_transaction, semanage_commit"
        rlRun "./test_transaction init" $ERR_ABORT
        rlRun "./test_transaction init reversed" $ERR_ABORT
        rlRun "./test_transaction handle" $ERR_ABORT
        rlRun "./test_transaction handle reversed" $ERR_ABORT
        rlRun "./test_transaction conn"
        rlRun "./test_transaction conn twice"
        rlRun "./test_transaction conn reversed" $ERR_FAIL
    rlPhaseEnd

    rlPhaseStartCleanup
        rlRun "rm -f output test_root test_handle_create test_access_check \
               test_is_managed test_connect test_is_connected \
               test_mls_enabled test_transaction"
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd
