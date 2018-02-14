#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/checkpolicy/Sanity/sedispol
#   Description: Does sedispol work correctly?
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

PACKAGE="checkpolicy"
# TODO: repeat for all policy files that are installed under /etc/selinux
POLICY_FILE=`find /etc/selinux/targeted/policy/ -type f`

rlJournalStart
    rlPhaseStartSetup
        rlAssertRpm ${PACKAGE}
        OUTPUT_FILE=`mktemp`
    rlPhaseEnd

    rlPhaseStartTest
        if rlIsRHEL 6 ; then
            AVAILABLE_OPTIONS="1 2 3 4 5 6 c p u"
        else # RHEL-7 and above
            AVAILABLE_OPTIONS="1 2 3 4 5 6 8 c p u F"
        fi
        for OPTION in ${AVAILABLE_OPTIONS} ; do
            rlRun "rm -f ${OUTPUT_FILE}"
            rlWatchdog "./sedispol.exp ${OPTION} ${POLICY_FILE} ${OUTPUT_FILE}" 65
            # rlWatchdog kills the expect script, but we need to kill the sedispol process too
            rlRun "killall sedispol" 0,1
            rlRun "ls -l ${OUTPUT_FILE}"
            if [ -s ${OUTPUT_FILE} ] ; then
                rlPass "sedispol produced some output"
            else
                rlFail "sedispol did not produce any output"
            fi
        done
    rlPhaseEnd

    rlPhaseStartTest
        rlRun "echo q | sedispol ${POLICY_FILE} >& ${OUTPUT_FILE}"
        rlRun "grep AVTAB ${OUTPUT_FILE}"
        rlRun "grep AVTAG ${OUTPUT_FILE}" 1
        rlRun "echo -en 'u\nq\n' | sedispol ${POLICY_FILE} >& ${OUTPUT_FILE}"
        rlRun "grep permissions ${OUTPUT_FILE}"
        rlRun "grep permisions ${OUTPUT_FILE}" 1
    rlPhaseEnd

    rlPhaseStartCleanup
        rlRun "rm -f ${OUTPUT_FILE}"
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd

