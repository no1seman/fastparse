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

local function test_file(file, file_path, temp_folder, parse_type)
    -- load request file
    local request = load_file(file_path)

    -- remove old saved files
    if temp_folder then
        fio.unlink(fio.pathjoin(temp_folder, file .. '_fast.json'))
        fio.unlink(fio.pathjoin(temp_folder, file .. '_fast.err'))

        fio.unlink(fio.pathjoin(temp_folder, file .. '_vanilla.json'))
        fio.unlink(fio.pathjoin(temp_folder, file .. '_vanilla.err'))
    end

    -- make fast parse
    local fast_parse, fast_err, vanilla_parse, vanilla_err

    if parse_type == 'schema' then
        fast_parse, fast_err = fastparse.parse_schema(request)
        vanilla_parse = nil
        vanilla_err = nil
    else
        fast_parse, fast_err = fastparse.parse_query(request)
        vanilla_parse, vanilla_err = Parse_Error:pcall(parse.parse, request)
    end

    if temp_folder then
        if fast_parse then
            save_file(fio.pathjoin(temp_folder, file .. '_fast.json'),
                      json.encode(fast_parse))
        else
            save_file(fio.pathjoin(temp_folder, file .. '_fast.err'),
                      tostring(fast_err))
        end

        if parse_type == 'query' then
            if vanilla_parse then
                save_file(fio.pathjoin(temp_folder, file .. '_vanilla.json'),
                        json.encode(vanilla_parse))
            else
                save_file(fio.pathjoin(temp_folder, file .. '_vanilla.err'),
                        tostring(vanilla_err))
            end
        end
    end

    if parse_type == 'schema' then
        if fast_parse then
            return true
        else
            return false
        end
    else
        if not fast_parse or not vanilla_parse then
            return false
        else
            return table_eq(vanilla_parse, fast_parse)
        end
    end
end

local function test(folder, temp_folder, parse_type)
    if parse_type == nil and parse_type ~= 'schema' and parse_type ~= 'query' then
        parse_type = 'query'
    end

    local success = 0
    local fail = 0
    if fio.path.is_dir(fio.abspath(folder)) then
        local files_list, err = fio.glob(fio.pathjoin(folder, '*.graphql'))

        if files_list then
            for _, file in pairs(files_list) do
                local filename = fio.basename(file):sub(0, #fio.basename(
                                                            file) -
                                                            #fio.basename(
                                                                file):match(
                                                                '^.+(%..+)$'))
                local file_path = fio.abspath(file)
                local temp_path = temp_folder and fio.abspath(temp_folder) or nil
                local ok = test_file(filename, file_path, temp_path, parse_type)

                if ok then
                    success = success + 1
                    print('\t' .. filename .. ' - ' .. GREEN .. 'Success' .. RESET)
                else
                    fail = fail + 1
                    print('\t' .. filename .. ' - ' .. RED .. 'Fail' .. RESET)
                end
            end
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

local function print_col(str, width, justify)
    if justify == 'left' or justify == nil then
        return str .. string.rep(' ', width - #str)
    elseif justify == 'center' then
        local half = math.floor((width - #str)/2)
        return string.rep(' ', half) .. str .. string.rep(' ', width - #str - half)
    else
        return string.rep(' ', width - #str) .. str
    end

end

local function test_perf(folder, iterations, parse_type)
    local requests = {}
    -- load all requests
    if fio.path.is_dir(fio.abspath(folder)) then
        local files_list, err = fio.glob(fio.pathjoin(folder, '*.graphql'))
        if files_list then
            for _, file in pairs(files_list) do
                local item = {
                    request = load_file(fio.abspath(file)),
                    vanilla = 0,
                    fast = 0,
                    name = fio.basename(file):sub(0, #fio.basename(file) -
                           #fio.basename(file):match('^.+(%..+)$'))
                }
                table.insert(requests, item)
            end
        else
            return
        end

        -- make fast parse
        local fast_parse, start, duration
        for index, request in pairs(requests) do
            for i = 1, iterations do
                start = clock.realtime64()
                if parse_type == 'schema' then
                    fast_parse = fastparse.parse_schema(request.request)
                else
                    fast_parse = fastparse.parse_query(request.request)
                end
                duration = clock.realtime64() - start
                if fast_parse then
                    requests[index].fast = requests[index].fast + tonumber(duration)
                end
            end
        end

        -- make vanilla parse
        local vanilla_parse
        for index, request in pairs(requests) do
            for i = 1, iterations do
                start = clock.realtime64()
                vanilla_parse, err = Parse_Error:pcall(parse.parse, request.request)
                duration = clock.realtime64() - start
                if vanilla_parse then
                    requests[index].vanilla = requests[index].vanilla + tonumber(duration)
                end
            end
        end

        print(
            print_col('Request', 20, 'center') .. ' | ' ..
            print_col('Iterations', 10, 'center') .. ' | ' ..
            print_col('Vanilla took time, ms', 22, 'center') .. ' | '..
            print_col('Fast took time, ms', 22, 'center') .. ' | ' ..
            print_col('Vanilla, RPS', 15, 'center') .. ' | ' ..
            print_col('Fast, RPS', 15, 'center') .. ' | ' ..
            print_col('Fast vs Vanilla ratio', 25, 'center') .. ' | '
        )

        print(
            print_col(string.rep('-', 19), 20, 'center') .. ' | ' ..
            print_col(string.rep('-', 9), 10, 'center') .. ' | ' ..
            print_col(string.rep('-', 21), 22, 'center') .. ' | '..
            print_col(string.rep('-', 21), 22, 'center') .. ' | ' ..
            print_col(string.rep('-', 14), 15, 'center') .. ' | ' ..
            print_col(string.rep('-', 14), 15, 'center') .. ' | ' ..
            print_col(string.rep('-', 24), 25, 'center') .. ' | '
        )

        for i, request in pairs(requests) do
            print(
                print_col(request.name, 20, 'left') .. ' | ' ..
                print_col(tostring(iterations), 10, 'center') .. ' | ' ..
                print_col(string.format('%.3f', request.vanilla/1e6), 22, 'center').. ' | '..
                print_col(string.format('%.3f', request.fast/1e6), 22, 'center').. ' | ' ..
                print_col(tostring(math.floor(1/(request.vanilla/1e9/iterations))), 15, 'right').. ' | ' ..
                print_col(tostring(math.floor(1/(request.fast/1e9/iterations))), 15, 'right').. ' | ' ..
                print_col(string.format('%.3f', request.vanilla/request.fast), 25, 'right').. ' | '
            )
        end
    end
end

print('GraphQL spec test:')
test('./fixtures/spec', create_folder_if_not_exist('tmp/spec'), 'query')

print('Tarantool Cartridge parser compatibility test:')
test('./fixtures/execution/', create_folder_if_not_exist('tmp/execution'), 'query')

print('Schema parse test:')
test('./fixtures/schema/', create_folder_if_not_exist('tmp/schema'), 'schema')

print('\nPerformance test (short run):')
test_perf('./fixtures/execution/', 1, 'query')

print('\nPerformance test (long run):')
test_perf('./fixtures/execution/', 10000, 'query')

