#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <c-stache.h>
#include <git2.h>

static git_repository *gitRepo;
static CStacheEngine stacheEngine;

enum {
	LOG_ROOT,
	LOG_COMMITS,
};

struct log_iter {
	int state;
	git_revwalk *revwalk;
	git_commit  *commit;
};

static void
die(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
	fputc('\n', stderr);
	exit(1);
}

static int
log_enter(void *userptr, const char *section)
{
	git_oid oid;
	struct log_iter *iter = userptr;
	switch (iter->state) {
	case LOG_ROOT:
		if (!strcmp(section, "commits")) {
			iter->state = LOG_COMMITS;
			git_revwalk_new(&iter->revwalk, gitRepo);
			git_revwalk_push_head(iter->revwalk);
			if (!git_revwalk_next(&oid, iter->revwalk)) {
				git_commit_lookup(&iter->commit, gitRepo, &oid);
				return 1;
			} else {
				git_revwalk_free(iter->revwalk);
				return 0;
			}
		}
		/* fallthrough */

	default:
		return 0;
	}
}

static int
log_next(void *userptr)
{
	git_oid oid;
	struct log_iter *iter = userptr;
	switch (iter->state) {
	case LOG_COMMITS:
		git_commit_free(iter->commit);
		if (!git_revwalk_next(&oid, iter->revwalk)) {
			git_commit_lookup(&iter->commit, gitRepo, &oid);
			return 1;
		} else {
			git_revwalk_free(iter->revwalk);
			return 0;
		}

	default:
		return 0;
	}
}

static int
log_empty(void *userptr, const char *section)
{
	struct log_iter *iter = userptr;
	(void) iter;
	(void) section;
	/* TODO */
	return 0;
}

static const char *
log_subst(void *userptr, const char *key)
{
	struct log_iter *iter = userptr;
	switch (iter->state) {
	case LOG_COMMITS:
		if (!strcmp(key, "summary")) {
			return git_commit_summary(iter->commit);
		}
		/* fallthrough */
	
	default:
		return NULL;
	}
}

static void
make_log(const char *tplfile)
{
	CStacheTemplate *tpl;
	int status;

	status = c_stache_load_template(&stacheEngine, tplfile, &tpl);
	if (status != C_STACHE_OK)
		die("%s", c_stache_strerror(status));

	struct log_iter iter = { 0 };

	CStacheModel model = {
		log_enter,
		log_next,
		log_empty,
		log_subst,
		&iter
	};

	CStacheSink sink = {
		c_stache_escape_xml,
		c_stache_write_file,
		stdout
	};

	c_stache_render(tpl, &model, &sink);

	c_stache_drop_template(&stacheEngine, tpl);
}

int
main()
{
	const git_error *error;
	int status;

	git_libgit2_init();
	c_stache_start_engine(&stacheEngine, c_stache_read_file);

	status = git_repository_open(&gitRepo, ".");
	if (status < 0) {
		error = git_error_last();
		die("cannot open repo: %s", error->message);
	}

	make_log("template/log.html.mustache");

	git_repository_free(gitRepo);
	c_stache_shutdown_engine(&stacheEngine);
	git_libgit2_shutdown();
	return 0;
}

