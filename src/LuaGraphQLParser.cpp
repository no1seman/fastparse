/**
 * Copyright 2019-present, GraphQL Foundation
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "LuaGraphQLParser.h"

#include "AstNode.h"

#include "parser/parser.tab.hpp"
#include "lexer/lexer.h"
#include "parser/syntaxdefs.h"

namespace facebook
{
  namespace graphql
  {

    static auto doParse(const char **outError, yyscan_t scanner, bool enableSchema) -> std::unique_ptr<ast::Node>
    {
      Node *outAST = nullptr;
      yy::GraphQLParserImpl parser(enableSchema, &outAST, outError, scanner);
      int failure = parser.parse();
      if (failure)
      {
        delete outAST;
      }
      return !failure ? std::unique_ptr<ast::Node>(outAST) : nullptr;
    }

    static auto parseStringImpl(const char *text, const char **error, bool enableSchema) -> std::unique_ptr<ast::Node>
    {
      yyscan_t scanner = nullptr;
      struct LexerExtra extra;
      yylex_init_extra(&extra, &scanner);
      YY_BUFFER_STATE buffer = yy_scan_string(text, scanner);
      yy_switch_to_buffer(buffer, scanner);

      auto result = doParse(error, scanner, enableSchema);
      yylex_destroy(scanner);
      return result;
    }

    auto parseString(const char *text, const char **error) -> std::unique_ptr<ast::Node>
    {
      return parseStringImpl(text, error, false);
    }

    auto parseStringWithExperimentalSchemaSupport(const char *text, const char **error) -> std::unique_ptr<ast::Node>
    {
      return parseStringImpl(text, error, true);
    }

  } // namespace graphql
} // namespace facebook
