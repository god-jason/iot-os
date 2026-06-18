-----------------------------------------------------------------------
-- Xmake script file
-- Copyright (C) 2022-2026, Xinsheng
-- All rights reserved.
--
-- @author      wangyuanlin
-- @file        xmake.lua
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Configuration
-----------------------------------------------------------------------
local TARGET_NAME = "cmaudioplayer"

-----------------------------------------------------------------------
-- Export global macro
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Export global include directory
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Private function
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Target
-----------------------------------------------------------------------
target(TARGET_NAME)
-- [don't edit] ---
    set_kind("static")
	add_deps("base")
	add_global_deps(TARGET_NAME)
-- [   end    ] ---
    add_includedirs("src")
    add_includedirs("src/ringBuffer")
    add_srcfile("src/cm_audio_common.c")
    add_srcfile("src/cm_audio_player.c")
    add_srcfile("src/test/shell_test_audio_play.c")
    add_srcfile("src/test/at_test_audio_play.c")
    add_srcfile("src/ringBuffer/cm_ringBuffer.c")
    add_srcfile("src/ringBuffer/cm_ringBuffer_test.c")
target_end()
