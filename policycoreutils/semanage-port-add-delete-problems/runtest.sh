#!/bin/bash
# vim: dict=/usr/share/rhts-library/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/policycoreutils/Regression/semanage-port-add-delete-problems
#   Description: semanage accepts invalid port numbers and then cannot delete them
#   Author: Milos Malik <mmalik@redhat.com>
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   Copyright (c) 2009 Red Hat, Inc. All rights reserved.
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

. /usr/bin/rhts-environment.sh || exit 1
. /usr/share/beakerlib/beakerlib.sh || exit 1

PACKAGE="policycoreutils"
PORT_NAME="ldap_port_t"
BAD_PORT_NUMBER="123456"
GOOD_PORT_NUMBER="1389"

rlJournalStart
    rlPhaseStartSetup
        rlAssertRpm ${PACKAGE}
        rlRun "rpm -qf /usr/sbin/semanage"
        rlRun "rpm -qf /usr/bin/seinfo"
        OUTPUT_FILE=`mktemp`
        rlRun "setenforce 1"
        rlRun "sestatus"
    rlPhaseEnd

    rlPhaseStartTest
        rlRun "semanage port -l | grep ${PORT_NAME}"

        rlRun "semanage port -l | grep ${PORT_NAME} | tee -a ${OUTPUT_FILE}"
        rlRun "semanage port -a -t ${PORT_NAME} -p tcp ${BAD_PORT_NUMBER}" 1
        rlRun "semanage port -l | grep ${PORT_NAME} | tee -a ${OUTPUT_FILE}"
        rlRun "semanage port -d -t ${PORT_NAME} -p tcp ${BAD_PORT_NUMBER}" 1
        rlRun "semanage port -l | grep ${PORT_NAME} | tee -a ${OUTPUT_FILE}"
        #rlRun "sort ${OUTPUT_FILE} | uniq | wc -l | grep '^2$'"

        rlRun "semanage port -l | grep ${PORT_NAME} | grep ${GOOD_PORT_NUMBER}" 1
        rlRun "semanage port -a -t ${PORT_NAME} -p tcp ${GOOD_PORT_NUMBER}"
        rlRun "semanage port -l | grep ${PORT_NAME} | grep ${GOOD_PORT_NUMBER}"
        rlRun "semanage port -d -t ${PORT_NAME} -p tcp ${GOOD_PORT_NUMBER}"
        rlRun "semanage port -l | grep ${PORT_NAME} | grep ${GOOD_PORT_NUMBER}" 1
    rlPhaseEnd

    rlPhaseStartTest
        rlRun "semanage port -a -t syslogd_port_t -p tcp 60514-60516 2>&1 | grep -i traceback" 1
        rlRun "semanage port -l | grep syslogd_port_t"
        rlRun "semanage port -d -t syslogd_port_t -p tcp 60514-60516 2>&1 | grep -i traceback" 1
    rlPhaseEnd

    if rlIsRHEL ; then
    rlPhaseStartTest
        rlRun "ps -efZ | grep -v grep | grep \"auditd_t.*auditd\""
        if rlIsRHEL 5 6; then
            PORT_TYPE="syslogd_port_t"
        else
            PORT_TYPE="commplex_link_port_t"
        fi

        # adding a port number to a type
        START_DATE_TIME=`date "+%m/%d/%Y %T"`
        sleep 1
        rlRun "semanage port -a -p tcp -t $PORT_TYPE 5005"
        sleep 2

        # Check for user_avc
        rlRun "ausearch -m user_avc -ts ${START_DATE_TIME} > ${OUTPUT_FILE}" 0,1
        LINE_COUNT=`wc -l < ${OUTPUT_FILE}`
        rlRun "cat ${OUTPUT_FILE}"
        rlAssert0 "number of lines in ${OUTPUT_FILE} should be 0" ${LINE_COUNT}

        # deleting a port number from a type
        START_DATE_TIME=`date "+%m/%d/%Y %T"`
        sleep 1
        rlRun "semanage port -d -p tcp -t $PORT_TYPE 5005"
        sleep 2

        # Check for user_avc
        rlRun "ausearch -m user_avc -ts ${START_DATE_TIME} > ${OUTPUT_FILE}" 0,1
        LINE_COUNT=`wc -l < ${OUTPUT_FILE}`
        rlRun "cat ${OUTPUT_FILE}"
        rlAssert0 "number of lines in ${OUTPUT_FILE} should be 0" ${LINE_COUNT}
    rlPhaseEnd
    fi

    if ! rlIsRHEL 5 ; then
    rlPhaseStartTest
        rlRun "seinfo --portcon | grep :hi_reserved_port_t:"
        rlRun "seinfo --portcon | grep :reserved_port_t:"
        rlRun "semanage port -l | grep ^hi_reserved_port_t"
        rlRun "semanage port -l | grep ^reserved_port_t"
        if ! rlIsRHEL 6 ; then
            rlRun "seinfo --portcon | grep :unreserved_port_t:"
            rlRun "semanage port -l | grep ^unreserved_port_t"
        fi
    rlPhaseEnd
    fi

    rlPhaseStartTest "manipulation with hard-wired ports"
        rlRun "semanage port -l | grep 'smtp_port_t.*tcp.*25'"
        rlRun "semanage port -a -t smtp_port_t -p tcp 25 2>&1 | tee ${OUTPUT_FILE}"
        rlAssertGrep "port .* already defined" ${OUTPUT_FILE} -i
        rlRun "semanage port -a -t smtp_port_t -p tcp 25 2>&1 | tee ${OUTPUT_FILE}"
        rlAssertGrep "port .* already defined" ${OUTPUT_FILE} -i
        rlRun "semanage port -l | grep 'smtp_port_t.*tcp.*25'"
        rlRun "semanage port -d -t smtp_port_t -p tcp 25 2>&1 | tee ${OUTPUT_FILE}"
        rlAssertGrep "port .* is defined in policy.*cannot be deleted" ${OUTPUT_FILE} -i
        rlRun "semanage port -d -t smtp_port_t -p tcp 25 2>&1 | tee ${OUTPUT_FILE}"
        rlAssertGrep "port .* is defined in policy.*cannot be deleted" ${OUTPUT_FILE} -i
        rlRun "semanage port -l | grep 'smtp_port_t.*tcp.*25'"
    rlPhaseEnd

    rlPhaseStartCleanup
        rm -f ${OUTPUT_FILE}
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd

