#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/libsemanage/Sanity/sanity-tests
#   Description: Test libsemanage functions
#   Author: Jan Zarsky <jzarsky@redhat.com>
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   Copyright (c) 2017 Red Hat, Inc.
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

PACKAGE="libsemanage"

rlJournalStart
    rlPhaseStartSetup
        rlRun "rlImport --all" || rlDie
        rlSESatisfyRequires

        rlRun "rlCheckMakefileRequires"

        VERS=$(rpm -q $PACKAGE | cut -f 2 -d '-')
        VERS_MAJOR=$(echo $VERS | cut -f 1 -d '.')
        VERS_MINOR=$(echo $VERS | cut -f 2 -d '.')
        VERS_RELEASE=$(rpm -q $PACKAGE | cut -f 3 -d '-' | cut -f 1 -d '.')

        rlRun "cd tests"

        rlRun "make VERS_MAJOR=$VERS_MAJOR VERS_MINOR=$VERS_MINOR VERS_RELEASE=$VERS_RELEASE"

        rlRun "./libsemanage-tests list > list"
    rlPhaseEnd

    for suite in $(cat list); do
        rlPhaseStartTest "$suite"
            rlRun "./libsemanage-tests suite $suite >rawoutput 2>erroutput" 0 "Run test suite $suite"

            rlRun "cat rawoutput | sed 1,5d" 0 "stdout"
            rlRun "cat erroutput" 0 "stderr"

            rlRun "grep -v 'Run Summary:' rawoutput > output"

            rlRun "! grep -i failed output" 0 "Fails"
            rlRun "! grep -i error output" 0 "Errors"
        rlPhaseEnd
    done;

    rlPhaseStartCleanup
        rlRun "make clean"
        rlRun "rm -f output rawoutput erroutput list"
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd
