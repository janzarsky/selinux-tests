#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/libsemanage/Sanity/verify-options-in-semanage-conf
#   Description: Are the verify options in semanage.conf honored?
#   Author: Milos Malik <mmalik@redhat.com>
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   Copyright (c) 2016 Red Hat, Inc.
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

# Include Beaker environment
. /usr/bin/rhts-environment.sh || exit 1
. /usr/share/beakerlib/beakerlib.sh || exit 1

PACKAGE="libsemanage"
MODULE_NAME="empty"
SEMANAGE_CONF="/etc/selinux/semanage.conf"

rlJournalStart
	rlPhaseStartSetup
		rlAssertRpm ${PACKAGE}
		rlAssertRpm policycoreutils
		rlAssertRpm selinux-policy
		rlFileBackup ${SEMANAGE_CONF}
		rlRun "rpm -qf /usr/sbin/semanage"
		rlRun "grep -v -e '^#' -e '^$' ${SEMANAGE_CONF}"
		OUTPUT_FILE=`mktemp`
		
		rlRun "setenforce 1"
		rlRun "sestatus"
		rlRun "ls -l ${MODULE_NAME}.te"
		rlRun "make -f /usr/share/selinux/devel/Makefile"
		rlRun "ls -l ${MODULE_NAME}.pp"
    rlPhaseEnd

	rlLog "positive cases follow"
	# TODO: /bin/true could be replaced a script, which prints the supplied arguments into a file for further inspection

	rlPhaseStartTest "verify kernel"
		rlRun "semodule -r ${MODULE_NAME}" 0,1
		rlFileRestore
		rlRun "echo -en '[verify kernel]\npath = /bin/true\nargs = \$@\n[end]\n' >> ${SEMANAGE_CONF}"
		rlRun "semodule -i ${MODULE_NAME}.pp 2>&1 | tee ${OUTPUT_FILE}"
		rlAssertNotGrep "semodule.*failed" ${OUTPUT_FILE} -i
		rlRun "semodule -l | grep ${MODULE_NAME}"
		rlRun "semanage module -a ${MODULE_NAME}.pp 2>&1 | tee ${OUTPUT_FILE}"
		rlAssertNotGrep "could not commit semanage transaction|no such file or directory" ${OUTPUT_FILE} -Ei
		rlRun "semanage module -l | grep ${MODULE_NAME}"
	rlPhaseEnd

	rlPhaseStartTest "verify module"
		rlRun "semodule -r ${MODULE_NAME}" 0,1
		rlFileRestore
		rlRun "echo -en '[verify module]\npath = /bin/true\nargs = \$@\n[end]\n' >> ${SEMANAGE_CONF}"
		rlRun "semodule -i ${MODULE_NAME}.pp 2>&1 | tee ${OUTPUT_FILE}"
		rlAssertNotGrep "semodule.*failed" ${OUTPUT_FILE} -i
		rlRun "semodule -l | grep ${MODULE_NAME}"
		rlRun "semanage module -a ${MODULE_NAME}.pp 2>&1 | tee ${OUTPUT_FILE}"
		rlAssertNotGrep "could not commit semanage transaction|no such file or directory" ${OUTPUT_FILE} -Ei
		rlRun "semanage module -l | grep ${MODULE_NAME}"
	rlPhaseEnd

    if rlIsRHEL '<7.3' ; then # because "[verify linked]" was dropped
	rlPhaseStartTest "verify linked"
		rlRun "semodule -r ${MODULE_NAME}" 0,1
		rlFileRestore
		rlRun "echo -en '[verify linked]\npath = /bin/true\nargs = \$@\n[end]\n' >> ${SEMANAGE_CONF}"
		rlRun "semodule -i ${MODULE_NAME}.pp 2>&1 | tee ${OUTPUT_FILE}"
		rlAssertNotGrep "semodule.*failed" ${OUTPUT_FILE} -i
		rlRun "semodule -l | grep ${MODULE_NAME}"
		rlRun "semanage module -a ${MODULE_NAME}.pp 2>&1 | tee ${OUTPUT_FILE}"
		rlAssertNotGrep "could not commit semanage transaction|no such file or directory" ${OUTPUT_FILE} -Ei
		rlRun "semanage module -l | grep ${MODULE_NAME}"
	rlPhaseEnd
    fi

	rlLog "negative cases follow"
	# TODO: /bin/false could be replaced a script, which prints the supplied arguments into a file for further inspection

	rlPhaseStartTest "verify kernel"
		rlRun "semodule -r ${MODULE_NAME}" 0,1
		rlFileRestore
		rlRun "echo -en '[verify kernel]\npath = /bin/false\nargs = \$@\n[end]\n' >> ${SEMANAGE_CONF}"
		rlRun "semodule -i ${MODULE_NAME}.pp 2>&1 | tee ${OUTPUT_FILE}"
		rlAssertGrep "semodule.*failed" ${OUTPUT_FILE} -i
		rlRun "semodule -l | grep ${MODULE_NAME}" 1
		rlRun "semanage module -a ${MODULE_NAME}.pp 2>&1 | tee ${OUTPUT_FILE}"
		rlAssertGrep "could not commit semanage transaction|no such file or directory" ${OUTPUT_FILE} -Ei
		rlRun "semanage module -l | grep ${MODULE_NAME}" 1
	rlPhaseEnd

	rlPhaseStartTest "verify module"
		rlRun "semodule -r ${MODULE_NAME}" 0,1
		rlFileRestore
		rlRun "echo -en '[verify module]\npath = /bin/false\nargs = \$@\n[end]\n' >> ${SEMANAGE_CONF}"
		rlRun "semodule -i ${MODULE_NAME}.pp 2>&1 | tee ${OUTPUT_FILE}"
		rlAssertGrep "semodule.*failed" ${OUTPUT_FILE} -i
		rlRun "semodule -l | grep ${MODULE_NAME}" 1
		rlRun "semanage module -a ${MODULE_NAME}.pp 2>&1 | tee ${OUTPUT_FILE}"
		rlAssertGrep "could not commit semanage transaction|no such file or directory" ${OUTPUT_FILE} -Ei
		rlRun "semanage module -l | grep ${MODULE_NAME}" 1
	rlPhaseEnd

    if rlIsRHEL '<7.3' ; then # because "[verify linked]" was dropped
	rlPhaseStartTest "verify linked"
		rlRun "semodule -r ${MODULE_NAME}" 0,1
		rlFileRestore
		rlRun "echo -en '[verify linked]\npath = /bin/false\nargs = \$@\n[end]\n' >> ${SEMANAGE_CONF}"
		rlRun "semodule -i ${MODULE_NAME}.pp 2>&1 | tee ${OUTPUT_FILE}"
		rlAssertGrep "semodule.*failed" ${OUTPUT_FILE} -i
		rlRun "semodule -l | grep ${MODULE_NAME}" 1
		rlRun "semanage module -a ${MODULE_NAME}.pp 2>&1 | tee ${OUTPUT_FILE}"
		rlAssertGrep "could not commit semanage transaction|no such file or directory" ${OUTPUT_FILE} -Ei
		rlRun "semanage module -l | grep ${MODULE_NAME}" 1
	rlPhaseEnd
    fi

	rlPhaseStartCleanup
		rlRun "rm -f ${MODULE_NAME}.pp ${OUTPUT_FILE}"
		rlFileRestore	
	rlPhaseEnd
rlJournalPrintText
rlJournalEnd

