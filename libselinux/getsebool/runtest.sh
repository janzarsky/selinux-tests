#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/libselinux/Sanity/getsebool
#   Description: Does getsebool work as expected?
#   Author: Milos Malik <mmalik@redhat.com>
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   Copyright (c) 2017 Red Hat, Inc.
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
        rlRun "getsebool" 0,1
        OUTPUT_FILE=`mktemp`
    rlPhaseEnd

    rlPhaseStartTest 
        rlRun "getsebool -a"
        rlRun "umount ${SELINUX_FS_MOUNT}"
        rlRun "getsebool -a 2>&1 | tee ${OUTPUT_FILE}"
        rlAssertGrep "selinux.*disabled" ${OUTPUT_FILE} -i
        rlRun "mount -t selinuxfs none ${SELINUX_FS_MOUNT}"
        rlRun "mkdir booleans"
        rlRun "mount --bind ./booleans ${SELINUX_FS_MOUNT}/booleans"
        rlRun "getsebool -a 2>&1 | tee ${OUTPUT_FILE}"
        rlAssertGrep "unable to get boolean name.*no such file or directory" ${OUTPUT_FILE} -i
        rlRun "getsebool xen_use_nfs 2>&1 | tee ${OUTPUT_FILE}"
        rlAssertGrep "error getting active value for" ${OUTPUT_FILE} -i
        rlRun "umount ${SELINUX_FS_MOUNT}/booleans"
        rlRun "rmdir booleans"
    rlPhaseEnd

    rlPhaseStartCleanup
        rm -f ${OUTPUT_FILE}
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd

