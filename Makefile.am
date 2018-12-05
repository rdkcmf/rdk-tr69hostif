##########################################################################
# If not stated otherwise in this file or this component's Licenses.txt
# file the following copyright and licenses apply:
#
# Copyright 2016 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################
SUBDIRS = $(SUBDIRS_MOCA) $(SUBDIRS_WIFI) src src/tr69BusAgent
DIST_SUBDIRS = $(SUBDIRS_MOCA) $(SUBDIRS_WIFI) src src/tr69BusAgent

## IPK Generation Support
tr69hostif_IPK_OUTDIR = $(abs_top_builddir)/ipk
tr69hostif_IPK_STAGING_DIR=$(abs_top_builddir)/staging_dir
tr69hostif_IPK_TMP_DIR = $(tr69hostif_IPK_STAGING_DIR)/ipk
tr69hostif_CONTROL_FILES = control postinst prerm
tr69hostif_IPK_FILES  = debian-binary control.tar.gz data.tar.gz
tr69hostif_IPK_OUTPUT = tr69hostif.ipk

package_ipk:
	@echo "Generating IPK Package"
	## Install files
	$(MAKE) prefix=$(tr69hostif_IPK_STAGING_DIR) install
	## Create temp paths
	$(mkdir_p) $(tr69hostif_IPK_TMP_DIR)/usr/bin
	$(mkdir_p) $(tr69hostif_IPK_TMP_DIR)/etc
	$(mkdir_p) $(tr69hostif_IPK_TMP_DIR)/lib/systemd/system
	## Install files to temp path
	install -m 0755 $(tr69hostif_IPK_STAGING_DIR)/bin/tr69hostif $(tr69hostif_IPK_TMP_DIR)/usr/bin/
	install -m 0644 $(abs_top_builddir)/conf/mgrlist.conf $(tr69hostif_IPK_TMP_DIR)/etc/
	install -m 0644 $(abs_top_builddir)/tr69hostif.service $(tr69hostif_IPK_TMP_DIR)/lib/systemd/system
	## Create package files
	tar -czvf $(tr69hostif_IPK_OUTDIR)/data.tar.gz -C $(tr69hostif_IPK_TMP_DIR) usr etc lib
	tar -czvf $(tr69hostif_IPK_OUTDIR)/control.tar.gz -C $(tr69hostif_IPK_OUTDIR) $(tr69hostif_CONTROL_FILES)
	pushd $(tr69hostif_IPK_OUTDIR) && ar cr $(tr69hostif_IPK_OUTPUT) $(tr69hostif_IPK_FILES); popd
	rm -rf $(tr69hostif_IPK_TMP_DIR)
	rm -f $(tr69hostif_IPK_OUTDIR)/*.tar.gz
clean_package_ipk:
	rm -rf $(tr69hostif_IPK_STAGING_DIR)
	rm -f $(tr69hostif_IPK_OUTDIR)/*.tar.gz
	rm -f $(tr69hostif_IPK_OUTDIR)/*.ipk
