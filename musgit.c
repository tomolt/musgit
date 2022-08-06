#include <stdio.h>
#include <stdarg.h>

#include <c-stache.h>
#include <git2.h>

static git_repository *gitRepo;
static CStacheEngine stacheEngine;

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

static void
make_log(const char *tplname)
{
	git_revwalk *walker;
	git_oid oid;
	git_commit *commit;
	CStacheTemplate *tpl;
	int status;

	//status = c_stache_load_template(&stacheEngine, tplname, &tpl);
	//if (status != C_STACHE_OK)
	//	die("%s", c_stache_strerror(status));

	git_revwalk_new(&walker, gitRepo);
	git_revwalk_push_head(walker);

	while (!git_revwalk_next(&oid, walker)) {
		git_commit_lookup(&commit, gitRepo, &oid);

		printf("%s\n--\n", git_commit_summary(commit));

		git_commit_free(commit);
	}

	git_revwalk_free(walker);
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

	make_log("log.html.mustache");

	git_repository_free(gitRepo);
	c_stache_shutdown_engine(&stacheEngine);
	git_libgit2_shutdown();
	return 0;
}

