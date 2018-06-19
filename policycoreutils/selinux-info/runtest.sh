#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/policycoreutils/Sanity/selinux-info
#   Description: Test sestatus, avcstat, getenforce and other information gathering tools
#   Author: Jan Zarsky <jzarsky@redhat.com>
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   Copyright (c) 2018 Red Hat, Inc.
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

PACKAGE="policycoreutils"

rlJournalStart
    rlPhaseStartSetup
        rlAssertRpm $PACKAGE

        ENFORCING=$(cat /sys/fs/selinux/enforce)
        MLS=$(cat /sys/fs/selinux/mls)
        DENY=$(cat /sys/fs/selinux/deny_unknown)
        POLICYVERSION=$(cat /sys/fs/selinux/policyvers)
        POLICY_TYPE=$(grep -E '^SELINUXTYPE=' /etc/selinux/config | cut -f2 -d'=')
        MODE=$(grep -E '^SELINUX=' /etc/selinux/config | cut -f2 -d'=')

        BOOL_ON="mount_anyfile"
        BOOL_OFF="deny_ptrace"
        PROC_CON="unconfined_u:unconfined_r:unconfined_t:s0"
        INIT_CON="system_u:system_r:init_t:s0"
        # generic context:
        CON="[[:alpha:]_]*_u:[[:alpha:]_]*_r:[[:alpha:]_]*_t:s[[:digit:]]*"
    rlPhaseEnd

    rlPhaseStartTest "sestatus"
        rlRun "sestatus >stdout"
        rlAssertGrep "SELinux status: *enabled" stdout
        rlAssertGrep "SELinuxfs mount: */sys/fs/selinux" stdout
        rlAssertGrep "SELinux root directory: *[a-zA-Z0-9/]*" stdout
        rlAssertGrep "Loaded policy name: $POLICYTYPE" stdout
        if [[ $ENFORCING ]]; then
            rlAssertGrep "Current mode: *enforcing" stdout
        else
            rlAssertGrep "Current mode: *permissive" stdout
        fi
        rlAssertGrep "Mode from config file: *$MODE" stdout
        if [[ $MLS ]]; then
            rlAssertGrep "Policy MLS status: *enabled" stdout
        else
            rlAssertGrep "Policy MLS status: *disabled" stdout
        fi
        if [[ $DENY ]]; then
            rlAssertGrep "Policy deny_unknown status: *allowed" stdout
        else
            rlAssertGrep "Policy deny_unknown status: *denied" stdout
        fi
        rlAssertGrep "Max kernel policy version: *$VERS" stdout

        rlRun "sestatus -b >stdout"
        rlAssertGrep "Policy booleans:" stdout
        rlAssertGrep "$BOOL_ON *on" stdout
        rlAssertGrep "$BOOL_OFF *off" stdout

        rlRun "sestatus -v >stdout"
        rlAssertGrep "Process contexts:" stdout
        rlAssertGrep "Current context: *$PROC_CON" stdout
        rlAssertGrep "Init context: *$INIT_CON" stdout
        rlAssertGrep "File contexts:" stdout
        # Disable this test as there's no controlling terminal when it's run via ansible
        # rlAssertGrep "Controlling terminal: *$CON" stdout
        rlAssertGrep "/etc/passwd *$CON" stdout
    rlPhaseEnd

    rlPhaseStartTest "avcstat"
        rlRun "avcstat >stdout"
        rlAssertGrep " *lookups *hits *misses *allocs *reclaims *frees" stdout
        rlAssertGrep " *[[:digit:]]* *[[:digit:]]* *[[:digit:]]* *[[:digit:]]* *[[:digit:]]* *[[:digit:]]*" stdout

        rlRun "avcstat >stdout2"
        rlAssertDiffer stdout stdout2
    rlPhaseEnd

    rlPhaseStartTest "getenforce"
        rlRun "getenforce >stdout"
        if [[ $ENFORCING ]]; then
            rlAssertGrep "Enforcing" stdout
        else
            rlAssertGrep "Permissive" stdout
        fi

        rlRun "setenforce 0"
        rlRun "getenforce | grep Permissive"
        rlRun "setenforce 1"
        rlRun "getenforce | grep Enforcing"

        if [[ ! $ENFORCING ]]; then
            rlRun "setenforce 0"
        fi
    rlPhaseEnd

    rlPhaseStartCleanup
        rlRun "rm stdout stdout2"
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd
