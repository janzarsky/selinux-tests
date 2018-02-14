#!/bin/bash
# vim: dict=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/policycoreutils/Sanity/setsebool
#   Description: does setsebool work correctly ?
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
USER_NAME="user${RANDOM}"
USER_SECRET="s3kr3t${RANDOM}"
BOOLEAN="ftpd_connect_db"
if rlIsRHEL 5 6 ; then
    SELINUX_FS_MOUNT="/selinux"
else # RHEL-7 and above
    SELINUX_FS_MOUNT="/sys/fs/selinux"
fi

rlJournalStart
    rlPhaseStartSetup
        rlAssertRpm ${PACKAGE}
        OUTPUT_FILE=`mktemp`
        chcon -t tmp_t ${OUTPUT_FILE}

        rlRun "useradd ${USER_NAME}"
        rlRun "echo ${USER_SECRET} | passwd --stdin ${USER_NAME}"
    rlPhaseEnd

    rlPhaseStartTest
        for OPTION in "" "-P" ; do
            for OPERATOR in " " "=" ; do
                for VALUE in 0 1 false true off on ; do
                    rlRun "setsebool ${OPTION} ${BOOLEAN}${OPERATOR}${VALUE} | grep -i -e illegal -e usage -e invalid" 1
                    if [ ${VALUE} == "0" -o ${VALUE} == "false" ] ; then
                        SHOWN_VALUE="off"
                    elif [ ${VALUE} == "1" -o ${VALUE} == "true" ] ; then
                        SHOWN_VALUE="on"
                    else
                        SHOWN_VALUE=${VALUE}
                    fi
                    rlRun "getsebool -a | grep \"^${BOOLEAN}.*${SHOWN_VALUE}\""
                done
            done
        done
    rlPhaseEnd

    rlPhaseStartTest
        rlRun "setsebool" 1
        rlRun "setsebool xyz=1 2>&1 | tee /dev/stderr | grep -i -e \"invalid boolean\" -e \"not found\" -e \"not defined\""
        rlRun "setsebool xyz=-1 2>&1 | tee /dev/stderr | grep -i \"illegal value\""
        rlRun "setsebool xyz=2 2>&1 | tee /dev/stderr | grep -i \"illegal value\""
        if ! rlIsRHEL 5 6 ; then
            rlRun "setsebool -N 2>&1 | tee /dev/stderr | grep -i \"boolean.*required\""
            rlRun "setsebool -P 2>&1 | tee /dev/stderr | grep -i \"boolean.*required\""
        fi
        rlRun "setsebool -P xyz=1 2>&1 | tee /dev/stderr | grep -i -e \"invalid boolean\" -e \"not found\" -e \"not defined\""
        rlRun "setsebool -P xyz=-1 2>&1 | tee /dev/stderr | grep -i \"illegal value\""
        rlRun "setsebool -P xyz=2 2>&1 | tee /dev/stderr | grep -i \"illegal value\""
    rlPhaseEnd

    if ! rlIsRHEL 5 6 ; then
    rlPhaseStartTest
        rlRun "su -l -c '/usr/sbin/setsebool allow_ypbind 0' ${USER_NAME} 2>&1 | tee ${OUTPUT_FILE}"
        rlAssertGrep "try as root" ${OUTPUT_FILE} -i
        rlRun "su -l -c '/usr/sbin/setsebool allow_ypbind 1' ${USER_NAME} 2>&1 | tee ${OUTPUT_FILE}"
        rlAssertGrep "try as root" ${OUTPUT_FILE} -i
        rlRun "su -l -c '/usr/sbin/setsebool -P allow_ypbind 0' ${USER_NAME} 2>&1 | tee ${OUTPUT_FILE}"
        rlAssertGrep "try as root" ${OUTPUT_FILE} -i
        rlRun "su -l -c '/usr/sbin/setsebool -P allow_ypbind 1' ${USER_NAME} 2>&1 | tee ${OUTPUT_FILE}"
        rlAssertGrep "try as root" ${OUTPUT_FILE} -i
    rlPhaseEnd

    rlPhaseStartTest
        for OPTION in "" "-P" ; do
            rlRun "getsebool allow_ypbind | grep nis_enabled"
            rlRun "setsebool ${OPTION} allow_ypbind on"
            rlRun "getsebool allow_ypbind | grep \"nis_enabled.*on\""
            rlRun "setsebool ${OPTION} allow_ypbind off"
            rlRun "getsebool allow_ypbind | grep \"nis_enabled.*off\""
        done
    rlPhaseEnd

    rlPhaseStartTest
        # https://fedoraproject.org/wiki/Features/SELinuxBooleansRename
        for LINE in `cat /etc/selinux/*/booleans.subs_dist | sort | uniq | tr -s ' ' | tr ' ' ':'` ; do
            OLD_BOOLEAN_NAME=`echo ${LINE} | cut -d : -f 1`
            NEW_BOOLEAN_NAME=`echo ${LINE} | cut -d : -f 2`
            rlRun "getsebool ${OLD_BOOLEAN_NAME} 2>&1 | tee ${OUTPUT_FILE}"
            rlRun "getsebool ${NEW_BOOLEAN_NAME} 2>&1 | tee -a ${OUTPUT_FILE}"
            rlRun "uniq -c ${OUTPUT_FILE} | grep '2 '"
        done
    rlPhaseEnd
    fi

    rlPhaseStartTest "audit messages"
        START_DATE_TIME=`date "+%m/%d/%Y %T"`
        sleep 1
        rlRun "setsebool ${BOOLEAN} on"
        rlRun "setsebool ${BOOLEAN} off"
        rlRun "setsebool ${BOOLEAN} on"
        sleep 1
        rlRun "ausearch -m MAC_CONFIG_CHANGE -i -ts ${START_DATE_TIME} | grep \"type=MAC_CONFIG_CHANGE.*bool=${BOOLEAN} val=1 old_val=0\""
        rlRun "ausearch -m MAC_CONFIG_CHANGE -i -ts ${START_DATE_TIME} | grep \"type=MAC_CONFIG_CHANGE.*bool=${BOOLEAN} val=0 old_val=1\""
        if rlIsRHEL ; then
            rlRun "ausearch -m MAC_CONFIG_CHANGE -i -ts ${START_DATE_TIME} | grep \"type=SYSCALL.*comm=setsebool\""
        fi
    rlPhaseEnd

    rlPhaseStartTest "extreme cases"
        # pretend that no booleans are defined
        rlRun "mkdir ./booleans"
        rlRun "mount --bind ./booleans ${SELINUX_FS_MOUNT}/booleans"
        rlRun "setsebool ${BOOLEAN} on 2>&1 | tee ${OUTPUT_FILE}"
        rlAssertGrep "could not change active booleans" ${OUTPUT_FILE} -i
        rlRun "setsebool ${BOOLEAN} off 2>&1 | tee ${OUTPUT_FILE}"
        rlAssertGrep "could not change active booleans" ${OUTPUT_FILE} -i
        rlRun "umount ${SELINUX_FS_MOUNT}/booleans"
        rlRun "rmdir ./booleans"
    rlPhaseEnd

    rlPhaseStartCleanup
        rlRun "userdel -rf ${USER_NAME}"
        rm -f ${OUTPUT_FILE}
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd

