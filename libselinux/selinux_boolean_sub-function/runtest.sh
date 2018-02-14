#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/libselinux/Sanity/selinux_boolean_sub-function
#   Description: Test selinux_boolean_sub function
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
        rlRun "./test NULL | tee output"
        rlRun "grep 'selinux_boolean_sub: (null)' output"

        rlRun "./test my_nonexisting_record | tee output"
        rlRun "grep 'selinux_boolean_sub: my_nonexisting_record' output"
    
        policy_type="$(grep -E '^SELINUXTYPE=' /etc/selinux/config | cut -c13- | tr '[:upper:]' '[:lower:]' | tr -d ' ')"
        line1="$(cat /etc/selinux/$policy_type/booleans.subs_dist | head -n 1)"
        line2="$(cat /etc/selinux/$policy_type/booleans.subs_dist | head -n 7 | tail -n 1)"
        line3="$(cat /etc/selinux/$policy_type/booleans.subs_dist | tail -n 1)"

        input="$(echo $line1 | awk '{ print $1 }')"
        output="$(echo $line1 | awk '{ print $2 }')"

        rlRun "./test $input | tee output"
        rlRun "grep 'selinux_boolean_sub: $output' output"

        input="$(echo $line2 | awk '{ print $1 }')"
        output="$(echo $line2 | awk '{ print $2 }')"

        rlRun "./test $input | tee output"
        rlRun "grep 'selinux_boolean_sub: $output' output"

        input="$(echo $line3 | awk '{ print $1 }')"
        output="$(echo $line3 | awk '{ print $2 }')"

        rlRun "./test $input | tee output"
        rlRun "grep 'selinux_boolean_sub: $output' output"
    rlPhaseEnd

    rlPhaseStartCleanup
        rlRun "rm -f test output"
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd
