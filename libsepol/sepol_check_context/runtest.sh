#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/libsepol/Sanity/sepol_check_context
#   Description: Does sepol_check_context() work as expected?
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
. /usr/bin/rhts-environment.sh || exit 1
. /usr/share/beakerlib/beakerlib.sh || exit 1

PACKAGE="libsepol"
POLICY_PATH_PREFIX="/etc/selinux"

rlJournalStart
    rlPhaseStartSetup
        rlAssertRpm ${PACKAGE}
        rlAssertRpm ${PACKAGE}-devel
        rlRun "gcc -o example -lsepol example.c"
        rlRun "make -f /usr/share/selinux/devel/Makefile"
        rlRun "ls -l testpolicy.pp"
    rlPhaseEnd

    rlPhaseStartTest
        for POLICY_KIND in minimum mls targeted ; do
            if [ -d ${POLICY_PATH_PREFIX}/${POLICY_KIND}/policy ] ; then
                POLICY_PATH=`find ${POLICY_PATH_PREFIX}/${POLICY_KIND}/policy/ -type f -name policy.?? | head -n 1`
                rlRun "semodule -n -s ${POLICY_KIND} -r testpolicy" 0,1
                rlRun "./example ${POLICY_PATH} system_u:object_r:xyz_file_t:s0" 1
                rlRun "semodule -n -s ${POLICY_KIND} -i testpolicy.pp"
                rlRun "./example ${POLICY_PATH} system_u:object_r:xyz_file_t:s0"
                rlRun "semodule -n -s ${POLICY_KIND} -r testpolicy" 0,1
            fi
        done
    rlPhaseEnd

    rlPhaseStartCleanup
        rlRun "rm -f ./example ./testpolicy.pp"
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd

