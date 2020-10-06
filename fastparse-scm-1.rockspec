package = 'fastparse'
version = 'scm-1'
source  = {
    url    = 'git://github.com/no1seman/fastparse.git',
    branch = 'master',
}
description = {
    summary  = "Tarantool Cartridge fast GraphQL parser",
    homepage = 'git://github.com/no1seman/fastparse.git',
    license  = 'BSD',
}
dependencies = {
    'lua >= 5.1',
}
external_dependencies = {
    TARANTOOL = {
        header = "tarantool/module.h"
    }
}
build = {
    type = 'cmake',

    variables = {
        version = 'scm-1',
        TARANTOOL_DIR = '$(TARANTOOL_DIR)',
        TARANTOOL_INSTALL_LIBDIR = '$(LIBDIR)',
        TARANTOOL_INSTALL_LUADIR = '$(LUADIR)',
    }
}