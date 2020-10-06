/**
 * Copyright 2019-present, GraphQL Foundation
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "parser/location.hh"

namespace facebook
{
  namespace graphql
  {
    namespace ast
    {

      namespace visitor
      {
        class AstVisitor;
      }
      //NOLINTNEXTLINE
      class Node
      {
        yy::location location_state;

      public:
        explicit Node(const yy::location &location)
            : location_state(location) {}

        virtual ~Node() = default;

        const yy::location &getLocation() const
        {
          return location_state;
        }

        virtual void accept(visitor::AstVisitor *visitor) const = 0;
      };

    } // namespace ast
  }   // namespace graphql
} // namespace facebook
