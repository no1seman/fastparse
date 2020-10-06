local graphqlparser = require("fastparse.lib")
local checks = require("checks")
local errors = require("errors")

local Fast_Parse_Error = errors.new_class("GraphQL fastparse error")

local function parse_query(query_string)
    checks("string")
    return Fast_Parse_Error:pcall(graphqlparser.parse_query, query_string)
end

local function parse_schema(schema_string)
    checks("string")
    return Fast_Parse_Error:pcall(graphqlparser.parse_schema, schema_string)
end

return {
    parse = parse_query,
    parse_query = parse_query,
    parse_schema = parse_schema,
}