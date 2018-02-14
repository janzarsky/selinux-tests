#!/bin/bash
# vim: dict=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/policycoreutils/Regression/semanage-user
#   Description: Does semanage user ... work correctly?
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
        rlRun "make -f /usr/share/selinux/devel/Makefile"
        rlRun "ls -l testpolicy.pp"
    rlPhaseEnd

    rlPhaseStartTest
        if rlIsRHEL 5 6; then
            rlRun "semanage user --help" 1
        else
            rlRun "semanage user --help" 0
            # semanage: list option can not be used with --level ("semanage user -l")
            rlRun "semanage user --help | grep fcontext" 1
        fi
        for POLICY_TYPE in minimum mls targeted ; do
            if [ ! -d /etc/selinux/${POLICY_TYPE} ] ; then
                continue
            fi
            rlRun "semanage user -l -S ${POLICY_TYPE}"
        done
        if ! rlIsRHEL 5; then
          rlRun "semanage user -l -S unknown 2>&1 | grep \"store cannot be accessed\""
        fi
        rlRun "semanage user -a -P user -R xyz_r xyz_u 2>&1 | grep -i -e 'undefined' -e 'error' -e 'could not'"
        rlRun "semanage user -m xyz_u" 1
        rlRun "semanage user -d xyz_u" 1
        rlRun "semodule -i testpolicy.pp"
        rlRun "semanage user -a -P user -R xyz_r xyz_u"
        rlRun "semanage user -m -r s0 xyz_u"
        rlRun "semanage user -l | grep \"xyz_u.*s0.*s0.*xyz_r\""
        rlRun "semanage user -d xyz_u"
        rlRun "semanage user -l | grep xyz_u" 1
        rlRun "semodule -r testpolicy"
    rlPhaseEnd

    rlPhaseStartCleanup
        rlRun "rm -rf tmp testpolicy.{fc,if,pp}"
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd

