#!/bin/bash
# vim: dict=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/policycoreutils/Regression/semanage-interface
#   Description: Does semanage interface ... work correctly?
#   Author: Milos Malik <mmalik@redhat.com>
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   Copyright (c) 2013 Red Hat, Inc. All rights reserved.
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

rlJournalStart
    rlPhaseStartSetup
        rlAssertRpm ${PACKAGE}
    rlPhaseEnd

    rlPhaseStartTest
        rlRun "semanage interface --help" 0,1
        for POLICY_TYPE in minimum mls targeted ; do
            if [ ! -d /etc/selinux/${POLICY_TYPE} ] ; then
                continue
            fi
            rlRun "semanage interface -l -S ${POLICY_TYPE}"
        done
        if ! rlIsRHEL 5; then
          rlRun "semanage interface -l -S unknown 2>&1 | grep \"store cannot be accessed\""
        fi
        rlRun "semanage interface -a -t xyz_t xyz 2>&1 | grep -i -e 'not defined' -e 'error' -e 'could not'"
        rlRun "semanage interface -m xyz" 1,2
        rlRun "semanage interface -d xyz" 1
        rlRun "semanage interface -a -t netif_t xyz"
        if rlIsRHEL 5 6; then
            rlRun "semanage interface -m -r s0 xyz"
        else
            rlRun "semanage interface -m -t netif_t -r s0 xyz"
        fi
        rlRun "semanage interface -l | grep \"xyz.*:netif_t:s0\""
        rlRun "semanage interface -d xyz"
        rlRun "semanage interface -l | grep xyz" 1
    rlPhaseEnd

    rlPhaseStartCleanup
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd

