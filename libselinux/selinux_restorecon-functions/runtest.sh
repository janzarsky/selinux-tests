#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/libselinux/Sanity/selinux_restorecon-functions
#   Description: Test functions in selinux_restorecon.c
#   Author: Jan Zarsky <jzarsky@redhat.com>
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   Copyright (c) 2016 Red Hat, Inc.
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

PACKAGE="libselinux"

function createTestFiles {
    rlLogInfo "Creating testing files"

    TmpDir="/home/user/testdir"
    rlRun "mkdir $TmpDir"

    rlRun "pushd $TmpDir"

    rlRun "mkdir -p a/b"

    rlRun "touch afile"
    rlRun "touch a/bfile"
    rlRun "touch a/b/cfile"
    
    rlRun "popd"
}

function changeContext {
    rlLogInfo "Changing context of testing files"

    rlRun "pushd $TmpDir"

    rlRun "chcon -t var_log_t ." 
    rlRun "chcon -t var_log_t a"
    rlRun "chcon -t var_log_t a/b"
    rlRun "chcon -t var_log_t afile"
    rlRun "chcon -t var_log_t a/bfile"
    rlRun "chcon -t var_log_t a/b/cfile"
    
    rlRun "popd"
}

function deleteTestFiles {
    rlLogInfo "Deleting testing files"

    rlRun "rm -rf $TmpDir"
}

rlJournalStart
    rlPhaseStartSetup
        rlAssertRpm ${PACKAGE}
        rlAssertRpm ${PACKAGE}-devel
        rlAssertRpm "glibc"
        rlAssertRpm "strace"

        rlRun -l "gcc test_restorecon.c -o test_restorecon -lselinux -pedantic -Wall -Wextra -std=c99"
        rlRun -l "gcc test_exclude_list.c -o test_exclude_list -lselinux -pedantic -Wall -Wextra -std=c99"
        rlRun -l "gcc test_sehandle.c -o test_sehandle -lselinux -pedantic -Wall -Wextra -std=c99"

        rlRun "useradd user"
    rlPhaseEnd

    rlPhaseStartTest "test call"
        createTestFiles

        rlRun "./test_restorecon $TmpDir" 0 "Calling selinux_restorecon"

        deleteTestFiles
    rlPhaseEnd

    rlPhaseStartTest "test call with flags"
        createTestFiles

        rlRun "./test_restorecon $TmpDir IGNORE_DIGEST IGNORE_DIGEST NOCHANGE VERBOSE PROGRESS RECURSE \
            SET_SPECFILE_CTX REALPATH XDEV" 0 "Calling selinux_restorecon with all flags"

        deleteTestFiles
    rlPhaseEnd

    rlPhaseStartTest "invalid path"
        rlRun "./test_restorecon EMPTY" 255 "Calling selinux_restorecon with empty path"

        # should probably return both 139 (segfault) or 255, but it is not so important
        rlRun "./test_restorecon NULL" 139,255 "Calling selinux_restorecon with null as path"
        rlRun "./test_restorecon NULL REALPATH" 139,255 "Calling selinux_restorecon with null as path and REALPATH flag"
        
        rlRun "./test_restorecon /nonexistent" 255 "Calling selinux_restorecon with nonexisting path"
    rlPhaseEnd

    rlPhaseStartTest "no flags"
        createTestFiles
        changeContext

        rlRun "strace -ostrace.out -s 64 ./test_restorecon $TmpDir" 0 "Calling selinux_restorecon"

        rlLogInfo "Checking lgetxattr calls"
        rlRun "grep lgetxattr strace.out | grep security.selinux > strace_xattr.out"

        rlRun "grep \"\\\"$TmpDir\\\"\"             strace_xattr.out | grep var_log_t"
        rlRun "grep \"\\\"$TmpDir/a\\\"\"           strace_xattr.out | grep var_log_t" 1
        rlRun "grep \"\\\"$TmpDir/afile\\\"\"       strace_xattr.out | grep var_log_t" 1
        rlRun "grep \"\\\"$TmpDir/a/b\\\"\"         strace_xattr.out | grep var_log_t" 1
        rlRun "grep \"\\\"$TmpDir/a/bfile\\\"\"     strace_xattr.out | grep var_log_t" 1
        rlRun "grep \"\\\"$TmpDir/a/b/cfile\\\"\"   strace_xattr.out | grep var_log_t" 1
        
        rlLogInfo "Checking lsetxattr calls"
        rlRun "grep lsetxattr strace.out | grep security.selinux > strace_xattr.out"

        rlRun "grep \"\\\"$TmpDir\\\"\"             strace_xattr.out | grep user_home_t"
        rlRun "grep \"\\\"$TmpDir/a\\\"\"           strace_xattr.out | grep user_home_t" 1
        rlRun "grep \"\\\"$TmpDir/afile\\\"\"       strace_xattr.out | grep user_home_t" 1
        rlRun "grep \"\\\"$TmpDir/a/b\\\"\"         strace_xattr.out | grep user_home_t" 1
        rlRun "grep \"\\\"$TmpDir/a/bfile\\\"\"     strace_xattr.out | grep user_home_t" 1
        rlRun "grep \"\\\"$TmpDir/a/b/cfile\\\"\"   strace_xattr.out | grep user_home_t" 1

        rlRun "rm -f strace.out"
        rlRun "rm -f strace_xattr.out"

        deleteTestFiles
    rlPhaseEnd

    rlPhaseStartTest "RECURSE flag"
        createTestFiles
        changeContext

        rlRun "strace -ostrace.out -s 64 ./test_restorecon $TmpDir RECURSE" 0 "Calling selinux_restorecon with RECURSE flag"

        rlLogInfo "Checking lgetxattr calls"
        rlRun "grep lgetxattr strace.out | grep security.selinux > strace_xattr.out"

        rlRun "grep \"\\\"$TmpDir\\\"\"             strace_xattr.out | grep var_log_t"
        rlRun "grep \"\\\"$TmpDir/a\\\"\"           strace_xattr.out | grep var_log_t"
        rlRun "grep \"\\\"$TmpDir/afile\\\"\"       strace_xattr.out | grep var_log_t"
        rlRun "grep \"\\\"$TmpDir/a/b\\\"\"         strace_xattr.out | grep var_log_t"
        rlRun "grep \"\\\"$TmpDir/a/bfile\\\"\"     strace_xattr.out | grep var_log_t"
        rlRun "grep \"\\\"$TmpDir/a/b/cfile\\\"\"   strace_xattr.out | grep var_log_t"

        rlLogInfo "Checking lsetxattr calls"
        rlRun "grep lsetxattr strace.out | grep security.selinux > strace_xattr.out"

        rlRun "grep \"\\\"$TmpDir\\\"\"             strace_xattr.out | grep user_home_t"
        rlRun "grep \"\\\"$TmpDir/a\\\"\"           strace_xattr.out | grep user_home_t"
        rlRun "grep \"\\\"$TmpDir/afile\\\"\"       strace_xattr.out | grep user_home_t"
        rlRun "grep \"\\\"$TmpDir/a/b\\\"\"         strace_xattr.out | grep user_home_t"
        rlRun "grep \"\\\"$TmpDir/a/bfile\\\"\"     strace_xattr.out | grep user_home_t"
        rlRun "grep \"\\\"$TmpDir/a/b/cfile\\\"\"   strace_xattr.out | grep user_home_t"

        rlRun "rm -f strace.out"
        rlRun "rm -f strace_xattr.out"

        deleteTestFiles
    rlPhaseEnd

    rlPhaseStartTest "NOCHANGE flag"
        createTestFiles
        changeContext

        rlRun "strace -ostrace.out -s 64 ./test_restorecon $TmpDir RECURSE NOCHANGE" 0 "Calling selinux_restorecon with NOCHANGE flag"

        rlLogInfo "Checking lsetxattr calls"
        rlRun "grep lsetxattr strace.out" 1

        rlRun "rm -f strace.out"

        deleteTestFiles
    rlPhaseEnd

    rlPhaseStartTest "/sys directory"
        # directory that supports security.restorecon_last xattr
        rlRun "strace -ostrace.out -s 64 ./test_restorecon /var/log RECURSE NOCHANGE" 0 "Calling selinux_restorecon on /tmp"

        rlRun "grep security.restorecon_last strace.out" 0

        # directory that does not supports security.restorecon_last xattr
        rlRun "strace -ostrace.out -s 64 ./test_restorecon /sys RECURSE NOCHANGE" 0 "Calling selinux_restorecon on /sys"

        rlRun "grep security.restorecon_last strace.out" 1

        rlRun "rm -f strace.out"
    rlPhaseEnd

    rlPhaseStartTest "no IGNORE_DIGEST flag"
        createTestFiles

        # run restorecon first time
        rlRun "strace -ostrace.out -s 64 ./test_restorecon $TmpDir RECURSE" 0 "Calling selinux_restorecon for the first time"

        rlLogInfo "Checking lgetxattr calls"
        rlRun "grep lgetxattr strace.out | grep security.selinux > strace_xattr.out"

        rlRun "grep \"\\\"$TmpDir\\\"\"             strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a\\\"\"           strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/afile\\\"\"       strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a/b\\\"\"         strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a/bfile\\\"\"     strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a/b/cfile\\\"\"   strace_xattr.out"

        # run restorecon second time
        rlRun "strace -ostrace.out -s 64 ./test_restorecon $TmpDir RECURSE" 0 "Calling selinux_restorecon for the second time"

        rlLogInfo "Checking lgetxattr calls"
        rlRun "grep lgetxattr strace.out | grep security.selinux" 1

        rlRun "rm -f strace.out"
        rlRun "rm -f strace_xattr.out"

        deleteTestFiles
    rlPhaseEnd

    rlPhaseStartTest "IGNORE_DIGEST flag"
        createTestFiles

        # run restorecon first time
        rlRun "strace -ostrace.out -s 64 ./test_restorecon $TmpDir RECURSE" 0 "Calling selinux_restorecon for the first time"

        rlLogInfo "Checking lgetxattr calls"
        rlRun "grep lgetxattr strace.out | grep security.selinux > strace_xattr.out"

        rlRun "grep \"\\\"$TmpDir\\\"\"             strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a\\\"\"           strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/afile\\\"\"       strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a/b\\\"\"         strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a/bfile\\\"\"     strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a/b/cfile\\\"\"   strace_xattr.out"

        # run restorecon second time
        rlRun "strace -ostrace.out -s 64 ./test_restorecon $TmpDir RECURSE IGNORE_DIGEST" 0 "Calling selinux_restorecon for the second time"

        rlLogInfo "Checking lgetxattr calls"
        rlRun "grep lgetxattr strace.out | grep security.selinux > strace_xattr.out"

        rlRun "grep \"\\\"$TmpDir\\\"\"             strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a\\\"\"           strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/afile\\\"\"       strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a/b\\\"\"         strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a/bfile\\\"\"     strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a/b/cfile\\\"\"   strace_xattr.out"

        rlRun "rm -f strace.out"
        rlRun "rm -f strace_xattr.out"

        deleteTestFiles
    rlPhaseEnd

    rlPhaseStartTest "selinux_restorecon_set_exclude_list"
        createTestFiles

        # empty exclude list
        rlRun "strace -ostrace.out -s 64 ./test_exclude_list EMPTY $TmpDir" 0 "Calling selinux_restorecon_set_exclude_list with empty list"

        rlLogInfo "Checking lgetxattr calls"
        rlRun "grep lgetxattr strace.out | grep security.selinux > strace_xattr.out"

        rlRun "grep \"\\\"$TmpDir\\\"\"             strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a\\\"\"           strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/afile\\\"\"       strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a/b\\\"\"         strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a/bfile\\\"\"     strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a/b/cfile\\\"\"   strace_xattr.out"

        # null as list
        if rlIsFedora ">=26"; then
            rlRun "strace -ostrace.out -s 64 ./test_exclude_list NULL $TmpDir" 139 "Calling selinux_restorecon_set_exclude_list with null as list"
        else
            rlRun "strace -ostrace.out -s 64 ./test_exclude_list NULL $TmpDir" 0 "Calling selinux_restorecon_set_exclude_list with null as list"

            rlLogInfo "Checking lgetxattr calls"
            rlRun "grep lgetxattr strace.out | grep security.selinux > strace_xattr.out"

            rlRun "grep \"\\\"$TmpDir\\\"\"             strace_xattr.out"
            rlRun "grep \"\\\"$TmpDir/a\\\"\"           strace_xattr.out"
            rlRun "grep \"\\\"$TmpDir/afile\\\"\"       strace_xattr.out"
            rlRun "grep \"\\\"$TmpDir/a/b\\\"\"         strace_xattr.out"
            rlRun "grep \"\\\"$TmpDir/a/bfile\\\"\"     strace_xattr.out"
            rlRun "grep \"\\\"$TmpDir/a/b/cfile\\\"\"   strace_xattr.out"
        fi

        # exclude $TmpDir/a
        rlRun "strace -ostrace.out -s 64 ./test_exclude_list $TmpDir/a $TmpDir" 0 "Calling selinux_restorecon_set_exclude_list"

        rlLogInfo "Checking lgetxattr calls"
        rlRun "grep lgetxattr strace.out | grep security.selinux > strace_xattr.out"

        rlRun "grep \"\\\"$TmpDir\\\"\"             strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a\\\"\"           strace_xattr.out" 1
        rlRun "grep \"\\\"$TmpDir/afile\\\"\"       strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a/b\\\"\"         strace_xattr.out" 1
        rlRun "grep \"\\\"$TmpDir/a/bfile\\\"\"     strace_xattr.out" 1
        rlRun "grep \"\\\"$TmpDir/a/b/cfile\\\"\"   strace_xattr.out" 1

        rlRun "rm -f strace.out"
        rlRun "rm -f strace_xattr.out"

        deleteTestFiles
    rlPhaseEnd

    rlPhaseStartTest "selinux_restorecon_set_sehandle"
        createTestFiles

        # null sehandle
        rlRun "./test_sehandle NULL $TmpDir" 139,255 "Calling selinux_restorecon_set_sehandle with null handle"

        # invalid sehandle
        rlRun "./test_sehandle INVALID $TmpDir" 139,255 "Calling selinux_restorecon_set_sehandle with invalid handle"

        # default sehandle
        rlRun "strace -ostrace.out -s 64 ./test_sehandle DEFAULT $TmpDir" 0 "Calling selinux_restorecon_set_sehandle with default handle"

        rlLogInfo "Checking lgetxattr calls"
        rlRun "grep lgetxattr strace.out | grep security.selinux > strace_xattr.out"

        rlRun "grep \"\\\"$TmpDir\\\"\"             strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a\\\"\"           strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/afile\\\"\"       strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a/b\\\"\"         strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a/bfile\\\"\"     strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a/b/cfile\\\"\"   strace_xattr.out"

        # custom sehandle
        rlRun "strace -ostrace.out -s 64 ./test_sehandle CUSTOM $TmpDir" 0 "Calling selinux_restorecon_set_sehandle with custom handle"

        rlLogInfo "Checking lgetxattr calls"
        rlRun "grep lgetxattr strace.out | grep security.selinux > strace_xattr.out"

        rlRun "grep \"\\\"$TmpDir\\\"\"             strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a\\\"\"           strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/afile\\\"\"       strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a/b\\\"\"         strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a/bfile\\\"\"     strace_xattr.out"
        rlRun "grep \"\\\"$TmpDir/a/b/cfile\\\"\"   strace_xattr.out"

        rlRun "rm -f strace.out"
        rlRun "rm -f strace_xattr.out"

        deleteTestFiles
    rlPhaseEnd

    rlPhaseStartCleanup
        rlRun "rm -f test_restorecon test_exclude_list test_sehandle"

        rlRun "userdel -r user"
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd
