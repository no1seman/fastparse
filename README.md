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

## Intermediate test results

Tests was performed on: Core i7-8550U, Windows 10 20H1, WSL2, Ubuntu 20.04, Tarantool 2.4.3-1, Cartridge 2.3.0

Short run test:
Request|Iterations|Vanilla took time, ms|Fast took time, ms|Vanilla, RPS|Fast, RPS|FastParse vs Vanilla ratio
-|-|-|-|-|-|-
create_space|1|0.460|0.116|2172|8598|3.957
delete_space|1|0.150|0.022|6644|44843|6.749
edittopology|1|0.217|0.039|4612|25510|5.531
get_space|1|0.282|0.068|3542|14662|4.139
getcluster|1|0.616|0.160|1624|6234|3.837
instance_data|1|0.988|0.164|1012|6090|6.016
introspection|1|0.746|0.132|1340|7575|5.653
kitchen-sink|1|0.971|0.114|1030|8802|8.545
query_nested|1|0.153|0.028|6514|35842|5.502

Long run test:
Request|Iterations|Vanilla took time, ms|Fast took time, ms|Vanilla, RPS|Fast, RPS|Fast vs Vanilla ratio|
-|-|-|-|-|-|-
create_space|1000000|178663.446|74311.685|5597|13456|2.404
delete_space|1000000|27708.995|9429.785|36089|106046|2.938
edittopology|1000000|67339.603|24878.481|14850|40195|2.707
get_space|1000000|135392.278|51998.175|7385|19231|2.604
getcluster|1000000|386361.732|193017.407|2588|5180|2.002
instance_data|1000000|420601.482|177021.836|2377|5649|2.376
introspection|1000000|334205.194|143284.584|2992|6979|2.332
kitchen-sink|1000000|313315.563|108970.258|3191|9176|2.875
query_nested|1000000|57434.327|17569.229|17411|56917|3.269

## Known issues/TODO

- make wrapper to comply vanilla parser restrictions;
- add more tests;
- fix schema parsing;
- make more tests to comply last GraphQL draft spec;
- add OOM/memory leaks tests;
