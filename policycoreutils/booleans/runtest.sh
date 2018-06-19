#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/policycoreutils/Sanity/booleans
#   Description: Basic operations with booleans
#   Author: Jan Zarsky <jzarsky@redhat.com>
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   Copyright (c) 2018 Red Hat, Inc.
#
#   This program is free software: you can redistribute it and/or
#   modify it under the terms of the GNU General Public License as
#   published by the Free Software Foundation, either version 2 of
#   the License, or (at your option) any later version.
#
#   This program is distributed in the hope that it will be
#   useful, but WITHOUT ANY WARRANTY; without even the implied
#   warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#   PURPOSE.  See the GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program. If not, see http://www.gnu.org/licenses/.
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# Include Beaker environment
. /usr/bin/rhts-environment.sh || exit 1
. /usr/share/beakerlib/beakerlib.sh || exit 1

PACKAGE="policycoreutils"

rlJournalStart
    rlPhaseStartSetup
        rlAssertRpm $PACKAGE

        cat << EOF >test_module.cil
(boolean test_bool_on true)
(boolean test_bool_off false)
EOF
        rlRun "semodule -i test_module.cil"

        BOOL_ON=test_bool_on
        BOOL_OFF=test_bool_off
    rlPhaseEnd

    rlPhaseStartTest "listing"
        rlRun "semanage boolean -l >stdout"

        rlRun "grep -E '$BOOL_ON *\(on   ,   on\) ' stdout"
        rlRun "grep -E '$BOOL_OFF *\(off  ,  off\) ' stdout"

        rlRun "getsebool -a >stdout"

        rlRun "grep 'test_bool_on --> on' stdout"
        rlRun "grep 'test_bool_off --> off' stdout"

        rlRun "getsebool $BOOL_ON | grep '$BOOL_ON --> on'"
        rlRun "getsebool $BOOL_OFF | grep '$BOOL_OFF --> off'"
    rlPhaseEnd

    rlPhaseStartTest "setting"
        # temporary via setsebool
        rlRun "getsebool $BOOL_ON | grep '$BOOL_ON --> on'"

        rlRun "setsebool $BOOL_ON off"
        rlRun "getsebool $BOOL_ON | grep '$BOOL_ON --> off'"

        rlRun "setsebool $BOOL_ON on"
        rlRun "getsebool $BOOL_ON | grep '$BOOL_ON --> on'"

        # permanent via setsebool
        rlRun "getsebool $BOOL_ON | grep '$BOOL_ON --> on'"

        rlRun "setsebool -P $BOOL_ON off"
        rlRun "getsebool $BOOL_ON | grep '$BOOL_ON --> off'"

        rlRun "setsebool -P $BOOL_ON on"
        rlRun "getsebool $BOOL_ON | grep '$BOOL_ON --> on'"

        # permanent via semanage boolean
        rlRun "semanage boolean -m $BOOL_ON --off"
        rlRun "getsebool $BOOL_ON | grep '$BOOL_ON --> off'"
        rlRun "semanage boolean -l -C | grep -E '$BOOL_ON *\(off  ,  off\) '"

        rlRun "semanage boolean -m $BOOL_ON --on"
        rlRun "getsebool $BOOL_ON | grep '$BOOL_ON --> on'"
        rlRun "semanage boolean -l -C | grep -E '$BOOL_ON *\(on   ,   on\) '"
    rlPhaseEnd

    rlPhaseStartCleanup
        rlRun "semodule -r test_module"
        rlRun "rm stdout test_module.cil"
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd
