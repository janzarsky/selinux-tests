#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/checkpolicy/Sanity/sedismod
#   Description: Does sedismod work correctly 
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
# TODO: repeat for all policy modules that are installed under /etc/selinux
if rlIsFedora ; then
    POLICY_FILE="`find /var/lib/selinux/targeted -type d -name base`/hll"
elif rlIsRHEL '<7.3' ; then
    POLICY_FILE=`find /etc/selinux/targeted -type f -name base.pp`
else # RHEL-7.3 and above
    POLICY_FILE="`find /etc/selinux/targeted -type d -name base`/hll"
fi

rlJournalStart
    rlPhaseStartSetup
        rlAssertRpm ${PACKAGE}
        OUTPUT_FILE=`mktemp`
        if rlIsRHEL '>=7.3' || rlIsFedora ; then
            rlRun "semodule -H -E base"
        else
            rlRun "cp ${POLICY_FILE} ./base.pp.bz2"
            rlRun "rm -f base.pp"
            rlRun "bzip2 -d ./base.pp.bz2"
        fi
        POLICY_FILE="base.pp"
        rlRun "ls -l ${POLICY_FILE}"
    rlPhaseEnd

    rlPhaseStartTest "check all available options"
        if rlIsRHEL 6 ; then
            AVAILABLE_OPTIONS="1 2 3 4 5 6 7 8 0 a b c u"
        else # RHEL-7 and above
            AVAILABLE_OPTIONS="1 2 3 4 5 6 7 8 9 0 a b c u F"
        fi
        for OPTION in ${AVAILABLE_OPTIONS} ; do
            rlRun "rm -f ${OUTPUT_FILE}"
            rlWatchdog "./sedismod.exp ${OPTION} ${POLICY_FILE} ${OUTPUT_FILE}" 65
            # rlWatchdog kills the expect script, but we need to kill the sedismod process too
            rlRun "killall sedismod" 0,1
            rlRun "ls -l ${OUTPUT_FILE}"
            if [ -s ${OUTPUT_FILE} ] ; then
                rlPass "sedismod produced some output"
            else
                rlFail "sedismod did not produce any output"
            fi
        done
    rlPhaseEnd

    rlPhaseStartCleanup
        rlRun "rm -f ${OUTPUT_FILE} ${POLICY_FILE}"
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd

