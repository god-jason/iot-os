/*
@module  sqlite3
@summary SQLite3数据库操作
@version 1.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Database
*/

/**
SQLite3参考示例
-- 打开数据库
local db = sqlite3.open("/data/test.db")

-- 创建表
db:exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT, age INTEGER)")

-- 插入数据
db:exec("INSERT INTO users (name, age) VALUES ('张三', 25)")

-- 查询数据
local rows = db:query("SELECT * FROM users")
for i, row in ipairs(rows) do
    print(row.id, row.name, row.age)
end

-- 参数化查询
local stmt = db:prepare("SELECT * FROM users WHERE age > ?")
stmt:bind(1, 18)
local result = stmt:step()
while result do
    print(result.id, result.name, result.age)
    result = stmt:step()
end
stmt:finalize()

-- 关闭数据库
db:close()
*/

#ifndef IOT_SQLITE3_H
#define IOT_SQLITE3_H

#include "lua.h"

LUAMOD_API int luaopen_sqlite3(lua_State* L);

#endif /* IOT_SQLITE3_H */