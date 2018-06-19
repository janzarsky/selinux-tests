#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/selinux-policy/Sanity/mounting
#   Description: Mount various filesystems, test context options.
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

PACKAGE="selinux-policy"

rlJournalStart
    rlPhaseStartSetup
        rlAssertRpm $PACKAGE
        rlAssertRpm "util-linux"
        rlAssertRpm "e2fsprogs"
        rlAssertRpm "dosfstools"

        # create a loopback device
        rlRun "fallocate -l 100M loop_file"
        rlRun "losetup -f loop_file"
        rlRun "losetup -l | grep loop_file"
        DEV="$(losetup -l | tail -n 1 | cut -f1 -d' ')"

        # create a mounting directory
        rlRun "mkdir mnt"
        MNT="$(realpath mnt)"
        rlRun "semanage fcontext -a -t user_tmp_t '$MNT/.*'"

        # list the context twice so that it will be visible even when the grep
        # fails
        function checkFC {
            ls -Z $1
            ls -Z $1 | grep $2
            return $?
        }
    rlPhaseEnd

    for FS in mkfs.ext2 mkfs.ext3 mkfs.ext4 mkfs.xfs ; do
    rlPhaseStartTest "Test $FS"
        # create the filesystem
        if [[ $FS = "mkfs.xfs" ]]; then
            rlRun "$FS $DEV -f"
        else
            rlRun "$FS $DEV -F"
        fi

        # create testfile and run restorecon on it
        rlRun "mount $DEV $MNT"
        rlRun "touch $MNT/testfile"
        rlRun "checkFC $MNT/testfile unlabeled_t"
        rlRun "restorecon $MNT/testfile -v"
        rlRun "checkFC $MNT/testfile user_tmp_t"
        rlRun "umount $DEV"

        # test the context option
        rlRun "mount $DEV $MNT -o 'context="unconfined_u:object_r:user_home_t:s0"'"
        rlRun "checkFC $MNT/testfile user_home_t"
        # create unlabeled file (because of the context option, xattrs won't be
        # assigned)
        rlRun "touch $MNT/unlabeled"
        rlRun "checkFC $MNT/unlabeled user_home_t"
        rlRun "umount $DEV"

        # test the defcontext option
        rlRun "mount $DEV $MNT -o 'defcontext="unconfined_u:object_r:tmp_t:s0"'"
        # file with xattrs
        rlRun "checkFC $MNT/testfile user_tmp_t"
        # file without xattrs
        rlRun "checkFC $MNT/unlabeled tmp_t"
        rlRun "umount $DEV"
    rlPhaseEnd
    done

    rlPhaseStartTest "Test mkfs.fat"
        # create the filesystem
        rlRun "mkfs.fat $DEV"

        # create testfile
        rlRun "mount $DEV $MNT"
        rlRun "touch $MNT/testfile"
        rlRun "checkFC $MNT/testfile dosfs_t"
        rlRun "umount $DEV"

        # test the context option
        rlRun "mount $DEV $MNT -t vfat -o 'context="unconfined_u:object_r:user_home_t:s0"'"
        rlRun "checkFC $MNT/testfile user_home_t"
        rlRun "umount $DEV"
    rlPhaseEnd

    rlPhaseStartCleanup
        rlRun "losetup -d $DEV"
        rlRun "semanage fcontext -d '$MNT/.*'"
        rlRun "rm -rf $MNT loop_file"
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd
