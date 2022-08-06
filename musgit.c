#include <stdio.h>
#include <stdarg.h>

#include <c-stache.h>
#include <git2.h>

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
	CStacheTemplate *tpl;
	int status;

	status = c_stache_load_template(&stacheEngine, tplname, &tpl);
	if (status != C_STACHE_OK)
		die("cannot load template: %s", c_stache_strerror(status));
}

int
main()
{
	c_stache_start_engine(&stacheEngine, c_stache_read_file);

	make_log("log.html.mustache");

	c_stache_shutdown_engine(&stacheEngine);
	return 0;
}

