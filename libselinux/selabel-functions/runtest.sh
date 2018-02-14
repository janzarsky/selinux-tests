#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/libselinux/Sanity/selabel-functions
#   Description: Test selabel functions
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

rlJournalStart
    rlPhaseStartSetup
        rlAssertRpm ${PACKAGE}
        rlAssertRpm ${PACKAGE}-devel
        rlAssertRpm "glibc"
        rlAssertRpm "gcc"

        if rlIsRHEL 6; then
            rlRun -l "gcc test_open.c -o test_open -lselinux -Wall -Wextra -std=c99 -DRHEL6"
            rlRun -l "gcc test_lookup.c -o test_lookup -lselinux -Wall -Wextra -std=c99 -DRHEL6"
            rlRun -l "gcc test_stats.c -o test_stats -lselinux -Wall -Wextra -std=c99 -DRHEL6"
        else
            rlRun -l "gcc test_open.c -o test_open -lselinux -Wall -Wextra -std=c99"
            rlRun -l "gcc test_lookup.c -o test_lookup -lselinux -Wall -Wextra -std=c99"
            rlRun -l "gcc test_partial.c -o test_partial -lselinux -Wall -Wextra -std=c99"
            rlRun -l "gcc test_best.c -o test_best -lselinux -Wall -Wextra -std=c99"
            rlRun -l "gcc test_stats.c -o test_stats -lselinux -Wall -Wextra -std=c99"
            rlRun -l "gcc test_digest.c -o test_digest -lselinux -Wall -Wextra -std=c99"
        fi

        rlRun "TmpDir=\$(mktemp -d)" 0 "Creating tmp directory"
    rlPhaseEnd

    rlPhaseStartTest "selabel_open"
        # syntax: ./test_open BACKEND OPT_PATH OPT_SUBSET OPT_VALIDATE OPT_BASEONLY [nopt]

        rlLogInfo "Normal run"
        rlRun "./test_open CTX_FILE NULL NULL 0 0"

        rlLogInfo "Backends"
        rlRun "./test_open CTX_MEDIA NULL NULL 0 0" 0
        rlRun "./test_open CTX_X NULL NULL 0 0" 0
        rlRun "./test_open CTX_DB NULL NULL 0 0" 0
        if rlIsRHEL "7" || rlIsFedora "<26"; then
            rlRun "./test_open CTX_ANDROID_PROP NULL NULL 0 0" 0
            rlRun "./test_open 5 NULL NULL 0 0" 22
        fi
        rlRun "./test_open 2147483647 NULL NULL 0 0" 22

        rlLogInfo "Parameter nopt"
        rlRun "./test_open CTX_FILE NULL NULL 0 0 2147483647" 22,139
        rlRun "./test_open CTX_MEDIA NULL NULL 0 0 2147483647" 22,139
        rlRun "./test_open CTX_X NULL NULL 0 0 2147483647" 22,139
        rlRun "./test_open CTX_DB NULL NULL 0 0 2147483647" 22,139
        if rlIsRHEL "7" || rlIsFedora "<26"; then
            rlRun "./test_open CTX_ANDROID_PROP NULL NULL 0 0 2147483647" 22,139
        fi

        rlRun "./test_open CTX_FILE NULL NULL 0 0 1"
        rlRun "./test_open CTX_MEDIA NULL NULL 0 0 1"
        rlRun "./test_open CTX_X NULL NULL 0 0 1"
        rlRun "./test_open CTX_DB NULL NULL 0 0 1"
        if rlIsRHEL "7" || rlIsFedora "<26"; then
            rlRun "./test_open CTX_ANDROID_PROP NULL NULL 0 0 1"
        fi

        rlRun "./test_open CTX_FILE NULL NULL 0 0 0"
        rlRun "./test_open CTX_MEDIA NULL NULL 0 0 0"
        rlRun "./test_open CTX_X NULL NULL 0 0 0"
        rlRun "./test_open CTX_DB NULL NULL 0 0 0"
        if rlIsRHEL "7" || rlIsFedora "<26"; then
            rlRun "./test_open CTX_ANDROID_PROP NULL NULL 0 0 0"
        fi

        rlRun "./test_open CTX_FILE NULL NULL 0 0 -1" 22,139
        rlRun "./test_open CTX_MEDIA NULL NULL 0 0 -1" 22,139
        rlRun "./test_open CTX_X NULL NULL 0 0 -1" 22,139
        rlRun "./test_open CTX_DB NULL NULL 0 0 -1" 22,139
        if rlIsRHEL "7" || rlIsFedora "<26"; then
            rlRun "./test_open CTX_ANDROID_PROP NULL NULL 0 0 -1" 22,139
        fi

        rlLogInfo "Path option"
        rlRun "cat > $TmpDir/my_contexts <<EOF
EOF"

        rlRun "./test_open CTX_FILE $TmpDir/my_contexts NULL 0 0"
        rlRun "./test_open CTX_MEDIA $TmpDir/my_contexts NULL 0 0"
        rlRun "./test_open CTX_X $TmpDir/my_contexts NULL 0 0"
        rlRun "./test_open CTX_DB $TmpDir/my_contexts NULL 0 0"
        if rlIsRHEL "7" || rlIsFedora "<26"; then
            rlRun "./test_open CTX_ANDROID_PROP $TmpDir/my_contexts NULL 0 0"
        fi

        rlRun "./test_open CTX_FILE /nonexistent NULL 0 0" 2
        rlRun "./test_open CTX_MEDIA /nonexistent NULL 0 0" 2
        rlRun "./test_open CTX_X /nonexistent NULL 0 0" 2
        rlRun "./test_open CTX_DB /nonexistent NULL 0 0" 2
        if rlIsRHEL "7" || rlIsFedora "<26"; then
            rlRun "./test_open CTX_ANDROID_PROP /nonexistent NULL 0 0" 2
        fi

        rlLogInfo "Subset option"
        rlRun "./test_open CTX_FILE NULL /etc 0 0"

        rlLogInfo "Baseonly option"
        rlRun "./test_open CTX_FILE NULL NULL 0 1"

        rlLogInfo "Validate option"
        rlRun "./test_open CTX_FILE NULL NULL 1 0"
        rlRun "./test_open CTX_MEDIA NULL NULL 1 0"
        rlRun "./test_open CTX_X NULL NULL 1 0"
        rlRun "./test_open CTX_DB NULL NULL 1 0"
        if rlIsRHEL "7" || rlIsFedora "<26"; then
            rlRun "./test_open CTX_ANDROID_PROP NULL NULL 1 0"
        fi
    rlPhaseEnd

    rlPhaseStartTest "selabel_lookup and selabel_lookup_raw"
        rlLogInfo "Handle"
        rlRun "./test_lookup CTX_FILE NULL NULL 0 0 some_input 0 nohandle" 139
        rlRun "./test_lookup CTX_MEDIA NULL NULL 0 0 some_input 0 nohandle" 139
        rlRun "./test_lookup CTX_X NULL NULL 0 0 some_input 0 nohandle" 139
        rlRun "./test_lookup CTX_DB NULL NULL 0 0 some_input 0 nohandle" 139
        if rlIsRHEL "7" || rlIsFedora "<26"; then
            rlRun "./test_lookup CTX_ANDROID_PROP NULL NULL 0 0 some_input 0 nohandle" 139
        fi

        rlLogInfo "Path"
        if rlIsRHEL 6; then
            rlRun "./test_lookup CTX_FILE NULL NULL 0 0 NULL 0" 2,139
            rlRun "./test_lookup CTX_MEDIA NULL NULL 0 0 NULL 0" 2,139
            rlRun "./test_lookup CTX_X NULL NULL 0 0 NULL 0" 2,139
            rlRun "./test_lookup CTX_DB NULL NULL 0 0 NULL 0" 2,139
        else
            rlRun "./test_lookup CTX_FILE NULL NULL 0 0 NULL 0" 22
            rlRun "./test_lookup CTX_MEDIA NULL NULL 0 0 NULL 0" 22
            rlRun "./test_lookup CTX_X NULL NULL 0 0 NULL 0" 22
            rlRun "./test_lookup CTX_DB NULL NULL 0 0 NULL 0" 22
            # ANDROID_PROP backend does not set handle and returns NULL as handle
            # (see test_lookup.c for handling NULL handle)
            rlRun "./test_lookup CTX_ANDROID_PROP NULL NULL 0 0 NULL 0" 255
        fi
    rlPhaseEnd

    rlPhaseStartTest "validate option"
        rlLogInfo "Invalid entries"
        rlRun "cat > $TmpDir/my_contexts <<EOF
EOF"
        rlRun "./test_open CTX_FILE $TmpDir/my_contexts NULL 1 0 2> >(tee output >&2)" 0
        rlRun "grep 'line' output" 1

        rlRun "cat > $TmpDir/my_contexts <<EOF
one
EOF"
        if rlIsRHEL 6; then
            rlRun "./test_open CTX_FILE $TmpDir/my_contexts NULL 1 0 2> >(tee output >&2)" 0
        else
            rlRun "./test_open CTX_FILE $TmpDir/my_contexts NULL 1 0 2> >(tee output >&2)" 22
        fi
        rlRun "grep 'line 1 is missing fields' output"
        
        rlRun "cat > $TmpDir/my_contexts <<EOF
$TmpDir my_user_u:my_role_r:my_type_t:s0
EOF"
        if rlIsFedora "<27" || rlIsRHEL "<=7"; then
            rlRun "./test_open CTX_FILE $TmpDir/my_contexts NULL 1 0 2> >(tee output >&2)" 0
        else
            rlRun "./test_open CTX_FILE $TmpDir/my_contexts NULL 1 0 2> >(tee output >&2)" 22
        fi
        rlRun "grep 'line 1 has invalid context my_user_u:my_role_r:my_type_t:s0' output"
        
        rlRun "cat > $TmpDir/my_contexts <<EOF
$TmpDir invalid_file_type system_u:object_r:var_t:s0
EOF"
        if rlIsRHEL 6; then
            rlRun "./test_open CTX_FILE $TmpDir/my_contexts NULL 1 0 2> >(tee output >&2)" 0
        else
            rlRun "./test_open CTX_FILE $TmpDir/my_contexts NULL 1 0 2> >(tee output >&2)" 22
        fi
        rlRun "grep 'line 1 has invalid file type invalid_file_type' output"
        
        rlRun "cat > $TmpDir/my_contexts <<EOF
$TmpDir <<none>>
EOF"
        rlRun "./test_open CTX_FILE $TmpDir/my_contexts NULL 1 0 2> >(tee output >&2)" 0
        rlRun "grep 'line 1' output" 1
        
        rlRun "cat > $TmpDir/my_contexts <<EOF
#$TmpDir system_u:object_r:var_t:s0
$TmpDir system_u:object_r:var_t:s0 
EOF"
        rlRun "./test_open CTX_FILE $TmpDir/my_contexts NULL 1 0 2> >(tee output >&2)" 0
        rlRun "grep 'Multiple same specifications' output" 1

        rlLogInfo "Two same rules for the same path"
        rlRun "cat > $TmpDir/my_contexts <<EOF
$TmpDir system_u:object_r:var_t:s0
$TmpDir system_u:object_r:var_t:s0
EOF"
        rlRun "./test_open CTX_FILE $TmpDir/my_contexts NULL 1 0 2> >(tee output >&2)" 22
        rlRun "grep 'Multiple same specifications' output"

        rlLogInfo "Two different rules for the same path"
        rlRun "cat > $TmpDir/my_contexts <<EOF
$TmpDir system_u:object_r:bin_t:s0
$TmpDir system_u:object_r:usr_t:s0
EOF"
        rlRun "./test_open CTX_FILE $TmpDir/my_contexts NULL 1 0 2> >(tee output >&2)" 22
        rlRun "grep 'Multiple different specifications' output"

        rlLogInfo "Two different rules for same path but with different file type"
        rlRun "cat > $TmpDir/my_contexts <<EOF
$TmpDir -- system_u:object_r:bin_t:s0
$TmpDir -d system_u:object_r:usr_t:s0
EOF"
        rlRun "./test_open CTX_FILE $TmpDir/my_contexts NULL 1 0 2> >(tee output >&2)" 0
        rlRun "grep 'Multiple different specifications' output" 1

        rlLogInfo "Two different rules for same path one general and one with file type"
        rlRun "cat > $TmpDir/my_contexts <<EOF
$TmpDir system_u:object_r:bin_t:s0
$TmpDir -- system_u:object_r:usr_t:s0
EOF"
        rlRun "./test_open CTX_FILE $TmpDir/my_contexts NULL 1 0 2> >(tee output >&2)" 22
        rlRun "grep 'Multiple different specifications' output"
    rlPhaseEnd

    if rlIsRHEL ">=7" || rlIsFedora; then
    rlPhaseStartTest "file contexts files"
        rlLogInfo "subs file"
        rlRun "cat > $TmpDir/my_contexts <<EOF
$TmpDir system_u:object_r:var_t:s0
EOF"
        rlRun "cat > $TmpDir/my_contexts.subs <<EOF
/somepath $TmpDir
EOF"
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 /somepath 0 | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:var_t:s0' output" 0
        rlRun "rm -f $TmpDir/my_contexts.subs"
        
        rlLogInfo "subs_dist file"
        rlRun "cat > $TmpDir/my_contexts <<EOF
$TmpDir system_u:object_r:var_t:s0
EOF"
        rlRun "cat > $TmpDir/my_contexts.subs_dist <<EOF
/somepath $TmpDir
EOF"
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 /somepath 0 | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:var_t:s0' output" 0
        rlRun "rm -f $TmpDir/my_contexts.subs_dist"

        rlLogInfo "local file"
        rlRun "cat > $TmpDir/my_contexts <<EOF
EOF"
        rlRun "cat > $TmpDir/my_contexts.local <<EOF
$TmpDir system_u:object_r:var_t:s0
EOF"
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 $TmpDir 0 | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:var_t:s0' output" 0
        rlRun "rm -f $TmpDir/my_contexts.local"

        rlLogInfo "homedirs file"
        rlRun "cat > $TmpDir/my_contexts <<EOF
EOF"
        rlRun "cat > $TmpDir/my_contexts.homedirs <<EOF
$TmpDir system_u:object_r:var_t:s0
EOF"
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 $TmpDir 0 | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:var_t:s0' output" 0
        rlRun "rm -f $TmpDir/my_contexts.homedirs"

        rlLogInfo "Normal run"
        rlRun "./test_lookup CTX_FILE NULL NULL 0 0 /nonexistent 0 | tee output" 0 \
            "Run selabel_lookup"
        rlRun "grep 'selabel_lookup context: system_u:object_r:etc_runtime_t:s0' output" 0 \
            "Check context returned by selabel_lookup"
        rlRun "grep 'selabel_lookup_raw context: system_u:object_r:etc_runtime_t:s0' output" 0 \
            "Check context returned by selabel_lookup_raw"

        rlLogInfo "Context is <<none>>"
        rlRun "./test_lookup CTX_FILE NULL NULL 0 0 /tmp/somefile 0 2> >(tee output >&2)" 2 \
            "Run selabel_lookup on file with default context <<none>>"
        rlRun "grep 'selabel_lookup - ERROR: No such file or directory' output" 0
    rlPhaseEnd
    fi

    rlPhaseStartTest "media contexts files"
        rlLogInfo "Valid entries"
        rlRun "cat > $TmpDir/my_contexts <<EOF
floppy system_u:object_r:tmp_t:s0
cdrom system_u:object_r:var_t:s0
EOF"
        rlRun "./test_lookup CTX_MEDIA $TmpDir/my_contexts NULL 1 0 floppy 0 | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:tmp_t:s0' output"

        rlRun "cat > $TmpDir/my_contexts <<EOF
floppy system_u:object_r:tmp_t:s0
floppy system_u:object_r:var_t:s0
EOF"
        rlRun "./test_lookup CTX_MEDIA $TmpDir/my_contexts NULL 1 0 floppy 0 | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:tmp_t:s0' output"

        rlLogInfo "No entries"
        rlRun "cat > $TmpDir/my_contexts <<EOF
EOF"
        rlRun "./test_lookup CTX_MEDIA $TmpDir/my_contexts NULL 1 0 floppy 0 2> >(tee output >&2)" 2
        rlRun "grep 'selabel_lookup - ERROR: No such file or directory' output"

        rlLogInfo "Invalid entries"
        rlRun "cat > $TmpDir/my_contexts <<EOF
floppy
EOF"
        rlRun "./test_lookup CTX_MEDIA $TmpDir/my_contexts NULL 1 0 floppy 0 2> >(tee output >&2)" 2
        rlRun "grep 'line 1 is missing fields' output"

        rlRun "cat > $TmpDir/my_contexts <<EOF
floppy some_u:some_r:some_t:s0
EOF"
        rlRun "./test_lookup CTX_MEDIA $TmpDir/my_contexts NULL 1 0 floppy 0 2> >(tee output >&2)" 22
        rlRun "grep 'has invalid context some_u:some_r:some_t:s0' output"

#        defaultContext=$(cat /etc/selinux/targeted/contexts/removable_context)

#        rlLogInfo "empty contexts file"
#        rlRun "cat > $TmpDir/my_contexts <<EOF
#EOF"
#        rlRun "./test_lookup CTX_MEDIA $TmpDir/my_contexts NULL 1 0 floppy 0 | tee output" 0
#        rlRun "grep 'selabel_lookup context: $defaultContext' output" 0

#        rlLogInfo "invalid contexts file"
#        rlRun "cat > $TmpDir/my_contexts <<EOF
#some_removable some_u:some_r:some_t:s0
#EOF"
#        rlRun "./test_lookup CTX_MEDIA $TmpDir/my_contexts NULL 1 0 floppy 0 | tee output" 0
#        rlRun "grep 'selabel_lookup context: $defaultContext' output" 0

        rlLogInfo "CTX_MEDIA backend"
        # syntax: ./test_lookup CTX_MEDIA OPT_PATH NULL OPT_VALIDATE 0 path 0

        rlLogInfo "Normal run"
        rlRun "./test_lookup CTX_MEDIA NULL NULL 0 0 floppy 0 | tee output" 0 \
            "Run selabel_lookup"
        rlRun "grep 'selabel_lookup context: system_u:object_r:removable_device_t:s0' output" 0 \
            "Check context returned by selabel_lookup"
        rlRun "grep 'selabel_lookup_raw context: system_u:object_r:removable_device_t:s0' output" 0 \
            "Check context returned by selabel_lookup_raw"
    rlPhaseEnd

    rlPhaseStartTest "x contexts files"
        rlLogInfo "Valid entries"
        rlRun "cat > $TmpDir/my_contexts <<EOF
client * system_u:object_r:tmp_t:s0
selection PRIMARY system_u:object_r:var_t:s0
EOF"
        rlRun "./test_lookup CTX_X $TmpDir/my_contexts NULL 1 0 PRIMARY X_SELN | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:var_t:s0' output"

        rlLogInfo "Comments and empty lines"
        rlRun "cat > $TmpDir/my_contexts <<EOF
# some comment
client * system_u:object_r:tmp_t:s0

selection PRIMARY system_u:object_r:var_t:s0
EOF"
        rlRun "./test_lookup CTX_X $TmpDir/my_contexts NULL 1 0 PRIMARY X_SELN | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:var_t:s0' output"

        rlLogInfo "Object names"
        rlRun "cat > $TmpDir/my_contexts <<EOF
property * system_u:object_r:tmp_t:s0
selection * system_u:object_r:var_t:s0
extension * system_u:object_r:usr_t:s0
event * system_u:object_r:man_t:s0
client * system_u:object_r:lib_t:s0
poly_property * system_u:object_r:bin_t:s0
poly_selection * system_u:object_r:remote_t:s0
EOF"
        rlRun "./test_lookup CTX_X $TmpDir/my_contexts NULL 1 0 '*' X_PROP | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:tmp_t:s0' output"
        rlRun "./test_lookup CTX_X $TmpDir/my_contexts NULL 1 0 '*' X_SELN | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:var_t:s0' output"
        rlRun "./test_lookup CTX_X $TmpDir/my_contexts NULL 1 0 '*' X_EXT | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:usr_t:s0' output"
        rlRun "./test_lookup CTX_X $TmpDir/my_contexts NULL 1 0 '*' X_EVENT | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:man_t:s0' output"
        rlRun "./test_lookup CTX_X $TmpDir/my_contexts NULL 1 0 '*' X_CLIENT | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:lib_t:s0' output"
        rlRun "./test_lookup CTX_X $TmpDir/my_contexts NULL 1 0 '*' X_POLYPROP | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:bin_t:s0' output"
        rlRun "./test_lookup CTX_X $TmpDir/my_contexts NULL 1 0 '*' X_POLYSELN | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:remote_t:s0' output"

        rlLogInfo "Empty file"
        rlRun "cat > $TmpDir/my_contexts <<EOF
EOF"
        rlRun "./test_lookup CTX_X $TmpDir/my_contexts NULL 1 0 PRIMARY X_SELN" 2

        rlLogInfo "Invalid entries"
        rlRun "cat > $TmpDir/my_contexts <<EOF
property
EOF"
        rlRun "./test_lookup CTX_X $TmpDir/my_contexts NULL 1 0 PRIMARY X_SELN 2> >(tee output >&2)" 2
        rlRun "grep 'line 1 is missing fields' output"

        rlRun "cat > $TmpDir/my_contexts <<EOF
property *
EOF"
        rlRun "./test_lookup CTX_X $TmpDir/my_contexts NULL 1 0 PRIMARY X_SELN 2> >(tee output >&2)" 2
        rlRun "grep 'line 1 is missing fields' output"

        rlRun "cat > $TmpDir/my_contexts <<EOF
some_property * system_u:object_r:tmp_t:s0
EOF"
        rlRun "./test_lookup CTX_X $TmpDir/my_contexts NULL 1 0 PRIMARY X_SELN 2> >(tee output >&2)" 2
        rlRun "grep 'line 1 has invalid object type some_property' output"

        rlLogInfo "Wildcard matching"
        rlRun "cat > $TmpDir/my_contexts <<EOF
property * system_u:object_r:tmp_t:s0
EOF"
        rlRun "./test_lookup CTX_X $TmpDir/my_contexts NULL 1 0 SOME_PROPERTY X_PROP | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:tmp_t:s0' output"

        rlRun "cat > $TmpDir/my_contexts <<EOF
property CUT_BUFFER0 system_u:object_r:tmp_t:s0
property * system_u:object_r:usr_t:s0
property CUT_BUFFER1 system_u:object_r:var_t:s0
EOF"
        rlRun "./test_lookup CTX_X $TmpDir/my_contexts NULL 1 0 CUT_BUFFER0 X_PROP | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:tmp_t:s0' output"
        rlRun "./test_lookup CTX_X $TmpDir/my_contexts NULL 1 0 CUT_BUFFER1 X_PROP | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:usr_t:s0' output"
        rlRun "./test_lookup CTX_X $TmpDir/my_contexts NULL 1 0 CUT_BUFFER2 X_PROP | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:usr_t:s0' output"

        rlLogInfo "Substitution matching"
        rlRun "cat > $TmpDir/my_contexts <<EOF
property CUT_BUFFER? system_u:object_r:tmp_t:s0
EOF"
        rlRun "./test_lookup CTX_X $TmpDir/my_contexts NULL 1 0 CUT_BUFFER3 X_PROP | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:tmp_t:s0' output"

        rlLogInfo "Normal run"
        rlRun "./test_lookup CTX_X NULL NULL 0 0 asdf X_CLIENT | tee output" 0 \
            "Run selabel_lookup"
        rlRun "grep 'selabel_lookup context: system_u:object_r:remote_t:s0' output" 0 \
            "Check context returned by selabel_lookup"
        rlRun "grep 'selabel_lookup_raw context: system_u:object_r:remote_t:s0' output" 0 \
            "Check context returned by selabel_lookup_raw"
    rlPhaseEnd

    rlPhaseStartTest "db contexts files"
        rlLogInfo "Valid entries"
        rlRun "cat > $TmpDir/my_contexts <<EOF
db_database     my_database     system_u:object_r:sepgsql_db_t:s0
db_schema       my_schema       system_u:object_r:sepgsql_schema_t:s0
db_view         my_view         system_u:object_r:sepgsql_view_t:s0
db_table        my_table        system_u:object_r:sepgsql_table_t:s0
db_column       my_column       system_u:object_r:var_t:s0
db_tuple        my_tuple        system_u:object_r:tmp_t:s0
db_procedure    my_procedure    system_u:object_r:sepgsql_proc_exec_t:s0
db_sequence     my_sequence     system_u:object_r:sepgsql_seq_t:s0
db_blob         my_blob         system_u:object_r:sepgsql_blob_t:s0
EOF"
        if rlIsRHEL ">=7" || rlIsFedora; then
            rlRun "cat >> $TmpDir/my_contexts <<EOF
db_language     my_language     system_u:object_r:sepgsql_lang_t:s0
db_exception    my_exception    system_u:object_r:usr_t:s0
db_datatype     my_datatype     system_u:object_r:bin_t:s0
EOF"
        fi

        rlRun "./test_lookup CTX_DB $TmpDir/my_contexts NULL 1 0 my_database DB_DATABASE | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:sepgsql_db_t:s0' output"
        rlRun "./test_lookup CTX_DB $TmpDir/my_contexts NULL 1 0 my_schema DB_SCHEMA | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:sepgsql_schema_t:s0' output"
        rlRun "./test_lookup CTX_DB $TmpDir/my_contexts NULL 1 0 my_view DB_VIEW | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:sepgsql_view_t:s0' output"
        rlRun "./test_lookup CTX_DB $TmpDir/my_contexts NULL 1 0 my_table DB_TABLE | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:sepgsql_table_t:s0' output"
        rlRun "./test_lookup CTX_DB $TmpDir/my_contexts NULL 1 0 my_column DB_COLUMN | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:var_t:s0' output"
        rlRun "./test_lookup CTX_DB $TmpDir/my_contexts NULL 1 0 my_tuple DB_TUPLE | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:tmp_t:s0' output"
        rlRun "./test_lookup CTX_DB $TmpDir/my_contexts NULL 1 0 my_procedure DB_PROCEDURE | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:sepgsql_proc_exec_t:s0' output"
        rlRun "./test_lookup CTX_DB $TmpDir/my_contexts NULL 1 0 my_sequence DB_SEQUENCE | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:sepgsql_seq_t:s0' output"
        rlRun "./test_lookup CTX_DB $TmpDir/my_contexts NULL 1 0 my_blob DB_BLOB | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:sepgsql_blob_t:s0' output"

        if rlIsRHEL ">=7" || rlIsFedora; then
            rlRun "./test_lookup CTX_DB $TmpDir/my_contexts NULL 1 0 my_language DB_LANGUAGE | tee output" 0
            rlRun "grep 'selabel_lookup context: system_u:object_r:sepgsql_lang_t:s0' output"
            rlRun "./test_lookup CTX_DB $TmpDir/my_contexts NULL 1 0 my_exception DB_EXCEPTION | tee output" 0
            rlRun "grep 'selabel_lookup context: system_u:object_r:usr_t:s0' output"
            rlRun "./test_lookup CTX_DB $TmpDir/my_contexts NULL 1 0 my_datatype DB_DATATYPE | tee output" 0
            rlRun "grep 'selabel_lookup context: system_u:object_r:bin_t:s0' output"
        fi

        rlLogInfo "Comments and empty lines"
        rlRun "cat > $TmpDir/my_contexts <<EOF
# something

db_database     my_database     system_u:object_r:sepgsql_db_t:s0
EOF"
        rlRun "./test_lookup CTX_DB $TmpDir/my_contexts NULL 1 0 my_database DB_DATABASE | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:sepgsql_db_t:s0' output"

        rlLogInfo "Wildcard matching"
        rlRun "cat > $TmpDir/my_contexts <<EOF
db_database     my_database     system_u:object_r:var_t:s0
db_database     my_database*    system_u:object_r:bin_t:s0
db_database     *               system_u:object_r:usr_t:s0
EOF"
        rlRun "./test_lookup CTX_DB $TmpDir/my_contexts NULL 1 0 my_database DB_DATABASE | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:var_t:s0' output"
        rlRun "./test_lookup CTX_DB $TmpDir/my_contexts NULL 1 0 my_database_asdf DB_DATABASE | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:bin_t:s0' output"
        rlRun "./test_lookup CTX_DB $TmpDir/my_contexts NULL 1 0 my_asdf DB_DATABASE | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:usr_t:s0' output"

        rlRun "cat > $TmpDir/my_contexts <<EOF
db_database     *       system_u:object_r:usr_t:s0
db_database     my_*    system_u:object_r:bin_t:s0
EOF"
        rlRun "./test_lookup CTX_DB $TmpDir/my_contexts NULL 1 0 my_database DB_DATABASE | tee output" 0
        rlRun "grep 'selabel_lookup context: system_u:object_r:usr_t:s0' output"

        rlLogInfo "Empty file"
        rlRun "cat > $TmpDir/my_contexts <<EOF
EOF"
        rlRun "./test_lookup CTX_DB $TmpDir/my_contexts NULL 1 0 my_database DB_DATABASE " 2

        rlLogInfo "Invalid entries"
        rlRun "cat > $TmpDir/my_contexts <<EOF
one
EOF"
        rlRun "./test_lookup CTX_DB $TmpDir/my_contexts NULL 0 0 my_database DB_DATABASE 2> >(tee output >&2)" 2
        rlRun "grep 'line 1 has invalid format' output"

        rlRun "cat > $TmpDir/my_contexts <<EOF
one two
EOF"
        rlRun "./test_lookup CTX_DB $TmpDir/my_contexts NULL 0 0 my_database DB_DATABASE 2> >(tee output >&2)" 2
        rlRun "grep 'line 1 has invalid format' output"

        rlRun "cat > $TmpDir/my_contexts <<EOF
one two three
EOF"
        rlRun "./test_lookup CTX_DB $TmpDir/my_contexts NULL 0 0 my_database DB_DATABASE 2> >(tee output >&2)" 2
        rlRun "grep 'line 1 has invalid object type one' output"

        rlRun "cat > $TmpDir/my_contexts <<EOF
one two three four
EOF"
        rlRun "./test_lookup CTX_DB $TmpDir/my_contexts NULL 0 0 my_database DB_DATABASE 2> >(tee output >&2)" 2
        rlRun "grep 'line 1 has invalid format' output"

        rlLogInfo "Normal run"
        rlRun "./test_lookup CTX_DB NULL NULL 0 0 my_database DB_DATABASE | tee output"
        rlRun "grep 'selabel_lookup context: system_u:object_r:sepgsql_db_t:s0' output"
        rlRun "grep 'selabel_lookup_raw context: system_u:object_r:sepgsql_db_t:s0' output"
    rlPhaseEnd

    if rlIsRHEL ">=7" || rlIsFedora; then
    rlPhaseStartTest "baseonly option"
        rlRun "cat > $TmpDir/my_contexts <<EOF
$TmpDir.* system_u:object_r:var_t:s0
EOF"
        rlRun "cat > $TmpDir/my_contexts.subs <<EOF
/somepath $TmpDir
EOF"
        rlRun "cat > $TmpDir/my_contexts.local <<EOF
$TmpDir/local system_u:object_r:bin_t:s0
EOF"
        rlRun "cat > $TmpDir/my_contexts.homedirs <<EOF
$TmpDir/homedirs system_u:object_r:usr_t:s0
EOF"

        # without baseonly option, the rule in my_contexts will be overridden
        # by a rule in my_contexts.local file
        # .subs file should work even with baseonly option
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 /somepath/local 0 | tee output" 0 \
            "Running selabel_open without baseonly option"
        rlRun "grep 'selabel_lookup context: system_u:object_r:bin_t:s0' output" 0

        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 1 /somepath/local 0 | tee output" 0 \
            "Running selabel_open with baseonly option"
        rlRun "grep 'selabel_lookup context: system_u:object_r:var_t:s0' output" 0

        # without baseonly option, the rule in my_contexts will be overridden
        # by a rule in my_contexts.homedirs file
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 /somepath/homedirs 0 | tee output" 0 \
            "Running selabel_open without baseonly option"
        rlRun "grep 'selabel_lookup context: system_u:object_r:usr_t:s0' output" 0

        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 1 /somepath/homedirs 0 | tee output" 0 \
            "Running selabel_open with baseonly option"
        rlRun "grep 'selabel_lookup context: system_u:object_r:var_t:s0' output" 0

        rlRun "rm -f $TmpDir/my_contexts.subs"
        rlRun "rm -f $TmpDir/my_contexts.local"
        rlRun "rm -f $TmpDir/my_contexts.homedirs"
    rlPhaseEnd
    fi

    rlPhaseStartTest "selabel_lookup advanced tests"
        rlLogInfo "Custom file contexts file"
        rlRun "cat > $TmpDir/my_contexts <<EOF
$TmpDir my_user_u:my_role_r:my_type_t:s0
EOF"

        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 $TmpDir 0 | tee output" 0
        rlRun "grep 'selabel_lookup context: my_user_u:my_role_r:my_type_t:s0' output" 0

        rlLogInfo "Rules for different file types"
        rlRun "cat > $TmpDir/my_contexts <<EOF
$TmpDir -- user_u:role_r:regular_type_t:s0
$TmpDir -b user_u:role_r:block_type_t:s0
$TmpDir -c user_u:role_r:char_type_t:s0
$TmpDir -d user_u:role_r:dir_type_t:s0
$TmpDir -p user_u:role_r:pipe_type_t:s0
$TmpDir -l user_u:role_r:symlink_type_t:s0
$TmpDir -s user_u:role_r:socket_type_t:s0
EOF"

        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 $TmpDir 0010000 | tee output"
        rlRun "grep 'selabel_lookup context: user_u:role_r:pipe_type_t:s0' output"
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 $TmpDir 0020000 | tee output"
        rlRun "grep 'selabel_lookup context: user_u:role_r:char_type_t:s0' output"
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 $TmpDir 0040000 | tee output"
        rlRun "grep 'selabel_lookup context: user_u:role_r:dir_type_t:s0' output"
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 $TmpDir 0060000 | tee output"
        rlRun "grep 'selabel_lookup context: user_u:role_r:block_type_t:s0' output"
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 $TmpDir 0100000 | tee output"
        rlRun "grep 'selabel_lookup context: user_u:role_r:regular_type_t:s0' output"
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 $TmpDir 0120000 | tee output"
        rlRun "grep 'selabel_lookup context: user_u:role_r:symlink_type_t:s0' output"
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 $TmpDir 0140000 | tee output"
        rlRun "grep 'selabel_lookup context: user_u:role_r:socket_type_t:s0' output"
    rlPhaseEnd

    if rlIsRHEL ">=7" || rlIsFedora; then
    rlPhaseStartTest "selabel_partial_match"
        # syntax: ./test_partial BACKEND OPT_PATH OPT_SUBSET OPT_VALIDATE OPT_BASEONLY path [nohandle]

        rlLogInfo "nonsupporting backends"
        rlRun "./test_partial CTX_MEDIA NULL NULL 0 0 /somedir | tee output" 0
        rlRun "grep 'selabel_partial_match: true' output" 0
        rlRun "./test_partial CTX_DB NULL NULL 0 0 /somedir | tee output" 0
        rlRun "grep 'selabel_partial_match: true' output" 0
        rlRun "./test_partial CTX_X NULL NULL 0 0 /somedir | tee output" 0
        rlRun "grep 'selabel_partial_match: true' output" 0

        rlLogInfo "null as handle"
        rlRun "./test_partial CTX_FILE NULL NULL 0 0 /somedir nohandle" 22,139
        
        rlLogInfo "nonexisting entry"
        rlRun "cat > $TmpDir/my_contexts <<EOF
EOF"
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 /somedir 0" 2

        rlRun "./test_partial CTX_FILE $TmpDir/my_contexts NULL 0 0 /somedir | tee output" 0
        rlRun "grep 'selabel_partial_match: false' output" 0

        rlLogInfo "full match"
        rlRun "cat > $TmpDir/my_contexts <<EOF
/somedir user_u:role_r:type_t:s0
EOF"
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 /somedir 0 | tee output" 0
        rlRun "grep 'selabel_lookup context: user_u:role_r:type_t:s0' output"

        rlRun "./test_partial CTX_FILE $TmpDir/my_contexts NULL 0 0 /somedir | tee output" 0
        rlRun "grep 'selabel_partial_match: true' output" 0

        rlLogInfo "partial match"
        rlRun "cat > $TmpDir/my_contexts <<EOF
/somedir42 user_u:role_r:type_t:s0
EOF"
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 /somedir 0" 2

        rlRun "./test_partial CTX_FILE $TmpDir/my_contexts NULL 0 0 /somedir | tee output" 0
        rlRun "grep 'selabel_partial_match: true' output" 0
    rlPhaseEnd
    fi

    if rlIsRHEL ">=7" || rlIsFedora; then
    rlPhaseStartTest "selabel_best_match"
        # syntax: ./test_best BACKEND OPT_PATH OPT_SUBSET OPT_VALIDATE OPT_BASEONLY path mode [nohandle]

        rlLogInfo "nonsupported backends"
        rlRun "./test_best CTX_MEDIA NULL NULL 0 0 /somedir 0" 95
        rlRun "./test_best CTX_DB NULL NULL 0 0 /somedir 0" 95
        rlRun "./test_best CTX_X NULL NULL 0 0 /somedir 0" 95

        rlLogInfo "null as handle"
        rlRun "./test_best CTX_FILE NULL NULL 0 0 /somedir 0 nohandle" 22,139

        rlLogInfo "null as key"
        rlRun "./test_best CTX_FILE NULL NULL 0 0 NULL 0" 22

        rlLogInfo "nonexisting entry"
        rlRun "cat > $TmpDir/my_contexts <<EOF
EOF"
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 /somedir 0" 2

        rlRun "./test_best CTX_FILE $TmpDir/my_contexts NULL 0 0 /somedir 0" 2

        rlLogInfo "exact match without aliases"
        rlRun "cat > $TmpDir/my_contexts <<EOF
/somedir user_u:role_r:type_t:s0
EOF"
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 /somedir 0 | tee output" 0
        rlRun "grep 'selabel_lookup context: user_u:role_r:type_t:s0' output"

        # same as selabel_lookup
        rlRun "./test_best CTX_FILE $TmpDir/my_contexts NULL 0 0 /somedir 0 | tee output" 0
        rlRun "grep 'selabel_lookup_best_match context: user_u:role_r:type_t:s0' output"

        rlLogInfo "regex match without aliases"
        rlRun "cat > $TmpDir/my_contexts <<EOF
