#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/policycoreutils/Sanity/load_policy
#   Description: Does load_policy work as expected? Does it produce correct audit messages?
#   Author: Milos Malik <mmalik@redhat.com>
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   Copyright (c) 2016 Red Hat, Inc.
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

# Include Beaker environment
. /usr/bin/rhts-environment.sh || exit 1
. /usr/share/beakerlib/beakerlib.sh || exit 1

PACKAGE="policycoreutils"
if rlIsRHEL 6 ; then
    SELINUX_FS_MOUNT="/selinux"
else # RHEL-7 and above
    SELINUX_FS_MOUNT="/sys/fs/selinux"
fi

rlJournalStart
    rlPhaseStartSetup
        rlAssertRpm ${PACKAGE}
        rlRun "ls -l `which load_policy`"
        BINARY_POLICY=`find /etc/selinux/targeted -type f -name policy.?? | sort -n | tail -n 1`
        rlRun "ls -l ${BINARY_POLICY}"
    rlPhaseEnd

    rlPhaseStartTest
        rlRun "load_policy --xyz 2>&1 | grep \"invalid option\""
        rlRun "dmesg | grep -i selinux" 0,1
        rlRun "grep -i selinux /proc/mounts"
        START_DATE_TIME=`date "+%m/%d/%Y %T"`
        sleep 1
        rlRun "load_policy -q"
        rlRun "grep -i selinux /proc/mounts"
        sleep 1
        if rlIsRHEL ; then
            rlRun "ausearch -m MAC_POLICY_LOAD -i -ts ${START_DATE_TIME} | grep load_policy"
        fi
        rlRun "ausearch -m MAC_POLICY_LOAD -i -ts ${START_DATE_TIME} | grep 'policy loaded'"
        rlRun "umount ${SELINUX_FS_MOUNT}"
        rlRun "grep -i selinux /proc/mounts" 1
        START_DATE_TIME=`date "+%m/%d/%Y %T"`
        sleep 1
        rlRun "load_policy -i ${BINARY_POLICY}"
        rlRun "grep -i selinux /proc/mounts"
        sleep 1
        if rlIsRHEL ; then
            rlRun "ausearch -m MAC_POLICY_LOAD -i -ts ${START_DATE_TIME} | grep load_policy"
        fi
        rlRun "ausearch -m MAC_POLICY_LOAD -i -ts ${START_DATE_TIME} | grep 'policy loaded'"
        rlRun "dmesg | grep -i selinux"
    rlPhaseEnd

    rlPhaseStartCleanup
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd

