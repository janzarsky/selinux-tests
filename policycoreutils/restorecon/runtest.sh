#!/bin/bash
# vim: dict=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/policycoreutils/Sanity/restorecon
#   Description: does restorecon work correctly ?
#   Author: Milos Malik <mmalik@redhat.com>
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   Copyright (c) 2011 Red Hat, Inc. All rights reserved.
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

# Include rhts environment
. /usr/bin/rhts-environment.sh
. /usr/share/beakerlib/beakerlib.sh

PACKAGE="policycoreutils"

rlJournalStart
    rlPhaseStartSetup
        rlAssertRpm ${PACKAGE}
        if which mcstrans; then
            rlServiceStop mcstrans
        elif which mcstransd; then
            rlServiceStop mcstransd
        fi
        rlRun "rpm -qf `which restorecon` | grep ${PACKAGE}"
        rlRun "setenforce 1"
        rlRun "sestatus"
        rlRun "setsebool allow_domain_fd_use on"
    rlPhaseEnd

    rlPhaseStartTest "Functional test"

        TESTDIR="/opt/restorecon_testdir"
        DIRS="correct.dir incorrect1.dir incorrect2.dir customizable.dir"
        FILES="correct.file incorrect.file customizable.file"

        rlRun "make -f /usr/share/selinux/devel/Makefile"
        rlRun "semodule -i testpolicy.pp"

        rlFileBackup /etc/selinux/targeted/contexts/customizable_types
        rlRun "echo 'customizable_t' >> /etc/selinux/targeted/contexts/customizable_types"

        # Here is the testing dirs and files structure
        # all the files have initial context corresponding to their names

        # ./
        # correct.file
        # incorrect.file
        # customizable.file

        # correct.dir/
        #     correct.file
        #     incorrect.file
        #     customizable.file

        # incorrect1.dir/
        #     correct.file
        #     incorrect.file
        #     customizable.file

        # incorrect2.dir/
        #     correct.file
        #     incorrect.file
        #     customizable.file

        # customizable.dir/
        #     correct.file
        #     incorrect.file
        #     customizable.file

        # Function to set initial contexts
        function set_contexts {
            # Set the intended contexts
            rlLog "Setting initial contexts of testing dirs..."
            restorecon -R $TESTDIR
            for ITEM in `find . -name 'incorrect*'`; do
                chcon -t incorrect_t $ITEM
            done
            for ITEM in `find . -name 'customizable*'`; do
                chcon -t customizable_t $ITEM
            done
        }

        # Check that files in dir $1 have the initial contexts
        function check_initial_contexts {
            if echo $1 | grep -q 'incorrect.dir'; then 
                rlRun "ls -ladZ $1 | grep :incorrect_t"
            elif echo $1 | grep -q 'correct.dir'; then
                rlRun "ls -ladZ $1 | grep :correct_t"
            elif echo $1 | grep -q 'customizable.dir'; then
                rlRun "ls -ladZ $1 | grep :customizable_t"
            fi
            rlRun "ls -ladZ $1/* | grep '\<correct.file' | grep ':correct_t'"
            rlRun "ls -ladZ $1/* | grep '\<incorrect.file' | grep ':incorrect_t'"
            rlRun "ls -ladZ $1/* | grep '\<customizable.file' | grep ':customizable_t'"
        }

        # Check that files matching with $2 in dir $1 have context $3
        function check_contexts {
            rlRun "find $1 -name '$2' >files"
            rlRun "[ $(cat files | wc -l) -gt 0 ]"

            for f in $files; do
                rlRun "ls -ladZ $f | grep :$3"
            done
            rlRun "rm files"
        }

        # Create the testing dirs and files
        rlRun "mkdir -p $TESTDIR"
        rlRun "pushd $TESTDIR"
        rlRun "mkdir $DIRS"
        rlRun "touch $FILES"
        for DIR in $DIRS; do
            rlRun "pushd $DIR"
            rlRun "touch $FILES"
            rlRun "popd"
        done

        set_contexts

        echo
        rlLog "Checking initial contexts of testing dirs..."
        # Check the contexts are set properly
        check_initial_contexts '.'
        check_initial_contexts 'incorrect1.dir'
        check_initial_contexts 'incorrect2.dir'
        check_initial_contexts 'correct.dir'
        check_initial_contexts 'customizable.dir'
        check_contexts '.' 'incorrect*' 'incorrect_t'
        check_contexts '.' 'correct*' 'correct_t'
        check_contexts '.' 'customizable*' 'customizable_t'

        # -e directory
        #     exclude a directory (repeat the option to exclude more than one directory).

        echo
        rlLog "-e directory"
        set_contexts
        rlRun "restorecon -RF -e $TESTDIR/incorrect2.dir $TESTDIR"
        for ITEM in `ls *.file`; do rlRun "ls -ladZ $ITEM | grep correct_t"; done
        check_contexts 'incorrect1.dir' '*' 'correct_t'
        check_contexts 'customizable.dir' '*' 'correct_t'
        check_initial_contexts 'incorrect2.dir'
        rlRun "ls -ladZ incorrect2.dir | grep incorrect_t"

        # -f infilename
        #     infilename contains a list of files to be processed. Use - for stdin.

        echo
        rlLog "-f filename"
        set_contexts
        rlRun "cat > ../file_list <<EOF
