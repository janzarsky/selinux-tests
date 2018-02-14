#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/policycoreutils/Sanity/CIL-modules-without-compilation
#   Description: What the test does
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

PACKAGE="policycoreutils"

rlJournalStart
    rlPhaseStartSetup
        rlAssertRpm $PACKAGE
        rlRun "echo '()' > empty.cil"
        rlRun "echo '(())' > invalid.cil"
    rlPhaseEnd

    rlPhaseStartTest "empty CIL module"
        rlRun "semodule -lfull | grep '400.*empty.*cil'" 1
        rlRun "semodule -i empty.cil"
        rlRun "semodule -lfull | grep '400.*empty.*cil'"
        rlRun "semodule -r empty"
        rlRun "semodule -lfull | grep '400.*empty.*cil'" 1
        rlRun "semanage module -l | grep 'empty.*400.*cil'" 1
        rlRun "semanage module -a empty.cil"
        rlRun "semanage module -l | grep 'empty.*400.*cil'"
        rlRun "semanage module -r empty"
        rlRun "semanage module -l | grep 'empty.*400.*cil'" 1
    rlPhaseEnd

    rlPhaseStartTest "invalid CIL module"
        rlRun "semodule -lfull | grep '400.*invalid.*cil'" 1
        rlRun "semodule -i invalid.cil" 1
        rlRun "semodule -lfull | grep '400.*invalid.*cil'" 1
        rlRun "semodule -r invalid" 1
        rlRun "semodule -lfull | grep '400.*invalid.*cil'" 1
        rlRun "semanage module -l | grep 'invalid.*400.*cil'" 1
        rlRun "semanage module -a invalid.cil" 1
        rlRun "semanage module -l | grep 'invalid.*400.*cil'" 1
        rlRun "semanage module -r invalid" 1
        rlRun "semanage module -l | grep 'invalid.*400.*cil'" 1
    rlPhaseEnd

    rlPhaseStartCleanup
        rlRun "rm -f empty.cil invalid.cil"
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd

