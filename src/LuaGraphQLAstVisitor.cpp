#include <cstring>
#include <sstream>

#include "GraphQLAstVisitor.h"
#include "AstNode.h"
#include "AstVisitor.h"

#include "LuaGraphQLAstVisitor.h"

static auto visit_document(const struct GraphQLAstDocument *document, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "document");
    obj_begin(L);
    return 1;
}

static void end_visit_document(const struct GraphQLAstDocument *document, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_end(L, "definitions");
}

static auto visit_operation_definition(const GraphQLAstOperationDefinition *operation_definition, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "operation");
    obj_string(L, "operation", GraphQLAstOperationDefinition_get_operation(operation_definition));
    return 1;
}

static void end_visit_operation_definition(const GraphQLAstOperationDefinition *operation_definition, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    arr_end(L);
}

static auto visit_variable_definition_set(const GraphQLAstVariableDefinitionSet *variableDefinitionSet, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    return 1;
}

static void end_visit_variable_definition_set(const GraphQLAstVariableDefinitionSet *variableDefinitionSet, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_end(L, "variableDefinitions");
}

static auto visit_variable_definition(const GraphQLAstVariableDefinition *variable_definition, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    arr_begin(L);
    obj_string(L, "kind", "variableDefinition");
    return 1;
}

static void end_visit_variable_definition(const GraphQLAstVariableDefinition *variable_definition, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    arr_end(L);
}

static auto visit_selection_set(const GraphQLAstSelectionSet *selection_set, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "selectionSet");
    obj_begin(L);
    return 1;
}

static void end_visit_selection_set(const GraphQLAstSelectionSet *selection_set, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_end(L, "selections");
    obj_end(L, "selectionSet");
}

static auto visit_field(const struct GraphQLAstField *field, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "field");
    return 1;
}

static void end_visit_field(const GraphQLAstField *field, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    arr_end(L);
}

static auto visit_alias(const struct GraphQLAstAlias *alias, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "alias");
    return 1;
}

static void end_visit_alias(const GraphQLAstAlias *alias, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_end(L, "alias");
}

static auto visit_argument_set(const GraphQLAstArgumentSet *argument_set, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    return 1;
}

static void end_visit_argument_set(const GraphQLAstArgumentSet *argument_set, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_end(L, "arguments");
}

static auto visit_argument(const GraphQLAstArgument *argument, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    arr_begin(L);
    obj_string(L, "kind", "argument");
    return 1;
}

static void end_visit_argument(const GraphQLAstArgument *argument, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    arr_end(L);
}

static auto visit_fragment_spread(const GraphQLAstFragmentSpread *fragment_spread, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "fragmentSpread");
    return 1;
}

static void end_visit_fragment_spread(const GraphQLAstFragmentSpread *fragment_spread, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    arr_end(L);
}

static auto visit_inline_fragment(const GraphQLAstInlineFragment *inline_fragment, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "inlineFragment");
    return 1;
}

static void end_visit_inline_fragment(const GraphQLAstInlineFragment *inline_fragment, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    arr_end(L);
}

static auto visit_fragment_definition(const GraphQLAstFragmentDefinition *fragment_definition, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "fragmentDefinition");
    return 1;
}

static void end_visit_fragment_definition(const GraphQLAstFragmentDefinition *fragment_definition, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    arr_end(L);
}

static auto visit_type_condition(const GraphQLAstTypeCondition *typeCondition, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "namedType");
    return 1;
}

static void end_visit_type_condition(const GraphQLAstTypeCondition *typeCondition, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_end(L, "typeCondition");
}

static auto visit_variable(const GraphQLAstVariable *variable, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "variable");
    return 1;
}

static void end_visit_variable(const GraphQLAstVariable *variable, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_end(L, "variable");
}

static auto visit_variable_value(const GraphQLAstVariableValue *variable_value, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "variable");
    return 1;
}

static void end_visit_variable_value(const GraphQLAstVariableValue *variable_value, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_end(L, "value");
}

static auto visit_int_value(const GraphQLAstIntValue *int_value, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "int");
    obj_string(L, "value", GraphQLAstIntValue_get_value(int_value));
    return 1;
}

static void end_visit_int_value(const GraphQLAstIntValue *int_value, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_end(L, "value");
}

static auto visit_int_value_arr(const GraphQLAstIntValueArr *int_value, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    arr_begin(L);
    obj_string(L, "kind", "int");
    obj_string(L, "value", GraphQLAstIntValueArr_get_value(int_value));
    return 1;
}

static void end_visit_int_value_arr(const GraphQLAstIntValueArr *int_value, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    arr_end(L);
}

static auto visit_float_value(const GraphQLAstFloatValue *float_value, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "float");
    obj_string(L, "value", GraphQLAstFloatValue_get_value(float_value));
    return 1;
}

static void end_visit_float_value(const GraphQLAstFloatValue *float_value, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_end(L, "value");
}

static auto visit_float_value_arr(const GraphQLAstFloatValueArr *float_value, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    arr_begin(L);
    obj_string(L, "kind", "float");
    obj_string(L, "value", GraphQLAstFloatValueArr_get_value(float_value));
    return 1;
}

static void end_visit_float_value_arr(const GraphQLAstFloatValueArr *float_value, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    arr_end(L);
}

static auto visit_string_value(const GraphQLAstStringValue *string_value, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "string");
    obj_string(L, "value", GraphQLAstStringValue_get_value(string_value));
    return 1;
}

static void end_visit_string_value(const GraphQLAstStringValue *string_value, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_end(L, "value");
}

static auto visit_string_value_arr(const GraphQLAstStringValueArr *string_value, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    arr_begin(L);
    obj_string(L, "kind", "string");
    obj_string(L, "value", GraphQLAstStringValueArr_get_value(string_value));
    return 1;
}

static void end_visit_string_value_arr(const GraphQLAstStringValueArr *string_value, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    arr_end(L);
}

static auto visit_boolean_value(const GraphQLAstBooleanValue *boolean_value, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "boolean");
    obj_string(L, "value", GraphQLAstBooleanValue_get_value(boolean_value) ? "true" : "false");
    return 1;
}

static void end_visit_boolean_value(const GraphQLAstBooleanValue *boolean_value, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_end(L, "value");
}

static auto visit_boolean_value_arr(const GraphQLAstBooleanValueArr *boolean_value, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    arr_begin(L);
    obj_string(L, "kind", "boolean");
    obj_string(L, "value", GraphQLAstBooleanValueArr_get_value(boolean_value) ? "true" : "false");
    return 1;
}

static void end_visit_boolean_value_arr(const GraphQLAstBooleanValueArr *boolean_value, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    arr_end(L);
}

static auto visit_null_value(const GraphQLAstNullValue *null_value, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "nullValue");
    return 1;
}

static void end_visit_null_value(const GraphQLAstNullValue *null_value, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_end(L, "value");
}

static auto visit_null_value_arr(const GraphQLAstNullValueArr *null_value, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    arr_begin(L);
    obj_string(L, "kind", "nullValue");
    return 1;
}

static void end_visit_null_value_arr(const GraphQLAstNullValueArr *null_value, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    arr_end(L);
}

static auto visit_enum_value(const GraphQLAstEnumValue *enum_value, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "enum");
    obj_string(L, "value", GraphQLAstEnumValue_get_value(enum_value));
    return 1;
}

static void end_visit_enum_value(const GraphQLAstEnumValue *enum_value, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_end(L, "defaultValue");
}

static auto visit_list_value(const GraphQLAstListValue *list_value, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "list");
    obj_begin(L);
    return 1;
}

static void end_visit_list_value(const GraphQLAstListValue *list_value, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_end(L, "values");
    obj_end(L, "value");
}

static auto visit_object_value(const GraphQLAstObjectValue *object_value, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "inputObject");
    obj_begin(L);
    return 1;
}

static void end_visit_object_value(const GraphQLAstObjectValue *object_value, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_end(L, "values");
    obj_end(L, "value");
}

static auto visit_object_field(const GraphQLAstObjectField *object_field, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);

    return 1;
}

static void end_visit_object_field(const GraphQLAstObjectField *object_field, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_string(L, "name", GraphQLAstName_get_value(GraphQLAstObjectField_get_name(object_field)));
    arr_end(L);
}

static auto visit_directive_set(const GraphQLAstDirectiveSet *directiveSet, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    return 1;
}

static void end_visit_directive_set(const GraphQLAstDirectiveSet *directiveSet, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_end(L, "directives");
}

static auto visit_directive(const GraphQLAstDirective *directive, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "directive");
    return 1;
}

static void end_visit_directive(const GraphQLAstDirective *directive, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    arr_end(L);
}

static auto visit_named_type(const GraphQLAstNamedType *named_type, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "namedType");
    return 1;
}

static void end_visit_named_type(const GraphQLAstNamedType *named_type, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_end(L, "type");
}

static auto visit_list_type(const GraphQLAstListType *list_type, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "listType");
    return 1;
}

static void end_visit_list_type(const GraphQLAstListType *list_type, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_end(L, "type");
}

static auto visit_non_null_type(const GraphQLAstNonNullType *non_null_type, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "nonNullType");
    return 1;
}

static void end_visit_non_null_type(const GraphQLAstNonNullType *non_null_type, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_end(L, "type");
}

static auto visit_name(const struct GraphQLAstName *name, void *user_data) -> int
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_begin(L);
    obj_string(L, "kind", "name");
    obj_string(L, "value", GraphQLAstName_get_value(name));
    return 1;
}

static void end_visit_name(const GraphQLAstName *name, void *user_data)
{
    auto *L = static_cast<lua_State *>(user_data);
    obj_end(L, "name");
}

struct GraphQLAstVisitorCallbacks callbacks_test = {
    .visit_document = visit_document,
    .end_visit_document = end_visit_document,
    .visit_operation_definition = visit_operation_definition,
    .end_visit_operation_definition = end_visit_operation_definition,
    .visit_variable_definition_set = visit_variable_definition_set,
    .end_visit_variable_definition_set = end_visit_variable_definition_set,
    .visit_variable_definition = visit_variable_definition,
    .end_visit_variable_definition = end_visit_variable_definition,
    .visit_selection_set = visit_selection_set,
    .end_visit_selection_set = end_visit_selection_set,
    .visit_field = visit_field,
    .end_visit_field = end_visit_field,
    .visit_alias = visit_alias,
    .end_visit_alias = end_visit_alias,
    .visit_argument_set = visit_argument_set,
    .end_visit_argument_set = end_visit_argument_set,
    .visit_argument = visit_argument,
    .end_visit_argument = end_visit_argument,
    .visit_fragment_spread = visit_fragment_spread,
    .end_visit_fragment_spread = end_visit_fragment_spread,
    .visit_inline_fragment = visit_inline_fragment,
    .end_visit_inline_fragment = end_visit_inline_fragment,
    .visit_fragment_definition = visit_fragment_definition,
    .end_visit_fragment_definition = end_visit_fragment_definition,
    .visit_type_condition = visit_type_condition,
    .end_visit_type_condition = end_visit_type_condition,
    .visit_variable = visit_variable,
    .end_visit_variable = end_visit_variable,
    .visit_variable_value = visit_variable_value,
    .end_visit_variable_value = end_visit_variable_value,
    .visit_int_value = visit_int_value,
    .end_visit_int_value = end_visit_int_value,
    .visit_int_value_arr = visit_int_value_arr,
    .end_visit_int_value_arr = end_visit_int_value_arr,
    .visit_float_value = visit_float_value,
    .end_visit_float_value = end_visit_float_value,
    .visit_float_value_arr = visit_float_value_arr,
    .end_visit_float_value_arr = end_visit_float_value_arr,
    .visit_string_value = visit_string_value,
    .end_visit_string_value = end_visit_string_value,
    .visit_string_value_arr = visit_string_value_arr,
    .end_visit_string_value_arr = end_visit_string_value_arr,
    .visit_boolean_value = visit_boolean_value,
    .end_visit_boolean_value = end_visit_boolean_value,
    .visit_boolean_value_arr = visit_boolean_value_arr,
    .end_visit_boolean_value_arr = end_visit_boolean_value_arr,
    .visit_null_value = visit_null_value,
    .end_visit_null_value = end_visit_null_value,
    .visit_null_value_arr = visit_null_value_arr,
    .end_visit_null_value_arr = end_visit_null_value_arr,
    .visit_enum_value = visit_enum_value,
    .end_visit_enum_value = end_visit_enum_value,
    .visit_list_value = visit_list_value,
    .end_visit_list_value = end_visit_list_value,
    .visit_object_value = visit_object_value,
    .end_visit_object_value = end_visit_object_value,
    .visit_object_field = visit_object_field,
    .end_visit_object_field = end_visit_object_field,
    .visit_directive_set = visit_directive_set,
    .end_visit_directive_set = end_visit_directive_set,
    .visit_directive = visit_directive,
    .end_visit_directive = end_visit_directive,
    .visit_named_type = visit_named_type,
    .end_visit_named_type = end_visit_named_type,
    .visit_list_type = visit_list_type,
    .end_visit_list_type = end_visit_list_type,
    .visit_non_null_type = visit_non_null_type,
    .end_visit_non_null_type = end_visit_non_null_type,
    .visit_name = visit_name,
    .end_visit_name = end_visit_name,
    .visit_schema_definition = nullptr,
    .end_visit_schema_definition = nullptr,
    .visit_operation_type_definition = nullptr,
    .end_visit_operation_type_definition = nullptr,
    .visit_scalar_type_definition = nullptr,
    .end_visit_scalar_type_definition = nullptr,
    .visit_object_type_definition = nullptr,
    .end_visit_object_type_definition = nullptr,
    .visit_field_definition = nullptr,
    .end_visit_field_definition = nullptr,
    .visit_input_value_definition = nullptr,
    .end_visit_input_value_definition = nullptr,
    .visit_interface_type_definition = nullptr,
    .end_visit_interface_type_definition = nullptr,
    .visit_interface_extension_definition = nullptr,
    .end_visit_interface_extension_definition = nullptr,
    .visit_union_type_definition = nullptr,
    .end_visit_union_type_definition = nullptr,
    .visit_enum_type_definition = nullptr,
    .end_visit_enum_type_definition = nullptr,
    .visit_enum_value_definition = nullptr,
    .end_visit_enum_value_definition = nullptr,
    .visit_input_object_type_definition = nullptr,
    .end_visit_input_object_type_definition = nullptr,
    .visit_type_extension_definition = nullptr,
    .end_visit_type_extension_definition = nullptr,
    .visit_directive_definition = nullptr,
    .end_visit_directive_definition = nullptr};