./customizable.file
./customizable.dir
./correct.dir/customizable.file
./incorrect1.dir/customizable.file
./incorrect2.dir/customizable.file
./customizable.dir/customizable.file
EOF"
        if rlIsRHEL 5; then chcon -t file_t ../file_list ;fi
        rlRun "restorecon -F -f ../file_list"
        check_contexts '.' 'incorrect*' 'incorrect_t'
        check_contexts '.' 'correct*' 'correct_t'
        check_contexts '.' 'customizable*' 'correct_t'
        rlRun "rm -f ../file_list"


        echo
        rlLog "-f -       Input from stdin"
        set_contexts
        rlRun "echo -e 'incorrect2.dir\ncustomizable.file\nincorrect.file' | restorecon -f -"
        check_initial_contexts 'incorrect1.dir'
        check_initial_contexts 'correct.dir'
        check_initial_contexts 'customizable.dir'
        check_contexts 'incorrect2.dir' '*' 'correct_t'
        rlRun "ls -ladZ customizable.file | grep customizable_t"
        rlRun "ls -ladZ incorrect.file | grep :correct_t"

        # -F  Force reset of context to match file_context for customizable files, and
        #     the default file context, changing the user, role, range portion as well
        #     as the type.

        echo
        rlLog "-F     Force reset of customizable types"
        set_contexts
        rlRun "restorecon -RF $TESTDIR"
        check_contexts '.' '*' 'correct_t'

    # This feature is from RHEL6 further
    if ! rlIsRHEL; then
        echo
        rlLog "-F     Force reset of the whole context"
        set_contexts
        chcon -u staff_u *.file
        rlRun "ls -laZ correct.file | grep staff_u"
        rlRun "ls -laZ incorrect.file | grep staff_u"
        rlRun "ls -laZ customizable.file | grep staff_u"
        rlRun "restorecon -R $TESTDIR"
        rlRun "ls -laZ correct.file | grep staff_u"
        rlRun "ls -laZ incorrect.file | grep staff_u"
        rlRun "ls -laZ customizable.file | grep staff_u"
        rlRun "restorecon -RF $TESTDIR"
        rlRun "ls -laZ correct.file | grep system_u"
        rlRun "ls -laZ incorrect.file | grep system_u"
        rlRun "ls -laZ customizable.file | grep system_u"
    fi
 
        # -i  ignore files that do not exist.

        rlRun "restorecon non-existent-file" 1-255
        rlRun "restorecon -i non-existent-file"

        # -n  don't change any file labels (passive check).

        echo
        rlLog "-n      dry-run"
        set_contexts
        rlRun "restorecon -RF -n $TESTDIR"
        check_contexts '.' 'incorrect*' 'incorrect_t'
        check_contexts '.' 'correct*' 'correct_t'
        check_contexts '.' 'customizable*' 'customizable_t'

        # -o outfilename
        #     Deprecated,  SELinux  policy will probably block this access.  Use shell
        #     redirection to save list of files with incorrect context in filename.

        #  ----not tested yet 

        # -R, -r change  files  and directories file labels recursively (descend directo‐
        #     ries).
        #     Note: restorecon reports warnings on paths without default  labels  only
        #     if called non-recursively or in verbose mode.

        set_contexts
        rlRun "restorecon -R $TESTDIR"
        check_contexts '.' '*corr*' 'correct_t'
        check_contexts '.' 'customizable*' 'customizable_t'

        #     ...by default it does not operate recursively on directories

        set_contexts
        rlRun "restorecon $TESTDIR"
        check_initial_contexts 'incorrect1.dir'
        check_initial_contexts 'incorrect2.dir'
        check_initial_contexts 'correct.dir'
        check_initial_contexts 'customizable.dir'
        rlRun "ls -ladZ customizable.file | grep customizable_t"
        rlRun "ls -ladZ incorrect.file | grep :incorrect_t"
        rlRun "ls -ladZ correct.file | grep :correct_t"

        # -v show changes in file labels, if type or role are going to be changed.

        #  ----not tested yet 

    # -0 option is not present in RHEL5
    if ! rlIsRHEL 5; then
        # -0 the  separator  for  the input items is assumed to be the null character
        #     (instead of the white space).  The quotes and the  backslash  characters
        #     are  also  treated as normal characters that can form valid input.  This
        #     option finally also disables the end of file string,  which  is  treated
        #     like  any  other  argument.  Useful when input items might contain white
        #     space, quote marks or backslashes.  The -print0 option of GNU find  pro‐
        #     duces input suitable for this mode.

        echo
        rlLog "-0"
        set_contexts
        rlRun "find . -print0 | restorecon -f - -0"
        check_contexts '.' '*corr*' 'correct_t'
        check_contexts '.' 'customizable*' 'customizable_t'

        echo
        rlLog "-0   with  -F"
        set_contexts
        rlRun "find . -print0 | restorecon -F -f - -0"
        check_contexts '.' '*' 'correct_t'

    fi

        # If a file object does not have a context, restorecon will write the default
        # context to the file object's extended attributes.

        #  ----not tested yet 


        # Cleanup

        rlRun "popd"
        rlRun "rm -rf /opt/restorecon_testdir"
        rlFileRestore
        rlRun "semodule -r testpolicy"
    rlPhaseEnd

    # This is RFE from RHEL6 and further versions
    if ! rlIsRHEL 5;then
    rlPhaseStartTest
        # META-Fixed-In: policycoreutils-2.0.83-19.14.el6
        rlRun "pushd /root"
        rlRun "touch test-file"
        rlRun "mkdir test-dir"
        for ITEM in "test-file" "test-dir" ; do
            rlRun "chcon -u staff_u -t shadow_t -l s0:c1 ${ITEM}"
            rlRun "ls -dZ ${ITEM} | grep staff_u:object_r:shadow_t:s0:c1"
            rlRun "restorecon -v ${ITEM}" 0,1
            rlRun "ls -dZ ${ITEM} | grep staff_u:object_r:admin_home_t:s0:c1"
            rlRun "restorecon -F -v ${ITEM}" 0,1
            rlRun "ls -dZ ${ITEM} | grep system_u:object_r:admin_home_t:s0"
        done
        rlRun "rm -rf test-dir"
        rlRun "rm -f test-file"
        rlRun "popd"
    rlPhaseEnd
    fi

    rlPhaseStartTest
        # META-Fixed-In: policycoreutils-2.0.83-19.16.el6
        rlRun "pushd /root"
        rlRun "touch test-file"
        rlRun "mkdir test-dir"
        for ITEM in "test-file" "test-dir" ; do
            rlRun "chcon -t tmp_t ${ITEM}"
            rlRun "ls -dZ ${ITEM}"
            rlRun "chattr +i ${ITEM}"
            rlRun "restorecon -v ${ITEM}" 1-255
            rlRun "chattr -i ${ITEM}"
            rlRun "ls -dZ ${ITEM}"
            rlRun "restorecon -v ${ITEM}"
            rlRun "ls -dZ ${ITEM}"
        done
        rlRun "rm -rf test-dir"
        rlRun "rm -f test-file"
        rlRun "popd"
    rlPhaseEnd

    # The bug was closed as NEXTRELEASE for RHEL5
    if ! rlIsRHEL 5; then
    rlPhaseStartTest
        rlRun "touch ~/test-file"
        rlRun "restorecon -vF ~/test-file"
        rlRun "restorecon -vF ~/test-file | grep \"reset.*context\"" 1
        rlRun "rm -f ~/test-file"

        rlRun "mkdir ~/test-dir"
        rlRun "restorecon -vF ~/test-dir"
        rlRun "restorecon -vF ~/test-dir | grep \"reset.*context\"" 1
        rlRun "rm -rf ~/test-dir"
    rlPhaseEnd
    fi

    rlPhaseStartCleanup
        if which mcstrans; then
            rlServiceRestore mcstrans
        elif which mcstransd; then
            rlServiceRestore mcstransd
        fi
    rlPhaseEnd
    rlJournalPrintText
rlJournalEnd

