--- zlib 模块验证：内存压缩/解压 + 文件 gzip/zip
local T = dofile("app/tests/common.lua")
local zlib = T.mod("zlib")
local fs = T.mod("fs")

local TMP = "app/tests/tmp"
local SRC = TMP .. "/zlib_src.txt"
local GZ = TMP .. "/zlib_src.txt.gz"
local GZ_OUT = TMP .. "/zlib_out.txt"
local ZIP = TMP .. "/zlib_test.zip"
local ZIP_DIR = TMP .. "/zip_out"

local function ensure_tmp()
    if not fs.exists(TMP) then
        fs.mkdir(TMP, 0755)
    end
end

local function write_file(path, data)
    local f = fs.open(path, "w")
    if not f then return false end
    f:write(data)
    f:close()
    return true
end

local function read_file(path)
    local f = fs.open(path, "r")
    if not f then return nil end
    local data = f:read(1024 * 1024)
    f:close()
    return data
end

return function()
    ensure_tmp()

    local payload = string.rep("iot-os zlib test ", 64)
    local ok, err

    -- deflate 内存往返
    local compressed = zlib.deflate.compress(payload, 6)
    if T.assert_true("zlib.deflate.compress", compressed ~= nil, err) then
        local restored, derr = zlib.deflate.decompress(compressed)
        T.assert_true("zlib.deflate.decompress", restored == payload, derr)
    end

    -- gzip 内存往返
    local gz = zlib.gzip.compress(payload, 6)
    if T.assert_true("zlib.gzip.compress", gz ~= nil) then
        local restored = zlib.gzip.decompress(gz)
        T.assert_true("zlib.gzip.decompress", restored == payload)
    end

    -- gzip 文件往返
    if T.assert_true("zlib.write_src", write_file(SRC, payload)) then
        ok = zlib.gzip.compress_file(SRC, GZ, 6)
        if T.assert_true("zlib.gzip.compress_file", ok) then
            ok = zlib.gzip.decompress_file(GZ, GZ_OUT)
            if T.assert_true("zlib.gzip.decompress_file", ok) then
                local restored = read_file(GZ_OUT)
                T.assert_true("zlib.gzip.file_roundtrip", restored == payload)
            end
        end
    end

    -- zip 文件打包/解压
    ok = zlib.zip.compress_file(ZIP, { SRC }, 6)
    if T.assert_true("zlib.zip.compress_file", ok) then
        if not fs.exists(ZIP_DIR) then
            fs.mkdir(ZIP_DIR, 0755)
        end
        ok = zlib.zip.decompress_file(ZIP, ZIP_DIR)
        if T.assert_true("zlib.zip.decompress_file", ok) then
            local extracted = read_file(ZIP_DIR .. "/zlib_src.txt")
            T.assert_true("zlib.zip.file_roundtrip", extracted == payload)
        end
    end
end
