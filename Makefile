MIX = mix
CFLAGS += -g -O3 -ansi -Wall -Wextra -Wno-unused-parameter

ERLANG_PATH = $(shell erl -eval 'io:format("~s", [lists:concat([code:root_dir(), "/erts-", erlang:system_info(version), "/include"])])' -s init stop -noshell)
CFLAGS += -I$(ERLANG_PATH)

# adjust these as your library desires
CFLAGS += -I/usr/local/include -I/usr/include -L/usr/local/lib -L/usr/lib
CFLAGS += -std=gnu99 -Wno-unused-function
CFLAGS += `pkg-config --cflags libxslt` `pkg-config --libs libxslt`

ifneq ($(OS),Windows_NT)
	CFLAGS += -fPIC

	ifeq ($(shell uname),Darwin)
		LDFLAGS += -dynamiclib -undefined dynamic_lookup
	endif
endif

.PHONY: all libxslt clean

all: libxslt

libxslt:
	@$(MIX) compile

priv/xslt_ex.so: src/xslt_ex.c
	@$(CC) $(CFLAGS) -shared $(LDFLAGS) -o $@ src/xslt_ex.c

clean:
	@$(MIX) clean
