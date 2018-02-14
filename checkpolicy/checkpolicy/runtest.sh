#!/bin/bash
# vim: dict=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/checkpolicy/Sanity/checkpolicy
#   Description: runs checkpolicy with various options to find out if it behaves correctly
#   Author: Milos Malik <mmalik@redhat.com>
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   Copyright (c) 2009 Red Hat, Inc. All rights reserved.
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

# Include rhts environment
. /usr/bin/rhts-environment.sh
. /usr/share/beakerlib/beakerlib.sh

PACKAGE="checkpolicy"

rlJournalStart
    rlPhaseStartSetup
        rlAssertRpm ${PACKAGE}
        rlAssertRpm selinux-policy-minimum
        rlAssertRpm selinux-policy-mls
        rlAssertRpm selinux-policy-targeted
        rlRun "uname -a"
        TEST_FILE=`mktemp`
        TEST_DIR=`mktemp -d`
        OUTPUT_FILE=`mktemp`
        rlAssertExists "/usr/bin/checkpolicy"
    rlPhaseEnd

    rlPhaseStartTest "compilation from policy.conf"
        MIN_VERSION="15"
        MAX_VERSION=`find /etc/selinux/ -name policy.?? | cut -d / -f 6 | cut -d . -f 2 | head -n 1`
        if rlIsRHEL 5 6 ; then
            VERSIONS=`seq ${MIN_VERSION} 1 ${MAX_VERSION}`
        else
            # some versions are skipped because seinfo segfaults when inspecting binary policies between v.20 and v.23"
            VERSIONS=`seq ${MIN_VERSION} 1 ${MAX_VERSION} | grep -v -e 19 -e 20 -e 21 -e 22 -e 23`
        fi
        for CUR_VERSION in ${VERSIONS} ; do
            rlRun "rm -f policy.out"
            rlWatchdog "checkpolicy -M -c ${CUR_VERSION} -o policy.out policy.conf.from.secilc" 15
            if [ -s policy.out ] ; then
                rlRun "seinfo policy.out 2>&1 | tee ${OUTPUT_FILE}"
                rlRun "grep -i -e \"policy version.*${CUR_VERSION}\" -e \"unable to open policy\" ${OUTPUT_FILE}"
            else
                rlRun "ls -l policy.out"
            fi
        done
    rlPhaseEnd

    rlPhaseStartTest
        rlRun "checkpolicy >& ${TEST_FILE}" 1
        rlAssertGrep "loading policy configuration from policy.conf" ${TEST_FILE}
        rlRun "checkpolicy -b >& ${TEST_FILE}" 1
        rlAssertGrep "loading policy configuration from policy" ${TEST_FILE}
        rlRun "checkpolicy -V"
        rlRun "checkpolicy -U 2>&1 | grep \"option requires an argument\""
        rlRun "checkpolicy -U xyz" 1
        rlRun "rm -f policy.conf"
        if ! rlIsRHEL 4 ; then
            for OPTION in "deny" "reject" "allow" ; do
                rlRun "checkpolicy -U ${OPTION} >& ${TEST_FILE}" 1
                rlAssertGrep "unable to open policy.conf" ${TEST_FILE}
            done
        fi
        rlRun "rm -f ${TEST_FILE}"
        rlRun "touch ${TEST_FILE}"
        rlRun "rm -rf ${TEST_DIR}"
        rlRun "mkdir ${TEST_DIR}"
        rlRun "checkpolicy ${TEST_FILE}" 1,2
        rlRun "checkpolicy -b ${TEST_FILE}" 1
        rlRun "checkpolicy ${TEST_DIR}" 1,2
        rlRun "checkpolicy -b ${TEST_DIR}" 1
        rlRun "rm -f ${TEST_FILE}"
        rlRun "rm -rf ${TEST_DIR}"
        rlRun "checkpolicy ${TEST_FILE}" 1
        rlRun "checkpolicy -b ${TEST_FILE}" 1
        rlRun "checkpolicy -c 2>&1 | grep \"option requires an argument\""
        rlRun "checkpolicy -c 0 2>&1 | grep \"value 0 not in range\""
        rlRun "checkpolicy -t 2>&1 | grep \"option requires an argument\""
        rlRun "checkpolicy -t xyz 2>&1 | grep -i \"unknown target platform\""
        rlRun "checkpolicy --help 2>&1 | grep -- '-m]'" 1
    rlPhaseEnd

    rlPhaseStartTest
        if rlIsRHEL 5 6 ; then
            ACTIVE_POLICY="/selinux/policy"
        else
            ACTIVE_POLICY="/sys/fs/selinux/policy"
        fi
        rlRun "echo -e 'q\n' | checkpolicy -Mdb ${ACTIVE_POLICY} | tee ${OUTPUT_FILE}"
        rlRun "grep -qi -e error -e ebitmap -e 'not match' ${OUTPUT_FILE}" 1
        for POLICY_TYPE in minimum mls targeted ; do
            if [ ! -e /etc/selinux/${POLICY_TYPE}/policy/policy.* ] ; then
                continue
            fi
            rlRun "echo -e 'q\n' | checkpolicy -Mdb /etc/selinux/${POLICY_TYPE}/policy/policy.* | tee ${OUTPUT_FILE}"
            rlRun "grep -qi -e error -e ebitmap -e 'not match' ${OUTPUT_FILE}" 1
        done
    rlPhaseEnd

    rlPhaseStartTest
        if rlIsRHEL 5 6 ; then
            ACTIVE_POLICY_TREE="/selinux"
        else # RHEL-7 and above
            ACTIVE_POLICY_TREE="/sys/fs/selinux"
        fi
        MIN_VERSION="15"
        MAX_VERSION=`find /etc/selinux/ -name policy.?? | cut -d / -f 6 | cut -d . -f 2 | head -n 1`
        for POLICY_TYPE in minimum mls targeted ; do
            if rlIsRHEL 5 6 ; then
                VERSIONS=`seq ${MIN_VERSION} 1 ${MAX_VERSION}`
            else
                # some versions are skipped because seinfo segfaults when inspecting binary policies between v.20 and v.23"
                VERSIONS=`seq ${MIN_VERSION} 1 ${MAX_VERSION} | grep -v -e 19 -e 20 -e 21 -e 22 -e 23`
            fi
            for CUR_VERSION in ${VERSIONS} ; do
                rlRun "rm -f policy.out"
                rlWatchdog "checkpolicy -b -M -c ${CUR_VERSION} -o policy.out /etc/selinux/${POLICY_TYPE}/policy/policy.${MAX_VERSION}" 15
                if [ -s policy.out ] ; then
                    rlRun "seinfo policy.out 2>&1 | tee ${OUTPUT_FILE}"
                    rlRun "grep -i -e \"policy version.*${CUR_VERSION}\" -e \"unable to open policy\" ${OUTPUT_FILE}"
                else
                    rlRun "ls -l policy.out"
                fi
            done
        done
    rlPhaseEnd

    rlPhaseStartCleanup
        rm -f ${OUTPUT_FILE} policy.out
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd

