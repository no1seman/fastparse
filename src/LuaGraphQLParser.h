/**
 * Copyright 2019-present, GraphQL Foundation
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * The purpose of this file is to provide a nice interface to parsing
 * GraphQL, rather than the old-fashioned interface provided by bison
 * and flex.
 */

#pragma once

#include <memory>
#include <cstdio>

namespace facebook
{
namespace graphql
{

namespace ast
{
class Node;
}

auto parseString(const char *text, const char **error) -> std::unique_ptr<ast::Node> ;

auto parseStringWithExperimentalSchemaSupport(const char *text, const char **error) -> std::unique_ptr<ast::Node>;

} // namespace graphql
} // namespace facebook
