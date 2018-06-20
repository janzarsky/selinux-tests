#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/policycoreutils/Sanity/file-contexts
#   Description: Test semanage fcontext, restorecon, fixfiles, chcon
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

        RUN_CON="system_u:object_r:var_run_t:s0"
        HOME_BIN_CON="unconfined_u:object_r:home_bin_t:s0"
        FILE_CON="user_tmp_t"
        TEST_CON="user_home_t"
        TEST_CON2="tmp_t"

        function checkFC {
            ls -dZ $1
            ls -dZ $1 | grep $2
            return $?
        }
    rlPhaseEnd

    rlPhaseStartTest "semanage fcontext -l"
        rlRun "semanage fcontext -l >stdout"
        rlAssertGrep "SELinux fcontext" stdout
        rlAssertGrep "SELinux Distribution fcontext Equivalence" stdout
        rlAssertGrep "/run/\.\* *all files *$RUN_CON" stdout
        rlAssertGrep "/run *directory *$RUN_CON" stdout
        rlAssertGrep "/home/\[\^/\]+/bin(/\.\*)? *all files *$HOME_BIN_CON" stdout
        rlAssertGrep "/run = /var/run" stdout
    rlPhaseEnd

    rlPhaseStartTest "semanage fcontext add and delete"
        rlRun "mkdir /tmp/test"
        rlRun "pushd /tmp/test"
        DIR=$(pwd)

        # add custom file context
        rlRun "semanage fcontext -a -t $TEST_CON '$DIR/asdf'"

        rlRun "semanage fcontext -l -C | grep '$DIR/asdf *all files *system_u:object_r:$TEST_CON'"

        # test for regular file
        rlRun "touch asdf"
        rlRun "checkFC asdf $FILE_CON"
        rlRun "restorecon asdf"
        rlRun "checkFC asdf $TEST_CON"
        rlRun "rm asdf"

        # test for directory
        rlRun "mkdir asdf"
        rlRun "checkFC asdf $FILE_CON"
        rlRun "restorecon asdf"
        rlRun "checkFC asdf $TEST_CON"
        rlRun "rmdir asdf"

        # modify the context
        rlRun "semanage fcontext -m -t $TEST_CON2 '$DIR/asdf'"

        rlRun "semanage fcontext -l -C | grep '$DIR/asdf *all files *system_u:object_r:$TEST_CON2'"

        # test
        rlRun "touch asdf"
        rlRun "checkFC asdf $FILE_CON"
        rlRun "restorecon asdf"
        rlRun "checkFC asdf $TEST_CON2"
        rlRun "rm asdf"

        # delete the context
        rlRun "semanage fcontext -d '$DIR/asdf'"

        rlRun "popd"
        rlRun "rm -rf /tmp/test"
    rlPhaseEnd

    rlPhaseStartTest "semanage fcontext add and delete with file type"
        rlRun "mkdir /tmp/test"
        rlRun "pushd /tmp/test"
        DIR=$(pwd)

        # add custom file context for regular files
        rlRun "semanage fcontext -a -f f -t $TEST_CON '$DIR/asdf'"
        # add custom file context for directories
        rlRun "semanage fcontext -a -f d -t $TEST_CON2 '$DIR/asdf'"

        rlRun "semanage fcontext -l | grep '$DIR/asdf *regular file *system_u:object_r:$TEST_CON'"
        rlRun "semanage fcontext -l | grep '$DIR/asdf *directory *system_u:object_r:$TEST_CON2'"

        # test regular file
        rlRun "touch asdf"
        rlRun "checkFC asdf $FILE_CON"
        rlRun "restorecon asdf"
        rlRun "checkFC asdf $TEST_CON"
        rlRun "rm asdf"

        # test directory
        rlRun "mkdir asdf"
        rlRun "checkFC asdf $FILE_CON"
        rlRun "restorecon asdf"
        rlRun "checkFC asdf $TEST_CON2"
        rlRun "rmdir asdf"

        # delete the context
        rlRun "semanage fcontext -d -f f '$DIR/asdf'"
        rlRun "semanage fcontext -d -f d '$DIR/asdf'"

        rlRun "popd"
        rlRun "rm -rf /tmp/test"
    rlPhaseEnd

    rlPhaseStartTest "chcon"
        rlRun "mkdir /tmp/test"
        rlRun "pushd /tmp/test"

        rlRun "touch asdf"
        rlRun "checkFC asdf $FILE_CON"

        rlRun "chcon -t $TEST_CON asdf"
        rlRun "checkFC asdf $TEST_CON"

        rlRun "popd"
        rlRun "rm -rf /tmp/test"
    rlPhaseEnd

    rlPhaseStartTest "restorecon"
        rlRun "mkdir /tmp/test"
        rlRun "pushd /tmp/test"
        DIR=$(pwd)

        # add a custom file context for whole directory
        rlRun "semanage fcontext -a -t $TEST_CON '$DIR/.*'"

        # create test files and directories
        rlRun "touch a"
        rlRun "checkFC a $FILE_CON"
        rlRun "mkdir dir"
        rlRun "checkFC dir $FILE_CON"
        rlRun "touch dir/a"
        rlRun "checkFC dir/a $FILE_CON"
        rlRun "touch dir/b"
        rlRun "checkFC dir/b $FILE_CON"
        rlRun "mkdir dir/dir"
        rlRun "checkFC dir/dir $FILE_CON"
        rlRun "touch dir/dir/a"
        rlRun "checkFC dir/dir/a $FILE_CON"

        function prepareStr {
            echo -n ".* $DIR/$1 .* [[:alnum:]:_]*$2:[[:alnum:]:_]*.*[[:alnum:]:_]*$3:[[:alnum:]:_]*"
        }

        # run restorecon in dry-run mode for a single file
        rlRun "restorecon -v -n dir/a >stdout"
        rlRun "cat stdout"
        rlAssertGrep "$(prepareStr dir/a $FILE_CON $TEST_CON)" stdout
        rlRun "[ $(cat stdout | wc -l) -eq 1 ]"

        # run restorecon in recursive mode
        rlRun "restorecon -r -v dir >stdout"
        rlRun "cat stdout"
        rlAssertGrep "$(prepareStr dir $FILE_CON $TEST_CON)" stdout
        rlAssertGrep "$(prepareStr dir/a $FILE_CON $TEST_CON)" stdout
        rlAssertGrep "$(prepareStr dir/b $FILE_CON $TEST_CON)" stdout
        rlAssertGrep "$(prepareStr dir/dir $FILE_CON $TEST_CON)" stdout
        rlAssertGrep "$(prepareStr dir/dir/a $FILE_CON $TEST_CON)" stdout
        rlRun "[ $(cat stdout | wc -l) -eq 5 ]"

        rlRun "checkFC dir $TEST_CON"
        rlRun "checkFC dir/a $TEST_CON"
        rlRun "checkFC dir/b $TEST_CON"
        rlRun "checkFC dir/dir $TEST_CON"
        rlRun "checkFC dir/dir/a $TEST_CON"

        # delete the custom context
        rlRun "semanage fcontext -d '$DIR/.*'"

        rlRun "popd"
        rlRun "rm -rf /tmp/test"
    rlPhaseEnd

    rlPhaseStartCleanup
        rlRun "rm -rf stdout"
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd
