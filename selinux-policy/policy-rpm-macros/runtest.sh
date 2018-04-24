#!/bin/bash
# vim: dict+=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/selinux-policy/Sanity/policy-rpm-macros
#   Description: Are necessary RPM macros defined for various policy actions?
#   Author: Milos Malik <mmalik@redhat.com>
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   Copyright (c) 2017 Red Hat, Inc.
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

PACKAGE="selinux-policy"
MACRO_FILE="/usr/lib/rpm/macros.d/macros.selinux-policy"

rlJournalStart
    rlPhaseStartSetup
        rlAssertRpm ${PACKAGE}
        rlRun "ls -l ${MACRO_FILE}"
    rlPhaseEnd

    rlPhaseStartTest "Are the macros defined?"
        rlRun "grep ^%selinux_modules_install ${MACRO_FILE}"
        rlRun "grep ^%selinux_modules_uninstall ${MACRO_FILE}"
        rlRun "grep ^%selinux_relabel_pre ${MACRO_FILE}"
        rlRun "grep ^%selinux_relabel_post ${MACRO_FILE}"
    rlPhaseEnd

    rlPhaseStartTest "bz#1415694"
        rlRun "grep selinuxenabled ${MACRO_FILE}"
        rlRun "grep selinuxenabled ${MACRO_FILE} | grep _sbindir"
        rlRun "grep selinuxenabled ${MACRO_FILE} | grep /usr/sbin/" 1
    rlPhaseEnd

    rlPhaseStartTest "download and prepare the upstream test"
        rlRun "git clone https://github.com/fedora-selinux/selinux-policy-macros.git"
        rlRun "rm -f selinux-policy-macros/macros.selinux-policy"
        rlRun "cp /usr/lib/rpm/macros.d/macros.selinux-policy selinux-policy-macros/"
    rlPhaseEnd

    pushd selinux-policy-macros
    # running an upstream test written by Petr Lautrbach, thanks
    ./runtest.sh
    popd

    rlPhaseStartCleanup
        rlRun "rm -rf selinux-policy-macros"
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd

