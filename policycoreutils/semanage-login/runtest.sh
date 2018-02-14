#!/bin/bash
# vim: dict=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/policycoreutils/Regression/semanage-login
#   Description: Does semanage login ... work correctly?
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
        rlRun "semanage login --help" 0,1
        for POLICY_TYPE in minimum mls targeted ; do
            if [ ! -d /etc/selinux/${POLICY_TYPE} ] ; then
                continue
            fi
            rlRun "semanage login -l -S ${POLICY_TYPE}"
        done
        if ! rlIsRHEL 5; then
          rlRun "semanage login -l -S unknown 2>&1 | grep \"store cannot be accessed\""
        fi
        rlRun "semanage login -a -s xyz_u xyz 2>&1 | grep -i -e 'does not exist' -e 'mapping.*invalid' -e 'could not query'"
        rlRun "semanage login -m xyz" 1
        rlRun "semanage login -d xyz" 1
        rlRun "useradd xyz"
        rlRun "semanage login -a -s user_u xyz"
        rlRun "semanage login -m -r s0 xyz"
        rlRun "semanage login -l | grep \"xyz.*user_u.*s0\""
        rlRun "semanage login -d xyz"
        rlRun "semanage login -l | grep xyz" 1
        rlRun "userdel -rf xyz"
    rlPhaseEnd

    rlPhaseStartCleanup
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd

