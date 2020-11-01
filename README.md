# fastparse

fastparse is a library for Tarantool Cartridge that parses GraphQL requests into AST.

As for now Tarantool Cartridge has builtin implementation of GraphQL parser based on [lulpeg](https://github.com/pygy/LuLPeg). But it has the following problems:

- it's relatevily slow (c++ realisation at least 2 times faster)
- it's terrebly slow at low rate requests, because LuaJIT compiles function only if it called more than 30-50 times
- if LauJIT trace brakes GC may unload function and on next time it interprets the code again which is slow

This parser based on [libgraphqlparser](https://github.com/graphql/libgraphqlparser)

Note AST tree of Tarantool Cartridge executor is not compatible with Facebook's js implementation.

## Usage

Attention!!! At that very moment it's not for production purposes!!! ONLY as PoC!!!

## Build

### Prequisities

Flex (>=2.6)
Bison (>= 3.7)
Python (>=3.5)
Tarantool (>=2.x)
cmake
make
gcc
clang-tidy

### Clone repo

```sh
git clone https://github.com/no1seman/fastparse
cd fastparse
```

### Build rock

```sh
scripts/build_rock.sh
```

After build completion you will get:

- fastparse-{version}.{platform}.rock
- fastparse lib installed in local .rocks folder

### Test

#### Install Tarantool Cartridge in test folder

```sh
cd fastparse/test
tarantoolctl rocks install cartridge
```

#### Run tests

To test lib simply build rock and then run:

```sh
scripts/test.sh
```

## Install

Run the following command:

```sh
cd <Tarantool Cartridge application dir>
tarantoolctl rocks install <path to rock file>/fastparse-{version}.{platform}.rock
```

Then you need to open {Tarantool Cartridge application dir}/.rocks/share/tarantool/cartridge/graphql.lua in editor and:

1. Comment the line: ```local parse = require('cartridge.graphql.parse')```
2. Add the following line: ```local parse = require('fastparse')```

After restart of Tarantool Cartridge App it will use "fastparse" instead of vanilla GraphQL parser.

## Known issues/TODO

- fix schema parsing;
- make more tests to comply last GraphQL draft spec;
- add OOM/memory leaks tests;
