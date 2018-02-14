#!/bin/bash
# vim: dict=/usr/share/beakerlib/dictionary.vim cpt=.,w,b,u,t,i,k
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   runtest.sh of /CoreOS/policycoreutils/Sanity/sepolicy-generate
#   Description: sepolicy generate sanity test
#   Author: Michal Trunecka <mtruneck@redhat.com>
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#   Copyright (c) 2013 Red Hat, Inc. All rights reserved.
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

PACKAGE="policycoreutils"

rlJournalStart
    rlPhaseStartSetup
        rlRun "rlCheckRequirements ${PACKAGES[*]}" || rlDie "cannot continue"
        rlRun "TmpDir=\$(mktemp -d)" 0 "Creating tmp directory"
        rlRun "pushd $TmpDir"
    rlPhaseEnd

    rlPhaseStartTest
        rlRun "mkdir mypolicy"
        rlRun "sepolicy generate --customize -p mypolicy -n testpolicy -d httpd_sys_script_t -w /home"
        rlRun "grep 'manage_dirs_pattern(httpd_sys_script_t' mypolicy/testpolicy.te"
        rlRun "rm -rf mypolicy"
    rlPhaseEnd

    rlPhaseStartTest
        rlRun "mkdir mypolicy"
        rlRun "touch /usr/bin/testpolicy"
      for VARIANT in " -n testpolicy --admin_user -r webadm_r" \
                     " --application /usr/bin/testpolicy " \
                     " -n testpolicy --confined_admin -a firewalld " \
                     " -n testpolicy --confined_admin " \
                     " -n testpolicy --customize -d httpd_t -a firewalld " \
                     " -n testpolicy --customize -d httpd_t" \
                     " --dbus /usr/bin/testpolicy " \
                     " -n testpolicy --desktop_user " \
                     " --inetd /usr/bin/testpolicy " \
                     " --init /usr/bin/testpolicy " \
                     " -n testpolicy --newtype -t newtype_var_log_t " \
                     " -n testpolicy --newtype -t newtype_unit_file_t " \
                     " -n testpolicy --newtype -t newtype_var_run_t " \
                     " -n testpolicy --newtype -t newtype_var_cache_t " \
                     " -n testpolicy --newtype -t newtype_tmp_t " \
                     " -n testpolicy --newtype -t newtype_port_t " \
                     " -n testpolicy --newtype -t newtype_var_spool_t " \
                     " -n testpolicy --newtype -t newtype_var_lib_t " \
                     " -n testpolicy --sandbox " \
                     " -n testpolicy --term_user " \
                     " -n testpolicy --x_user "
#                     " --cgi /usr/bin/testpolicy "
        do
            rlRun "sepolicy generate -p mypolicy $VARIANT"
            rlRun "cat mypolicy/testpolicy.te"
            rlRun "cat mypolicy/testpolicy.if"
            rlRun "cat mypolicy/testpolicy.fc"
          if echo "$VARIANT" | grep -q newtype; then
            rlAssertNotExists "mypolicy/testpolicy.sh"
            rlAssertNotExists "mypolicy/testpolicy.spec"
          else
            rlRun "mypolicy/testpolicy.sh"
            rlRun "semodule -l | grep  testpolicy"
            rlRun "semanage user -d testpolicy_u" 0-255
            rlRun "semodule -r testpolicy"
          fi

            rlRun "rm -rf mypolicy/*"
            rlRun "sleep 1"

          if ! echo "$VARIANT" | grep -q newtype; then
            rlRun "sepolicy generate -p mypolicy -w /home  $VARIANT"
            rlRun "cat mypolicy/testpolicy.te"
            rlRun "cat mypolicy/testpolicy.if"
            rlRun "cat mypolicy/testpolicy.fc"

            rlRun "mypolicy/testpolicy.sh"
            rlRun "semodule -l | grep  testpolicy"
            rlRun "semanage user -d testpolicy_u" 0-255
            rlRun "semodule -r testpolicy"

            rlRun "rm -rf mypolicy/*"
            rlRun "sleep 1"
          fi
        done
        rlRun "rm -rf mypolicy"
    rlPhaseEnd

    rlPhaseStartCleanup
        rlRun "popd"
        rlRun "rm -r $TmpDir" 0 "Removing tmp directory"
    rlPhaseEnd
rlJournalPrintText
rlJournalEnd
