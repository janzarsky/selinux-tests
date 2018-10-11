#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/libselinux/Sanity/setenforce
#   Description: Does setenforce work as expected? Does it produce correct audit messages?
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
. /usr/share/beakerlib/beakerlib.sh || exit 1

PACKAGE="libselinux"
if rlIsRHEL 5 6 ; then
    SELINUX_FS_MOUNT="/selinux"
else # RHEL-7 and above
    SELINUX_FS_MOUNT="/sys/fs/selinux"
fi

rlJournalStart
    rlPhaseStartSetup
        rlAssertRpm ${PACKAGE}
        rlAssertRpm ${PACKAGE}-utils
        rlRun "setenforce --help" 0,1
        OUTPUT_FILE=`mktemp`
        export LC_ALL=en_US.utf8
        rlRun "TmpDir=\$(mktemp -d)" 0 "Creating tmp directory"
        rlRun "auditctl -l | sed 's/^No rules$/# No rules/' > $TmpDir/auditctl" 0 "Backup current audit rules"
        rlRun "auditctl -D" 0 "Delete all audit rules"
    rlPhaseEnd

    rlPhaseStartTest "basic use"
        START_DATE_TIME=`date "+%m/%d/%Y %T"`
        sleep 1
        rlRun "setenforce 1"
        rlRun "grep 1 ${SELINUX_FS_MOUNT}/enforce"
        rlRun "setenforce 0"
        rlRun "grep 0 ${SELINUX_FS_MOUNT}/enforce"
        rlRun "setenforce 1"
        sleep 5
        rlRun "ausearch --input-logs -m MAC_STATUS -i -ts ${START_DATE_TIME} | grep 'type=MAC_STATUS.*enforcing=1.*old_enforcing=0'"
        rlRun "ausearch --input-logs -m MAC_STATUS -i -ts ${START_DATE_TIME} | grep 'type=MAC_STATUS.*enforcing=0.*old_enforcing=1'"
        rlRun "ausearch --input-logs -m MAC_STATUS -i -ts ${START_DATE_TIME} | grep 'type=SYSCALL.*comm=setenforce'"
    rlPhaseEnd

    rlPhaseStartTest "extreme cases"
        rlRun "umount ${SELINUX_FS_MOUNT}"
        for OPTION in 1 0 Enforcing Permissive ; do
            rlRun "setenforce ${OPTION} 2>&1 | tee ${OUTPUT_FILE}"
            rlAssertGrep "selinux.*disabled" ${OUTPUT_FILE} -i
        done
        rlRun "mount -t selinuxfs none ${SELINUX_FS_MOUNT}"
        rlRun "touch ./enforce"
        rlRun "chattr +i ./enforce"
        rlRun "mount --bind ./enforce ${SELINUX_FS_MOUNT}/enforce"
        for OPTION in 1 0 Enforcing Permissive ; do
            rlRun "setenforce ${OPTION} 2>&1 | tee ${OUTPUT_FILE}"
            rlAssertGrep "setenforce.*failed" ${OUTPUT_FILE} -i
        done
        rlRun "umount ${SELINUX_FS_MOUNT}/enforce"
        rlRun "chattr -i ./enforce"
        rlRun "rm -f ./enforce"
    rlPhaseEnd

    rlPhaseStartCleanup
        rlRun "auditctl -R $TmpDir/auditctl" 0 "Restore audit rules"
        rm -f ${OUTPUT_FILE}
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd

