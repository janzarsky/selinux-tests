#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/libselinux/Sanity/selinux_restorecon
#   Description: Does selinux_restorecon work correctly?
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

rlJournalStart
    rlPhaseStartSetup
        rlAssertRpm ${PACKAGE}
        rlAssertRpm ${PACKAGE}-utils
    rlPhaseEnd

    rlPhaseStartTest "valid options / combinations"
        rlRun "selinux_restorecon --help" 0-255
        for CUR_DIR in /boot /etc /opt /root /usr /var ; do
            rlRun "setfattr -x security.restorecon_last ${CUR_DIR}" 0,1
            rlRun "getfattr -m . -d ${CUR_DIR} | grep security.restorecon_last=" 1
            rlRun "selinux_restorecon -R -d -C ${CUR_DIR} 2>&1 | grep -i \"updated digest for.*${CUR_DIR}\"" 1
            rlRun "getfattr -m . -d ${CUR_DIR} | grep security.restorecon_last="
            rlRun "selinux_restorecon -R -v -d -C ${CUR_DIR} 2>&1 | grep -i \"updated digest for.*${CUR_DIR}\""
        done
        rlRun "chcon -u unconfined_u /root"
        rlRun "ls -dZ /root | grep unconfined_u:"
        rlRun "selinux_restorecon -R -v -d -C /root 2>&1 | grep -i relabeled" 1
        rlRun "ls -dZ /root | grep unconfined_u:"
        rlRun "selinux_restorecon -R -v -d -C -F /root 2>&1 | grep -i relabeled"
        rlRun "ls -dZ /root | grep system_u:"
    rlPhaseEnd

    rlPhaseStartTest "invalid options / combinations"
        rlRun "selinux_restorecon -v -P 2>&1 | grep -i \"mutually exclusive\""
        rlRun "selinux_restorecon -f 2>&1 | grep -i \"option requires an argument\""
        rlRun "selinux_restorecon -p 2>&1 | grep -i \"option requires an argument\""
        rlRun "selinux_restorecon /non-existent 2>&1 | grep -i \"No such file or directory\""
    rlPhaseEnd

    rlPhaseStartTest
        OUTPUT_FILE=`mktemp`
        rlRun "selinux_restorecon -R -v -C /root 2>&1 | tee ${OUTPUT_FILE}"
        rlRun "grep -q -e /sys/fs/selinux/ -e /sys/kernel/security/ ${OUTPUT_FILE}" 1
        rlRun "selinux_restorecon -R -v -C -r /root 2>&1 | tee ${OUTPUT_FILE}"
        rlRun "grep -q -e /sys/fs/selinux/ -e /sys/kernel/security/ ${OUTPUT_FILE}" 1
        rm -f ${OUTPUT_FILE}
    rlPhaseEnd

    rlPhaseStartCleanup
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd

