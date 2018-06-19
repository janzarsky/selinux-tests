#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/policycoreutils/Sanity/modules
#   Description: Test module compiling and loading
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
        rlAssertRpm "selinux-policy-devel"
    rlPhaseEnd

    rlPhaseStartTest "TE module using Makefile"
        rlRun "mkdir test"
        rlRun "pushd test"

        # create new type, allow reading, do not allow writing
        rlRun "cat << EOF >test_module.te
module test_module 1.0;

require {
    type unconfined_t;
    type fs_t;
    class file { getattr ioctl lock open read relabelto write };
    class filesystem { associate };
}

type test_t;

allow test_t fs_t:filesystem { associate };
allow unconfined_t test_t:file { getattr ioctl lock open read relabelto};
dontaudit unconfined_t test_t:file { write };
EOF"
        # compile the module
        rlRun "make -f /usr/share/selinux/devel/Makefile test_module.pp"

        # load it
        rlRun "semodule -i test_module.pp"

        # test that the rules are enforced
        rlRun "touch test_file"
        rlRun "chcon -t test_t test_file"
        rlRun "cat test_file" 0
        rlRun "echo asdf >test_file" 1

        # unload the module
        rlRun "semodule -r test_module"

        rlRun "popd"
        rlRun "rm -rf test"
    rlPhaseEnd

    rlPhaseStartTest "CIL module"
        rlRun "mkdir test"
        rlRun "pushd test"

        # create new type, allow reading, do not allow writing
        rlRun "cat << EOF >test_module.cil
(type test_t)
(allow test_t fs_t (filesystem (associate)))
(allow unconfined_t test_t (file (getattr ioctl lock open read relabelto)))
(dontaudit unconfined_t test_t (file (write)))
EOF"

        # load the module
        rlRun "semodule -i test_module.cil"

        # test that the rules are enforced
        rlRun "touch test_file"
        rlRun "chcon -t test_t test_file"
        rlRun "cat test_file" 0
        rlRun "echo asdf >test_file" 1

        # unload the module
        rlRun "semodule -r test_module"

        rlRun "popd"
        rlRun "rm -rf test"
    rlPhaseEnd

    rlPhaseStartTest "Priorities"
        rlRun "mkdir test"
        rlRun "pushd test"

        # create new type, allow reading and writing
        rlRun "cat << EOF >test_module.cil
(type test_t)
(allow test_t fs_t (filesystem (associate)))
(allow unconfined_t test_t (file (getattr ioctl lock open read relabelto
    write)))
EOF"

        # load the module at priority 400
        rlRun "semodule -i test_module.cil -X 400"

        # test that both reading and writing is allowed
        rlRun "touch test_file"
        rlRun "chcon -t test_t test_file"
        rlRun "cat test_file" 0
        rlRun "echo asdf >test_file" 0

        # create new type, allow only reading
        rlRun "cat << EOF >test_module.cil
(type test_t)
(allow test_t fs_t (filesystem (associate)))
(allow unconfined_t test_t (file (getattr ioctl lock open read relabelto)))
(dontaudit unconfined_t test_t (file (write)))
EOF"

        # load the module at priority 500
        rlRun "semodule -i test_module.cil -X 500"

        # test that only reading is allowed
        rlRun "cat test_file" 0
        rlRun "echo asdf >test_file" 1

        # unload both modules
        rlRun "semodule -r test_module -X 400"
        rlRun "semodule -r test_module -X 500"

        rlRun "popd"
        rlRun "rm -rf test"
    rlPhaseEnd

    rlPhaseStartTest "Disabling modules"
        rlRun "mkdir test"
        rlRun "pushd test"

        # create new type, allow reading
        rlRun "cat << EOF >test_module.cil
(type test_t)
(allow test_t fs_t (filesystem (associate)))
(allow unconfined_t test_t (file (getattr ioctl lock open read relabelto)))
(dontaudit unconfined_t test_t (file (write)))
EOF"
        # allow writing
        rlRun "cat << EOF >test_module_writing.cil
(allow unconfined_t test_t (file (write)))
EOF"

        # load the modules
        rlRun "semodule -i test_module.cil test_module_writing.cil"

        # test that reading and writing is allowed
        rlRun "touch test_file"
        rlRun "chcon -t test_t test_file"
        rlRun "cat test_file"
        rlRun "echo asdf >test_file"

        # disable writing
        rlRun "semodule -d test_module_writing"

        # test that writing is not allowed
        rlRun "cat test_file" 0
        rlRun "echo asdf >test_file" 1

        # enable the module
        rlRun "semodule -e test_module_writing"

        # test that writing is allowed
        rlRun "cat test_file" 0
        rlRun "echo asdf >test_file" 0

        # unload the modules
        rlRun "semodule -r test_module test_module_writing"

        rlRun "popd"
        rlRun "rm -rf test"
    rlPhaseEnd

    rlPhaseStartCleanup
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd
