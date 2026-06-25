--- http 模块验证：GET 请求 + 文件下载
local T = dofile("app/tests/common.lua")
local http = T.mod("http")
local fs = T.mod("fs")

local TMP = "app/tests/tmp"
local DOWNLOAD = TMP .. "/http_download.html"

return function()
    if not fs.exists(TMP) then
        fs.mkdir(TMP, 0755)
    end

    local res = http.get("http://example.com/", { timeout = 20000 })
    if res and res.status_code == 200 then
        T.assert_true("http.get", true, "status=200")
        T.assert_true("http.get.body", res.body and #res.body > 0,
            "len=" .. tostring(res.body and #res.body))
    else
        T.skip("http.get", res and ("status=" .. tostring(res.status_code)) or "nil response")
    end

    local ok, err = http.download("http://example.com/", DOWNLOAD)
    if T.assert_true("http.download", ok, err) then
        T.assert_true("http.download.file_exists", fs.exists(DOWNLOAD))
        local f = fs.open(DOWNLOAD, "r")
        if f then
            local data = f:read(4096)
            f:close()
            T.assert_true("http.download.content", data and #data > 0, "len=" .. tostring(data and #data))
        else
            T.fail("http.download.read", "cannot open " .. DOWNLOAD)
        end
    end
end
