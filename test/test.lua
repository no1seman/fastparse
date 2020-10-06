#!/usr/bin/env tarantool

local fio = require('fio')
local checks = require('checks')
local errors = require('errors')
local json = require('json')
local log = require('log')
local clock = require('clock')

local utils = require('cartridge.utils')

local parse = require('cartridge.graphql.parse')
local fastparse = require('fastparse')

local Parse_Error = errors.new_class('GraphQL parse failed')

local RED = '\27[31m'
local GREEN = '\27[32m'
local RESET = '\27[0m'

local function load_file(filename)
    checks('string')
    local data, err = utils.file_read(filename)
    if data == nil then return nil, err end

    return data
end

local function save_file(filename, data)
    checks('string', 'string')
    local _data, err = utils.file_write(filename, data)
    if _data == nil then return nil, err end

    return data
end

local function table_eq(table1, table2)
    local avoid_loops = {}
    local function recurse(t1, t2)
        -- compare value types
        if type(t1) ~= type(t2) then return false end
        -- Base case: compare simple values
        if type(t1) ~= "table" then return t1 == t2 end
        -- Now, on to tables.
        -- First, let's avoid looping forever.
        if avoid_loops[t1] then return avoid_loops[t1] == t2 end
        avoid_loops[t1] = t2
        -- Copy keys from t2
        local t2keys = {}
        local t2tablekeys = {}
        for k, _ in pairs(t2) do
            if type(k) == 'table' then table.insert(t2tablekeys, k) end
            t2keys[k] = true
        end
        -- Let's iterate keys from t1
        for k1, v1 in pairs(t1) do
            local v2 = t2[k1]
            if type(k1) == 'table' then
                -- if key is a table, we need to find an equivalent one.
                local ok = false
                for i, tk in ipairs(t2tablekeys) do
                    if table_eq(k1, tk) and recurse(v1, t2[tk]) then
                        table.remove(t2tablekeys, i)
                        t2keys[tk] = nil
                        ok = true
                        break
                    end
                end
                if not ok then return false end
            else
                -- t1 has a key which t2 doesn't have, fail.
                if v2 == nil then return false end
                t2keys[k1] = nil
                if not recurse(v1, v2) then return false end
            end
        end
        -- if t2 has a key which t1 doesn't have, fail.
        if next(t2keys) then return false end
        return true
    end
    return recurse(table1, table2)
end

local function test_file(file, file_path, temp_folder, print_diag, parse_type)
    local start_fast, stop_fast, start_vanilla, stop_vanilla, duration_fast,
          duration_vanilla
    -- load request file
    local request = load_file(file_path)

    -- make fast parse
    start_fast = clock.monotonic64()
    local fast_parse, err

    if parse_type == 'schema' then
        fast_parse, err = fastparse.parse_schema(request)
    else
        fast_parse, err = fastparse.parse_query(request)
    end
    stop_fast = clock.monotonic64()

    if temp_folder then
        if fast_parse then
            save_file(fio.pathjoin(temp_folder, file .. '_fast.json'),
                      json.encode(fast_parse))
        else
            save_file(fio.pathjoin(temp_folder, file .. '_fast.err'),
                      tostring(err))
        end
    end

    duration_fast = fast_parse and tonumber(stop_fast - start_fast) / 1000.0 or 0

    -- make vanilla parse
    start_vanilla = clock.monotonic64()
    local vanilla_parse
    vanilla_parse, err = Parse_Error:pcall(parse.parse, request)
    stop_vanilla = clock.monotonic64()
    if temp_folder then
        if vanilla_parse then
            save_file(fio.pathjoin(temp_folder, file .. '_vanilla.json'),
                      json.encode(vanilla_parse))
        else
            save_file(fio.pathjoin(temp_folder, file .. '_vanilla.err'),
                      tostring(err))
        end
    end

    duration_vanilla = fast_parse and tonumber(stop_vanilla - start_vanilla) / 1000.0 or 0

    -- print results
    if duration_fast and duration_vanilla then
        if print_diag then
            print('Fast parse "' .. file .. '.graphql" ' ..
                      tostring(duration_fast) .. ' us ')
            print('Vanilla parse "' .. file .. '.graphql" ' ..
                      tostring(duration_vanilla) .. ' us ')
            print('"Fast parse" faster than "Vanilla parse": ' ..
                      tostring(duration_vanilla / duration_fast))
        end
        -- check equality of parse results
        local equal = table_eq(vanilla_parse, fast_parse)
        if print_diag then
            print(file .. '.graphql : ' .. tostring(equal))
        end
        return equal, duration_fast, duration_vanilla
    end
    return false
end

local function test(folder, temp_folder, iterations, parse_type)
    if not parse_type or parse_type ~= 'schema' or parse_type ~= 'query' then
        parse_type = 'query'
    end

    local fast = 0
    local vanilla = 0
    local success = 0
    local fail = 0
    if fio.path.is_dir(fio.abspath(folder)) then
        local files_list, err = fio.glob(fio.pathjoin(folder, '*.graphql'))

        if files_list then
            for i = 1, iterations do
                for _, file in pairs(files_list) do
                    local filename = fio.basename(file):sub(0, #fio.basename(
                                                                file) -
                                                                #fio.basename(
                                                                    file):match(
                                                                    '^.+(%..+)$'))
                    local file_path = fio.abspath(file)
                    local temp_path =
                        temp_folder and fio.abspath(temp_folder) or nil
                    -- print('Processing: ' .. filename .. ' - ' .. file_path .. ' - '.. temp_path)
                    local ok, _fast, _vanilla =
                        test_file(filename, file_path, temp_path, false, parse_type)

                    if iterations == 1 then
                        if ok then
                            print('\t' .. filename .. ' - ' .. GREEN ..
                                      'Success' .. RESET)
                        else
                            print('\t' .. filename .. ' - ' .. RED .. 'Fail' ..
                                      RESET)
                        end
                    end

                    if ok then
                        fast = fast + _fast
                        vanilla = vanilla + _vanilla
                        success = success + 1
                    else
                        fail = fail + 1
                    end
                end
            end
        end

        if iterations > 1 then
            print('Total time "vanilla parse": ' .. tostring(vanilla) .. ' us')
            print('Total time "fast parse": ' .. tostring(fast) .. ' us')
            print('Iterations: ' .. tostring(iterations))
            print('Total requests: ' .. tostring(success))
            print('Total errors: ' .. tostring(fail))
            local ratio = fast == 0 and 0 or tostring(vanilla / fast)
            print('"Fast parse" ' .. GREEN .. ratio .. RESET ..
                      ' times faster than "vanilla parser"!')
        end
        return true, success, fail
    end
    print('Error: ' .. folder .. ' is not a folder. Nothing to process')
    return false, 0, 0
end

local function create_folder_if_not_exist(folder)
    if not fio.path.exists(folder) then
        fio.mktree(folder)
    else
        -- if path is already exists then cleaning all files in it
        fio.rmdir(folder)
        fio.mktree(folder)
    end
    return folder
end

print('GraphQL spec test:')
test('./fixtures/spec', create_folder_if_not_exist('tmp/spec'), 1)

print('Tarantool Cartridge parser compatibility test:')
test('./fixtures/execution/', create_folder_if_not_exist('tmp/execution'), 1, 'query')

print('Schema parse test:')
test('./fixtures/schema/', create_folder_if_not_exist('./tmp/schema'), 1, 'schema')

print('Performance test:')
test('./fixtures/execution/', nil, 100)