/some.* user_u:role_r:type_t:s0
EOF"
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 /somedir 0 | tee output" 0
        rlRun "grep 'selabel_lookup context: user_u:role_r:type_t:s0' output"

        # same as selabel_lookup
        rlRun "./test_best CTX_FILE $TmpDir/my_contexts NULL 0 0 /somedir 0 | tee output" 0
        rlRun "grep 'selabel_lookup_best_match context: user_u:role_r:type_t:s0' output"

        rlLogInfo "exact match with exactly matching alias"
        rlRun "cat > $TmpDir/my_contexts <<EOF
/somedir user_u:role_r:orig_t:s0
/somesymlink user_u:role_r:link_t:s0
EOF"
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 /somedir 0 | tee output" 0
        rlRun "grep 'selabel_lookup context: user_u:role_r:orig_t:s0' output"

        # original context should be chosen when found
        rlRun "./test_best CTX_FILE $TmpDir/my_contexts NULL 0 0 /somedir 0 /somesymlink | tee output" 0
        rlRun "grep 'selabel_lookup_best_match context: user_u:role_r:orig_t:s0' output"

        rlLogInfo "no match with exactly matching alias"
        rlRun "cat > $TmpDir/my_contexts <<EOF
/somesymlink user_u:role_r:type_t:s0
EOF"
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 /somedir 0" 2

        # when there is no context for path, alias context should be chosen
        rlRun "./test_best CTX_FILE $TmpDir/my_contexts NULL 0 0 /somedir 0 /somesymlink | tee output" 0
        rlRun "grep 'selabel_lookup_best_match context: user_u:role_r:type_t:s0' output"

        rlLogInfo "no match with multiple exactly matching aliases"
        rlRun "cat > $TmpDir/my_contexts <<EOF
/firstsymlink user_u:role_r:first_t:s0
/secondsymlink user_u:role_r:second_t:s0
EOF"
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 /somedir 0" 2

        # with no context for path and multiple aliases, first exact match should be chosen
        rlRun "./test_best CTX_FILE $TmpDir/my_contexts NULL 0 0 /somedir 0 /firstsymlink /secondsymlink | tee output" 0
        rlRun "grep 'selabel_lookup_best_match context: user_u:role_r:first_t:s0' output"

        rlLogInfo "no match with multiple regex matching aliases"
        rlRun "cat > $TmpDir/my_contexts <<EOF
/short.* user_u:role_r:short_t:s0
/verylong.* user_u:role_r:long_t:s0
EOF"
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 /somedir 0" 2

        # with no context for path and multiple regex aliases, match with longest prefix should be chosen
        rlRun "./test_best CTX_FILE $TmpDir/my_contexts NULL 0 0 /somedir 0 /shortsymlink /verylongsymlink | tee output" 0
        rlRun "grep 'selabel_lookup_best_match context: user_u:role_r:long_t:s0' output"

        rlLogInfo "regex match with multiple regex matching aliases"
        rlRun "cat > $TmpDir/my_contexts <<EOF
/some.* user_u:role_r:orig_t:s0
/short.* user_u:role_r:short_t:s0
/verylong.* user_u:role_r:long_t:s0
EOF"
        rlRun "./test_lookup CTX_FILE $TmpDir/my_contexts NULL 0 0 /somedir 0 | tee output" 0
        rlRun "grep 'selabel_lookup context: user_u:role_r:orig_t:s0' output"

        # with regex matching context for path and multiple regex aliases, match with longest prefix should be chosen
        # (among both original path and aliases)
        rlRun "./test_best CTX_FILE $TmpDir/my_contexts NULL 0 0 /somedir 0 /shortsymlink /verylongsymlink | tee output" 0
        rlRun "grep 'selabel_lookup_best_match context: user_u:role_r:long_t:s0' output"
    rlPhaseEnd
    fi

    rlPhaseStartTest "selabel_stats"
        # syntax: ./test_stats BACKEND OPT_PATH OPT_VALIDATE [nohandle]

        rlLogInfo "calling selabel_test - not checking output"
        rlRun "cat > $TmpDir/my_contexts <<EOF
EOF"
        rlRun "./test_stats CTX_FILE $TmpDir/my_contexts 0" 0
        rlRun "./test_stats CTX_MEDIA $TmpDir/my_contexts 0" 0
        rlRun "./test_stats CTX_X $TmpDir/my_contexts 0" 0
        rlRun "./test_stats CTX_DB $TmpDir/my_contexts 0" 0
        if rlIsRHEL "7" || rlIsFedora "<26"; then
            rlRun "./test_stats CTX_ANDROID_PROP $TmpDir/my_contexts 0" 0
        fi
        
        rlRun "./test_stats CTX_FILE $TmpDir/my_contexts 0 nohandle" 139
    rlPhaseEnd

    if rlIsRHEL ">=7" || rlIsFedora; then
    rlPhaseStartTest "selabel_digest"
        # syntax: ./test_digest BACKEND OPT_PATH OPT_VALIDATE OPT_DIGEST [nohandle]

        rlRun "./test_digest CTX_FILE NULL 0 0" 22
        rlRun "./test_digest CTX_FILE NULL 0 0 nohandle" 139

        rlRun "./test_digest CTX_FILE NULL 0 1" 0
        rlRun "./test_digest CTX_MEDIA NULL 0 1" 0
        rlRun "./test_digest CTX_X NULL 0 1" 0
        rlRun "./test_digest CTX_DB NULL 0 1" 0

        rlRun "cat > $TmpDir/my_contexts <<EOF
EOF"
        if rlIsRHEL "7" || rlIsFedora "<26"; then
            rlRun "./test_digest CTX_ANDROID_PROP $TmpDir/my_contexts 0 1" 0
        fi
    rlPhaseEnd
    fi

    rlPhaseStartCleanup
        rlRun "rm -r $TmpDir" 0 "Removing tmp directory"
        rlRun "rm -f test_open test_lookup test_partial test_best test_stats test_digest output"
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd
