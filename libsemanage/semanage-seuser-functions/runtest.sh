#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/libsemanage/Sanity/semanage-seuser-functions
#   Description: Test semanage_seuser_* functions
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
        rlAssertRpm ${PACKAGE}
        rlAssertRpm ${PACKAGE}-devel
        rlAssertRpm "glibc"
        rlAssertRpm "gcc"

        for f in test_*.c ; do 
            out=$(echo -n $f | cut -d'.' -f1)
            rlRun "gcc $f -o $out -lsemanage -Wall -Wextra -Werror -std=c99"
        done

        POLICY_TYPE="$(grep -E '^SELINUXTYPE=' /etc/selinux/config | cut -d'=' -f2 | tr '[:upper:]' '[:lower:]' | tr -d ' ')"

        if rlIsFedora; then
            SEUSERS_PATH="/var/lib/selinux/$POLICY_TYPE/active/seusers"
        elif rlIsRHEL '>=7'; then
            SEUSERS_PATH="/etc/selinux/$POLICY_TYPE/active/seusers"
        else
            SEUSERS_PATH="/etc/selinux/$POLICY_TYPE/seusers"
        fi

        rlRun "cat $SEUSERS_PATH"

        SEUSERS_COUNT="$(cat $SEUSERS_PATH | grep -vE '^#|^$' | wc -l)"
        rlRun "[[ \"$SEUSERS_COUNT\" -gt 0 ]]"

        SEUSERS="$(cat $SEUSERS_PATH | grep -vE '^#|^$' | cut -d':' -f1 | tr '\n' ' ')"
        rlRun "[[ -n \"$SEUSERS\" ]]"

        first_line="$(cat $SEUSERS_PATH | grep -vE '^#|^$' | head -n 1)"
        SEUSER="$(echo -n $first_line | cut -d':' -f1)"
        rlRun "[[ -n \"$SEUSER\" ]]"
        SEUSER_SENAME="$(echo -n $first_line | cut -d':' -f2)"
        rlRun "[[ -n \"$SEUSER_SENAME\" ]]"
        SEUSER_MLSRANGE="$(echo -n $first_line | cut -d':' -f3-4)"
        rlRun "[[ -n \"$SEUSER_MLSRANGE\" ]]"

        SEUSER_NONEXISTENT="nonuser"
        SEUSER_DEFAULT="__default__"

        ERR_FAIL=1
        ERR_ABORT=134
        ERR_SEGFAULT=139

        # note: each test_*.c program takes first argument which specifies setup
        #       before executing specified function
        #       init      semanage handle == NULL
        #       handle    semanage handle obtained via semanage_handle_create
        #       conn      connected via semanage_connect
        #       trans     inside transaction, via semanage_begin_transaction
        # program returns 1 on error in function, 2 on error in setup
    rlPhaseEnd

    rlPhaseStartTest "semanage_seuser_key_create, semanage_seuser_key_free"
        # FIXME
        # rlRun "./test_key_create init   $SEUSER" $ERR_ABORT,$ERR_SEGFAULT
        # rlRun "./test_key_create handle $SEUSER" $ERR_FAIL
        rlRun "./test_key_create conn   $SEUSER"
        rlRun "./test_key_create trans  $SEUSER"
    rlPhaseEnd

    rlPhaseStartTest "semanage_seuser_key_extract"
        # FIXME
        #rlRun "./test_key_extract conn  new"
        rlRun "./test_key_extract conn  first"
        # FIXME
        #rlRun "./test_key_extract trans new"
        rlRun "./test_key_extract trans first"
    rlPhaseEnd

    rlPhaseStartTest "semanage_seuser_compare"
        rlRun "./test_compare conn  $SEUSER             same"
        rlRun "./test_compare conn  $SEUSER_NONEXISTENT different"
        rlRun "./test_compare trans $SEUSER             same"
        rlRun "./test_compare trans $SEUSER_NONEXISTENT different"
    rlPhaseEnd

    rlPhaseStartTest "semanage_seuser_compare2"
        rlRun "./test_compare2 conn  NULL 0"    $ERR_ABORT,$ERR_SEGFAULT
        rlRun "./test_compare2 conn  0    NULL" $ERR_ABORT,$ERR_SEGFAULT
        rlRun "./test_compare2 conn  NULL NULL" $ERR_ABORT,$ERR_SEGFAULT
        rlRun "./test_compare2 conn  0    0"
        rlRun "./test_compare2 conn  0    1"
        rlRun "./test_compare2 trans NULL 0"    $ERR_ABORT,$ERR_SEGFAULT
        rlRun "./test_compare2 trans 0    NULL" $ERR_ABORT,$ERR_SEGFAULT
        rlRun "./test_compare2 trans NULL NULL" $ERR_ABORT,$ERR_SEGFAULT
        rlRun "./test_compare2 trans 0    0"
        rlRun "./test_compare2 trans 0    1"
    rlPhaseEnd

    rlPhaseStartTest "semanage_seuser_count"
        rlRun "./test_count init"   $ERR_ABORT,$ERR_SEGFAULT
        rlRun "./test_count handle" $ERR_FAIL
        rlRun "./test_count conn  $SEUSERS_COUNT"
        rlRun "./test_count trans $SEUSERS_COUNT"
    rlPhaseEnd

    rlPhaseStartTest "semanage_seuser_list"
        rlRun "./test_list init"   $ERR_ABORT,$ERR_SEGFAULT
        rlRun "./test_list handle" $ERR_FAIL
        rlRun "./test_list conn  $SEUSERS_COUNT $SEUSERS"
        rlRun "./test_list trans $SEUSERS_COUNT $SEUSERS"
    rlPhaseEnd

    rlPhaseStartTest "semanage_seuser_iterate"
        rlRun "./test_iterate init"   $ERR_ABORT,$ERR_SEGFAULT
        rlRun "./test_iterate handle" $ERR_FAIL
        rlRun "./test_iterate conn  $SEUSERS"
        rlRun "./test_iterate trans $SEUSERS"
    rlPhaseEnd

    rlPhaseStartTest "semanage_seuser_exists"
        rlRun "./test_exists conn  $SEUSER_NONEXISTENT 0"
        rlRun "./test_exists conn  $SEUSER_DEFAULT     1"
        rlRun "./test_exists conn  $USER               1"
        rlRun "./test_exists trans $SEUSER_NONEXISTENT 0"
        rlRun "./test_exists trans $SEUSER_DEFAULT     1"
        rlRun "./test_exists trans $SEUSER             1"
    rlPhaseEnd

    rlPhaseStartTest "semanage_seuser_query"
        rlRun "./test_query conn  $SEUSER_NONEXISTENT" $ERR_FAIL
        rlRun "./test_query conn  $SEUSER_DEFAULT"
        rlRun "./test_query conn  $SEUSER"
        rlRun "./test_query trans $SEUSER_NONEXISTENT" $ERR_FAIL
        rlRun "./test_query trans $SEUSER_DEFAULT"
        rlRun "./test_query trans $SEUSER"
    rlPhaseEnd

    rlPhaseStartTest "semanage_seuser_get_name"
        rlRun "./test_get_name conn  new   NULL"
        rlRun "./test_get_name conn  first $SEUSER"
        rlRun "./test_get_name trans new   NULL"
        rlRun "./test_get_name trans first $SEUSER"
    rlPhaseEnd

    rlPhaseStartTest "semanage_seuser_set_name"
        name="someuser"
        rlRun "./test_set_name conn  $name"
        rlRun "./test_set_name trans $name"
    rlPhaseEnd

    rlPhaseStartTest "semanage_seuser_get_sename"
        rlRun "./test_get_sename conn  new   NULL"
        rlRun "./test_get_sename conn  first $SEUSER_SENAME"
        rlRun "./test_get_sename trans new   NULL"
        rlRun "./test_get_sename trans first $SEUSER_SENAME"
    rlPhaseEnd

    rlPhaseStartTest "semanage_seuser_set_sename"
        sename="someuser_u"
        rlRun "./test_set_sename conn  $sename"
        rlRun "./test_set_sename trans $sename"
    rlPhaseEnd

    rlPhaseStartTest "semanage_seuser_get_mlsrange"
        rlRun "./test_get_mlsrange conn  new   NULL"
        rlRun "./test_get_mlsrange conn  first $SEUSER_MLSRANGE"
        rlRun "./test_get_mlsrange trans new   NULL"
        rlRun "./test_get_mlsrange trans first $SEUSER_MLSRANGE"
    rlPhaseEnd

    rlPhaseStartTest "semanage_seuser_set_mlsrange"
        mlsrange="c0-s1:c0.c42"
        rlRun "./test_set_mlsrange conn  $mlsrange"
        rlRun "./test_set_mlsrange trans $mlsrange"
    rlPhaseEnd

    rlPhaseStartTest "semanage_seuser_clone"
        # FIXME
        #rlRun "./test_clone conn  new"
        rlRun "./test_clone conn  first"
        # FIXME
        #rlRun "./test_clone trans new"
        rlRun "./test_clone trans first"
    rlPhaseEnd

    rlPhaseStartTest "semanage_seuser_create"
        # FIXME
        #rlRun "./test_create init" $ERR_ABORT,$ERR_SEGFAULT
        #rlRun "./test_create handle" $ERR_ABORT,$ERR_SEGFAULT
        rlRun "./test_create conn"
        rlRun "./test_create trans"
    rlPhaseEnd

    rlPhaseStartTest "semanage_seuser_modify_local"
        # function requires transaction
        #rlRun "./test_modify_local conn  new"   $ERR_FAIL
        #rlRun "./test_modify_local conn  first" $ERR_FAIL
        #rlRun "./test_modify_local trans new"   $ERR_FAIL
        rlRun "./test_modify_local trans first"
    rlPhaseEnd
    
    rlPhaseStartTest "semanage_seuser_del_local"
        # adding local seuser requires transaction
        # FIXME
        #rlRun "./test_del_local trans first new"
        #rlRun "./test_del_local trans first second"
        rlRun "./test_del_local trans first first"
    rlPhaseEnd

    rlPhaseStartTest "semanage_seuser_exists_local"
        # adding local seuser requires transaction
        rlRun "./test_exists_local trans first first  1"
        rlRun "./test_exists_local trans first second 0"
    rlPhaseEnd

    rlPhaseStartTest "semanage_seuser_count_local"
        # adding local seuser requires transaction
        # FIXME
        #rlRun "./test_count_local trans 0"
        rlRun "./test_count_local trans 1"
        rlRun "./test_count_local trans 2"
    rlPhaseEnd

    rlPhaseStartCleanup
        testfiles="$(ls -1 test_* | grep -v '\.c' | tr '\n' ' ')"
        rlRun "rm -f $testfiles"
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd
