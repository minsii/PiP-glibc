# Makefile for maintaining Fedora branches and creating Fedora source RPMs.

glibc.spec: # The default target.

.PHONY: follow merge merge-base tag force-tag archive finish_archive srpm rpm

releases-url := http://ftp.gnu.org/gnu/glibc/
tag-prefix := fedora/
branch-name := fedora

GIT ?= git
git = $(shell $(GIT) $1 $2)
gitconf = $(call git,config,--get $1)
branchname = $(patsubst refs/heads/%,%,$1)

my-branch := $(call branchname,$(call git,symbolic-ref,HEAD))
upstream-remote := $(call gitconf,branch.$(my-branch).remote)
upstream-branch := $(call branchname,$(call gitconf,branch.$(my-branch).follow))

ports-repo := $(call gitconf,glibc.ports-repo)

ifndef ports-repo
$(error \
You need a glibc-ports repository! \
Get one and do: "git config glibc.ports-repo GLIBC_PORTS.git" \
)
endif

dep-my-branch := $(firstword $(wildcard ../.git/refs/heads/$(my-branch) \
	      		    	       	../.git/packed-refs))
dep-upstream-branch := $(firstword $(wildcard \
	../.git/refs/remotes/$(upstream-remote)/$(upstream-branch) \
	../.git/packed-refs))

# Use 'make follow branch=release/3.14/master' to switch this checkout
# to a new upstream branch to merge from.
follow:
ifeq (,$(branch))
	@echo "Use '$(MAKE) follow branch=NAME'"; exit 2
else
	$(GIT) rev-parse --verify $(upstream-remote)/$(branch)
	$(GIT) config branch.$(my-branch).follow $(branch)
	@$(GIT) branch -v | grep "^. $(subst .,\\.,$(my-branch)) "
endif

# Use this to merge upstream changes into this branch.
# It will fail if conflict resolution is required.
# Then you follow up with editting, 'git add FILE...', and git commit.
merge:
	$(GIT) pull
	$(GIT) merge $(upstream-remote)/$(upstream-branch)

describe-merge = describe --match 'glibc-*'

merge-base-id := $(call git,merge-base,\
	      	       	HEAD $(upstream-remote)/$(upstream-branch))
merge-base-name := $(call git,$(describe-merge) $(merge-base-id))

merge-base:
	@echo $(merge-base-id) $(merge-base-name)

snapshot-name := $(patsubst glibc-%,%,$(merge-base-name))

tar-name = $(merge-base-name)

upstream-pristine = \
	$(GIT) $(describe-merge) --exact-match > /dev/null 2>&1 $(merge-base-id)

GIT-ports = $(GIT) --git-dir=$(ports-repo)
git-ports = $(shell $(GIT-ports) $1 $2)

ports-pristine = $(GIT-ports) $(describe-merge) --exact-match > /dev/null 2>&1 \
						$(upstream-branch)
ports-base-name := $(call git-ports,$(describe-merge) $(upstream-branch))

ports-tar-name := $(patsubst glibc-%,glibc-ports-%,$(ports-base-name))

glibc.spec: glibc.spec.in $(dep-my-branch)
	@rm -f $@.new
	echo '%define glibcsrcdir $(tar-name)' > $@.new
	if $(upstream-pristine) && $(ports-pristine); then \
	  echo '%define glibc_release_url $(releases-url)' >> $@.new; \
	else : ; fi; \
	$(GIT) show $(my-branch):version.h \
	| sed -n '/VERSION/s/^.*"\([^"]*\)"$$/%define glibcversion \1/p' \
	>> $@.new
	echo '%define glibcportsdir $(ports-tar-name)' >> $@.new
	echo '### $< follows:' >> $@.new
	cat $< >> $@.new
	mv -f $@.new $@

ifeq (,$(wildcard glibc.spec))
Makefile: glibc.spec ;
else

spec-nvr := $(shell rpm -q --qf '%{NAME}-%{VERSION}-%{RELEASE}\n' \
			--specfile glibc.spec 2> /dev/null | sed 1q)
spec-tag = $(spec-nvr)

tag-spec = -a -m'$(spec-nvr)' $(tag-prefix)$(spec-tag)

tag: glibc.spec
	$(GIT) tag $(tag-spec)

force-tag: glibc.spec
	$(GIT) tag -f $(tag-spec)

endif

# Omit these files from the patch and put them in a tar file.
outside-patch = releng/ c_stubs/ rtkaio/ \
		localedata/charmaps/GB18030 iconvdata/gb18030.c

glibc-$(branch-name).patch: glibc.spec Makefile $(dep-upstream-branch)
	@echo "Creating $@ from `$(git-describe) $(my-branch)`..."
	@$(GIT) diff -a --no-renames $(merge-base-id)..$(my-branch) \
	| awk '$$1 == "diff" && $$2 == "--git" { file = $$3 } \
	       $$1 == "---" && $$2 == "/dev/null" { $$2 = file } \
	       { print }' \
	| filterdiff --remove-timestamps --clean --strip=1 \
		     -x '*/.gitignore' \
		     $(patsubst %,-x '*/%',$(patsubst %/,%/*,$(outside-patch)))\
		     --addoldprefix='$(merge-base-name)/' \
		     --addnewprefix='$(spec-nvr)/' \
	> patch.tmp
	@mv -f patch.tmp $@

git-describe = $(GIT) describe --long --always
ports-git-describe = $(GIT-ports) describe --long --always

define git-tar
echo "Creating $@ from `$(git-describe) $1`..."; \
(cd ..; $(GIT) archive --format=tar --prefix='$(tar-name)/' $1 $2) \
| xz -9 > $@.new && \
mv -f $@.new $@
endef

define ports-git-tar
echo "Creating $@ from `$(ports-git-describe) $1`..."; \
(cd ..; $(GIT-ports) archive --format=tar --prefix='$(ports-tar-name)/' $1 $2) \
| xz -9 > $@.new && \
mv -f $@.new $@
endef

$(tar-name)-$(branch-name).tar.xz: glibc.spec Makefile
	@$(call git-tar,$(my-branch),$(outside-patch))

$(tar-name).tar.xz: $(dep-upstream-branch) Makefile
	@if $(upstream-pristine); then \
	   echo 'Fetching from $(releases-url)...'; \
	   curl -C - -O $(releases-url)/$@; \
	 else \
	   $(call git-tar,$(merge-base-id)); \
	 fi

$(ports-tar-name).tar.xz: Makefile
	@if $(ports-pristine); then \
	   echo 'Fetching from $(releases-url)...'; \
	   curl -C - -O $(releases-url)/$@; \
	 else \
	   $(call ports-git-tar,$(upstream-branch)); \
	 fi

archives = $(tar-name).tar.xz \
	   $(tar-name)-$(branch-name).tar.xz \
	   glibc-$(branch-name).patch \
	   $(ports-tar-name).tar.xz

finish_archive: $(archives)

archive: glibc.spec
	$(MAKE) tag finish_archive

rpm srpm: $(spec-nvr).src.rpm
$(spec-nvr).src.rpm: glibc.spec $(archives)
	rpmbuild --define "_topdir ." \
		 --define "_sourcedir %{_topdir}" \
		 --define "_specdir %{_topdir}" \
		 --define "_srcrpmdir %{_topdir}" \
		 --define "_rpmdir %{_topdir}" \
		 --define "_builddir %{_topdir}" \
		 --nodeps -bs $<
