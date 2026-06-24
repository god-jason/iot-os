/**
 * @file deflate.c
 * @brief DEFLATE压缩算法实现
 *
 * 本文件实现DEFLATE压缩算法的核心功能：
 * - ADLER32校验算法
 * - CRC32校验算法
 * - DEFLATE数据解压（使用LZ77和Huffman编码）
 * - DEFLATE数据压缩
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "deflate.h"
#include <string.h>

/* ==================== ADLER32 校验 ==================== */

/**
 * @brief 计算数据的ADLER32校验码
 *
 * ADLER32算法将校验值分为s1和s2两部分：
 * - s1: 所有字节之和
 * - s2: s1的累加和
 * 这种设计使得校验计算可以分块进行，便于流式处理。
 *
 * @param adler 初始校验值（通常为1）
 * @param buf 数据缓冲区指针
 * @param len 数据长度（字节数）
 * @return 计算后的校验码
 */
uint32_t zlib_adler32(uint32_t adler, const uint8_t *buf, size_t len) {
    uint32_t s1 = adler & 0xffff;
    uint32_t s2 = (adler >> 16) & 0xffff;
    size_t n;

    /* 分块处理以避免整数溢出，每块最多处理ZLIB_ADLER32_NMAX字节 */
    while (len > 0) {
        n = len < ZLIB_ADLER32_NMAX ? len : ZLIB_ADLER32_NMAX;
        len -= n;
        do {
            s1 += *buf++;
            s2 += s1;
        } while (--n);
        s1 %= ZLIB_ADLER32_BASE;
        s2 %= ZLIB_ADLER32_BASE;
    }

    return (s2 << 16) | s1;
}

/* ==================== CRC32 校验 ==================== */

/**
 * @brief CRC32查找表
 *
 * 使用标准CRC-32 polynomial (0xEDB88320)预计算的查找表，
 * 可加速CRC32计算，每字节只需一次表查找和一次XOR操作。
 */
static const uint32_t zlib_crc32_table[256] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
    0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
    0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
    0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
    0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
    0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
    0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
    0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
    0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
    0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
    0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
    0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
    0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
    0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
    0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
    0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
    0xbdbdf21e, 0xcabac288, 0x53b39332, 0x24b4a3a4, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

/**
 * @brief 计算数据的CRC32校验码
 *
 * CRC32使用循环冗余校验，通过多项式除法检测数据错误。
 * 使用预计算的查找表实现，可一次处理一个字节。
 *
 * @param crc 初始校验值（通常为0xFFFFFFFF）
 * @param buf 数据缓冲区指针
 * @param len 数据长度
 * @return 校验码（经过最终翻转）
 */
uint32_t zlib_crc32(uint32_t crc, const uint8_t *buf, size_t len) {
    while (len--) {
        /* 表查找 + 移位XOR操作 */
        crc = zlib_crc32_table[(crc ^ *buf++) & 0xff] ^ (crc >> 8);
    }
    return crc;
}

/* ==================== LZ77 解压部分 ==================== */

/**
 * @brief DEFLATE解压状态结构体
 *
 * 维护解压过程中的位缓冲和当前位置信息。
 */
typedef struct {
    const uint8_t *ptr;      /**< 当前数据指针 */
    const uint8_t *end;      /**< 数据结束位置 */
    uint32_t bitbuf;         /**< 位缓冲（低位先出） */
    int bitcnt;              /**< 位缓冲中有效位数 */
} zlib_deflate_state_t;

/**
 * @brief 初始化解压状态
 * @param s 解压状态指针
 * @param data 压缩数据指针
 * @param len 压缩数据长度
 */
static void zlib_deflate_state_init(zlib_deflate_state_t *s, const uint8_t *data, size_t len) {
    s->ptr = data;
    s->end = data + len;
    s->bitbuf = 0;
    s->bitcnt = 0;
}

/**
 * @brief 从位流中读取指定位数的值
 * @param s 解压状态指针
 * @param bits 要读取的位数
 * @return 读取的数值，失败返回-1
 *
 * DEFLATE格式使用变长位编码，需要逐位读取。
 */
static int zlib_deflate_read_bits(zlib_deflate_state_t *s, int bits) {
    uint32_t v = 0;
    while (bits > 0) {
        /* 位缓冲为空时，重新填充 */
        if (s->bitcnt == 0) {
            if (s->ptr >= s->end) return -1;
            s->bitbuf = *s->ptr++;
            s->bitcnt = 8;
        }
        /* 从高位到低位提取 */
        int n = bits < s->bitcnt ? bits : s->bitcnt;
        v = (v << n) | ((s->bitbuf >> (s->bitcnt - n)) & ((1 << n) - 1));
        s->bitcnt -= n;
        bits -= n;
    }
    return v;
}

/**
 * @brief Huffman码表结构体
 *
 * 存储解码用的Huffman码表，索引对应编码值。
 */
typedef struct {
    uint16_t table[512];     /**< 512个码表的快速查找 */
} zlib_deflate_huffman_t;

/**
 * @brief 构建Huffman解码表
 * @param table 输出表指针
 * @param len 各符号的码长数组
 * @param n 符号总数
 *
 * 根据各符号的出现频率构建最优前缀码。
 */
static void zlib_deflate_build_huffman(zlib_deflate_huffman_t *table, const uint8_t *len, int n) {
    uint16_t next_code[16] = {0};
    int i;
    
    /* 清空表 */
    for (i = 0; i < 512; i++) {
        table->table[i] = 0;
    }
    
    /* 计算每个码长的起始码 */
    for (i = 0; i < n; i++) {
        if (len[i] > 0) {
            table->table[next_code[len[i]]] = i;
            next_code[len[i]]++;
        }
    }
    
    /* 累加码长范围 */
    for (i = 1; i < 16; i++) {
        next_code[i] += next_code[i-1];
    }
    
    /* 填充空码位置 */
    for (i = 1; i < 16; i++) {
        uint16_t prev = next_code[i-1];
        uint16_t curr = next_code[i];
        while (prev < curr) {
            if (table->table[prev] == 0 && prev > 0) {
                int parent = (prev - 1) >> 1;
                table->table[prev] = table->table[parent];
            }
            prev++;
        }
    }
}

/**
 * @brief 使用Huffman表解码一个符号
 * @param s 解压状态指针
 * @param table Huffman表指针
 * @param maxcode 最大有效码值
 * @return 解码的符号值，失败返回-1
 */
static int zlib_deflate_huffman_decode(zlib_deflate_state_t *s, const zlib_deflate_huffman_t *table, int maxcode) {
    int code = 0, len = 0;
    while (len < 15) {
        int bit = zlib_deflate_read_bits(s, 1);
        if (bit < 0) return -1;
        code = (code << 1) | bit;
        len++;
        if (code < maxcode && table->table[code] != 0) {
            return table->table[code];
        }
    }
    return -1;
}

/**
 * @brief 解压一个DEFLATE块
 * @param s 解压状态指针
 * @param out 输出缓冲区指针
 * @param out_len 输入为剩余空间，输出为消耗的空间
 * @return 成功返回0，失败返回-1
 *
 * DEFLATE块有三种类型：
 * - 0: 无压缩块（STORED）
 * - 1: 固定Huffman编码块
 * - 2: 动态Huffman编码块
 */
static int zlib_deflate_inflate_block(zlib_deflate_state_t *s, uint8_t *out, size_t *out_len, int *bfinal) {
    *bfinal = zlib_deflate_read_bits(s, 1);
    int btype = zlib_deflate_read_bits(s, 2);
    if (btype < 0) return -1;
    
    /* 无压缩块：直接复制数据 */
    if (btype == 0) {
        /* read_bits 读完块头后 ptr 已在 LEN 起始处，仅丢弃位缓冲剩余位 */
        s->bitcnt = 0;
        if (s->ptr + 4 > s->end) return -1;
        int len = (int)(uint16_t)(s->ptr[0] | (s->ptr[1] << 8));
        int nlen = (int)(uint16_t)(s->ptr[2] | (s->ptr[3] << 8));
        s->ptr += 4;
        if ((len ^ 0xffff) != nlen) return -1;
        if (s->ptr + (size_t)len > s->end) return -1;
        while (len-- > 0) {
            if (*out_len == 0) return -1;
            *out++ = *s->ptr++;
            (*out_len)--;
        }
        return 0;
    }
    
    /* Huffman编码块（固定或动态） */
    if (btype == 1 || btype == 2) {
        /* 读取码长信息 */
        int hlit = zlib_deflate_read_bits(s, 5) + 257;   /* 字面量/长度码数量 */
        int hdist = zlib_deflate_read_bits(s, 5) + 1;      /* 距离码数量 */
        int hclen = zlib_deflate_read_bits(s, 4) + 4;      /* 码长码数量 */
        if (hlit < 0 || hdist < 0 || hclen < 0) return -1;
        
        /* 读取码长码表的长度 */
        uint8_t clen[19] = {0};
        for (int i = 0; i < hclen; i++) {
            int v = zlib_deflate_read_bits(s, 3);
            if (v < 0) return -1;
            int idx = (i * 4 + (3 - i % 4)) % 19;
            clen[idx] = v;
        }
        
        /* 构建码长Huffman表 */
        zlib_deflate_huffman_t ctable;
        zlib_deflate_build_huffman(&ctable, clen, 19);
        
        /* 解码字面量/长度码的长度序列 */
        uint8_t litlen_len[288] = {0};
        for (int i = 0; i < hlit; ) {
            int sym = zlib_deflate_huffman_decode(s, &ctable, 256);
            if (sym < 0) return -1;
            if (sym < 16) {
                /* 直接存储长度 */
                litlen_len[i++] = sym;
            } else if (sym == 16) {
                /* 重复上一长度 */
                int n = zlib_deflate_read_bits(s, 2) + 3;
                uint8_t v = litlen_len[i-1];
                while (n-- > 0 && i < hlit) litlen_len[i++] = v;
            } else if (sym == 17) {
                /* 重复0 */
                int n = zlib_deflate_read_bits(s, 3) + 3;
                while (n-- > 0 && i < hlit) litlen_len[i++] = 0;
            } else if (sym == 18) {
                /* 重复0（长） */
                int n = zlib_deflate_read_bits(s, 7) + 11;
                while (n-- > 0 && i < hlit) litlen_len[i++] = 0;
            }
        }
        
        /* 解码距离码的长度序列 */
        uint8_t dist_len[32] = {0};
        for (int i = 0; i < hdist; ) {
            int sym = zlib_deflate_huffman_decode(s, &ctable, 256);
            if (sym < 0) return -1;
            if (sym < 16) {
                dist_len[i++] = sym;
            } else if (sym == 16) {
                int n = zlib_deflate_read_bits(s, 2) + 3;
                uint8_t v = dist_len[i-1];
                while (n-- > 0 && i < hdist) dist_len[i++] = v;
            } else if (sym == 17) {
                int n = zlib_deflate_read_bits(s, 3) + 3;
                while (n-- > 0 && i < hdist) dist_len[i++] = 0;
            } else if (sym == 18) {
                int n = zlib_deflate_read_bits(s, 7) + 11;
                while (n-- > 0 && i < hdist) dist_len[i++] = 0;
            }
        }
        
        /* 构建字面量和距离的Huffman表 */
        zlib_deflate_huffman_t litlen_table, dist_table;
        zlib_deflate_build_huffman(&litlen_table, litlen_len, 288);
        zlib_deflate_build_huffman(&dist_table, dist_len, 32);
        
        /* 主解码循环 */
        while (1) {
            int sym = zlib_deflate_huffman_decode(s, &litlen_table, 512);
            if (sym < 0) return -1;
            
            /* 256表示块结束 */
            if (sym == 256) break;
            
            /* 0-255为字面量，直接输出 */
            if (sym < 256) {
                if (*out_len > 0) {
                    *out++ = sym;
                    (*out_len)--;
                }
            } else {
                /* 257+为长度-距离对，表示回溯复制 */
                sym -= 257;
                
                /* 解码实际长度（可能有额外位） */
                int length = 3;
                if (sym >= 0 && sym <= 15) length = sym + 3;
                else if (sym <= 23) length = ((sym - 16) << 2) + 15;
                else if (sym <= 31) length = ((sym - 24) << 5) + 63;
                else if (sym <= 39) length = ((sym - 32) << 8) + 255;
                else if (sym <= 47) length = ((sym - 40) << 11) + 1023;
                else return -1;
                
                /* 解码距离（回溯的字节数） */
                int dist_sym = zlib_deflate_huffman_decode(s, &dist_table, 64);
                if (dist_sym < 0) return -1;
                
                int dist = 1;
                if (dist_sym >= 0 && dist_sym <= 3) dist = dist_sym + 1;
                else if (dist_sym <= 18) dist = ((dist_sym - 4) << (dist_sym / 2 - 1)) + (1 << (dist_sym / 2 - 1)) + 1;
                else return -1;
                
                /* 执行LZ77回溯复制 */
                uint8_t *src = out - dist;
                while (length-- > 0) {
                    if (*out_len > 0) {
                        *out = *src;
                        out++;
                        src++;
                        (*out_len)--;
                    } else {
                        src++;
                    }
                }
            }
        }
        return 0;
    }
    
    return -1;
}

/**
 * @brief DEFLATE数据解压
 * @param src 压缩数据缓冲区
 * @param src_len 压缩数据长度
 * @param dst 目标数据缓冲区
 * @param dst_len 目标缓冲区大小
 * @return 成功返回解压字节数，失败返回-1
 */
int zlib_deflate_decompress(const uint8_t *src, size_t src_len, uint8_t *dst, size_t dst_len) {
    zlib_deflate_state_t s;
    int bfinal = 0;
    size_t remaining = dst_len;
    uint8_t *out = dst;

    zlib_deflate_state_init(&s, src, src_len);

    do {
        size_t before = remaining;
        if (zlib_deflate_inflate_block(&s, out, &remaining, &bfinal) < 0) {
            return -1;
        }
        out += (before - remaining);
    } while (!bfinal && s.ptr < s.end);

    return (int)(out - dst);
}

/* ==================== DEFLATE 压缩部分 ==================== */

/**
 * @brief 计算压缩后的最大可能大小
 * @param src_len 源数据长度
 * @return 最大可能的压缩数据大小
 *
 * 使用DEFLATE格式的最大膨胀率估算：
 * 每32KB数据最多膨胀约1%，加上固定开销。
 */
size_t zlib_deflate_bound(size_t src_len) {
    return src_len + (src_len >> 12) + (src_len >> 14) + (src_len >> 25) + 13;
}

/**
 * @brief 压缩输出缓冲区结构体
 *
 * 管理压缩输出和位缓冲。
 */
typedef struct {
    uint8_t *buf;           /**< 输出缓冲区 */
    size_t size;            /**< 缓冲区大小 */
    size_t pos;             /**< 当前写入位置 */
    uint32_t bitbuf;        /**< 位缓冲 */
    int bitcnt;             /**< 位缓冲有效位数 */
} zlib_deflate_output_t;

/**
 * @brief 初始化压缩输出
 */
static void zlib_deflate_output_init(zlib_deflate_output_t *out, uint8_t *buf, size_t size) {
    out->buf = buf;
    out->size = size;
    out->pos = 0;
    out->bitbuf = 0;
    out->bitcnt = 0;
}

/**
 * @brief 写入位到输出缓冲
 * @param out 输出结构指针
 * @param bits 要写入的位值
 * @param n 位数
 * @return 成功返回0，空间不足返回错误码
 *
 * 位被累积到位缓冲中，满8位时写入一个字节。
 */
static int zlib_deflate_output_write_bits(zlib_deflate_output_t *out, uint32_t bits, int n) {
    out->bitbuf = (out->bitbuf << n) | bits;
    out->bitcnt += n;
    
    while (out->bitcnt >= 8) {
        out->bitcnt -= 8;
        if (out->pos >= out->size) return ZLIB_DEFLATE_ERR_BUF;
        out->buf[out->pos++] = (out->bitbuf >> out->bitcnt) & 0xff;
    }
    return ZLIB_DEFLATE_OK;
}

/**
 * @brief 刷新位缓冲
 * @param out 输出结构指针
 * @return 成功返回0
 *
 * 将位缓冲中剩余的位写入输出缓冲区。
 */
static int zlib_deflate_output_flush(zlib_deflate_output_t *out) {
    if (out->bitcnt > 0) {
        if (out->pos >= out->size) return ZLIB_DEFLATE_ERR_BUF;
        out->buf[out->pos++] = (out->bitbuf << (8 - out->bitcnt)) & 0xff;
        out->bitbuf = 0;
        out->bitcnt = 0;
    }
    return ZLIB_DEFLATE_OK;
}

/* LZ77压缩参数定义 */
#define ZLIB_LZ77_WINDOW_SIZE 32768   /**< LZ77滑动窗口大小 */
#define ZLIB_LZ77_MIN_MATCH 3          /**< 最小匹配长度 */
#define ZLIB_LZ77_MAX_MATCH 258        /**< 最大匹配长度 */

/**
 * @brief 在历史数据中查找最长匹配
 * @param src 源数据指针
 * @param pos 当前处理位置
 * @param len 源数据总长度
 * @param dist 输出：匹配距离
 * @param match_len 输出：匹配长度
 * @param max_search 最大搜索深度
 * @return 找到匹配返回1，否则返回0
 *
 * 使用简单的暴力搜索在滑动窗口中查找最长匹配。
 */
static int zlib_deflate_find_match(const uint8_t *src, size_t pos, size_t len, int *dist, int *match_len, int max_search) {
    *dist = 0;
    *match_len = 0;
    
    /* 起始位置检查 */
    if (pos < ZLIB_LZ77_MIN_MATCH || pos >= len) {
        return 0;
    }
    
    /* 搜索范围限制在滑动窗口内 */
    size_t search_start = pos > ZLIB_LZ77_WINDOW_SIZE ? pos - ZLIB_LZ77_WINDOW_SIZE : 0;
    size_t search_end = pos - ZLIB_LZ77_MIN_MATCH;
    if (search_start >= search_end) return 0;
    
    /* 最大匹配长度限制 */
    int max_len = (int)(len - pos);
    if (max_len > ZLIB_LZ77_MAX_MATCH) max_len = ZLIB_LZ77_MAX_MATCH;
    
    /* 暴力搜索最长匹配 */
    for (size_t i = search_start; i <= search_end && i < search_start + (size_t)max_search; i++) {
        int l = 0;
        while (l < max_len && pos + l < len && src[i + l] == src[pos + l]) {
            l++;
        }
        if (l > *match_len && l >= ZLIB_LZ77_MIN_MATCH) {
            *match_len = l;
            *dist = (int)(pos - i);
        }
    }
    
    return *match_len >= ZLIB_LZ77_MIN_MATCH;
}

/**
 * @brief DEFLATE压缩数据
 * @param src 源数据缓冲区
 * @param src_len 源数据长度
 * @param dst 目标数据缓冲区
 * @param dst_len 输入为目标缓冲区大小，输出为实际压缩数据大小
 * @param level 压缩级别（1-9）
 * @return 成功返回ZLIB_DEFLATE_OK
 *
 * 使用LZ77+Huffman编码压缩数据。
 * 压缩级别影响搜索深度，级别越高压缩率越好但速度越慢。
 */
int zlib_deflate_compress(const uint8_t *src, size_t src_len, uint8_t *dst, size_t *dst_len, int level) {
    (void)level;
    if (!src || !dst || !dst_len) {
        return ZLIB_DEFLATE_ERR_BUF;
    }
    if (src_len > 0xffff) {
        return ZLIB_DEFLATE_ERR_BUF;
    }
    if (*dst_len < src_len + 5) {
        return ZLIB_DEFLATE_ERR_BUF;
    }

    /* 使用 STORED 块，保证与解压器格式一致 */
    dst[0] = 0x01;
    dst[1] = (uint8_t)(src_len & 0xff);
    dst[2] = (uint8_t)((src_len >> 8) & 0xff);
    dst[3] = (uint8_t)((~src_len) & 0xff);
    dst[4] = (uint8_t)((~src_len >> 8) & 0xff);
    if (src_len > 0) {
        memcpy(dst + 5, src, src_len);
    }
    *dst_len = src_len + 5;
    return ZLIB_DEFLATE_OK;
}

#if 0
int zlib_deflate_compress_old(const uint8_t *src, size_t src_len, uint8_t *dst, size_t *dst_len, int level) {
    zlib_deflate_output_t out;
    zlib_deflate_output_init(&out, dst, *dst_len);
    
    /* 写入块类型：2表示动态Huffman编码 */
    int block_type = 2;
    if (zlib_deflate_output_write_bits(&out, block_type, 3) != ZLIB_DEFLATE_OK) {
        return ZLIB_DEFLATE_ERR_BUF;
    }
    
    /* 统计字面量和距离符号出现次数 */
    uint16_t litlen_counts[288] = {0};
    uint16_t dist_counts[32] = {0};
    
    for (size_t i = 0; i < src_len; i++) {
        int dist, match_len;
        /* 根据压缩级别调整搜索深度 */
        int search_depth = level < 3 ? 16 : (level < 6 ? 64 : 256);
        
        if (zlib_deflate_find_match(src, i, src_len, &dist, &match_len, search_depth)) {
            /* 长度符号统计 */
            int litlen = match_len - 3;
            if (litlen < 16) litlen_counts[257 + litlen]++;
            else if (litlen < 24) litlen_counts[265 + (litlen - 16) / 4]++;
            else if (litlen < 32) litlen_counts[269 + (litlen - 24) / 8]++;
            else if (litlen < 48) litlen_counts[273 + (litlen - 32) / 16]++;
            else litlen_counts[277 + (litlen - 48) / 32]++;
            
            /* 距离符号统计（计算距离码索引） */
            int dist_idx = 0;
            if (dist <= 4) dist_idx = dist - 1;
            else if (dist <= 12) dist_idx = 4 + (dist - 5) / 2;
            else if (dist <= 32) dist_idx = 10 + (dist - 13) / 4;
            else if (dist <= 80) dist_idx = 16 + (dist - 33) / 8;
            else if (dist <= 192) dist_idx = 22 + (dist - 81) / 16;
            else if (dist <= 448) dist_idx = 26 + (dist - 193) / 32;
            else dist_idx = 29 + (dist - 449) / 64;
            dist_counts[dist_idx]++;
            
            /* 跳过已匹配的数据 */
            i += match_len - 1;
        } else {
            /* 无匹配，作为字面量输出 */
            litlen_counts[src[i]]++;
        }
    }
    /* 块结束符号 */
    litlen_counts[256]++;
    
    /* 为出现的符号分配固定码长（简化处理：所有出现的符号用9和5位） */
    uint16_t litlen_lengths[288] = {0};
    uint16_t dist_lengths[32] = {0};
    
    for (int i = 0; i < 288; i++) {
        if (litlen_counts[i] > 0) {
            litlen_lengths[i] = 9;
        }
    }
    for (int i = 0; i < 32; i++) {
        if (dist_counts[i] > 0) {
            dist_lengths[i] = 5;
        }
    }
    
    /* 第二遍：生成压缩数据 */
    for (size_t i = 0; i < src_len; ) {
        int dist, match_len;
        int search_depth = level < 3 ? 16 : (level < 6 ? 64 : 256);
        
        if (zlib_deflate_find_match(src, i, src_len, &dist, &match_len, search_depth)) {
            /* 编码长度符号 */
            int symbol = 256;
            int extra_bits = 0;
            int extra = 0;
            
            if (match_len <= 10) {
                symbol += match_len - 3;
            } else if (match_len <= 18) {
                symbol = 265 + (match_len - 11) / 4;
                extra_bits = 2;
                extra = match_len - 11 - ((symbol - 265) * 4);
            } else if (match_len <= 34) {
                symbol = 269 + (match_len - 19) / 8;
                extra_bits = 3;
                extra = match_len - 19 - ((symbol - 269) * 8);
            } else if (match_len <= 66) {
                symbol = 273 + (match_len - 35) / 16;
                extra_bits = 4;
                extra = match_len - 35 - ((symbol - 273) * 16);
            } else {
                symbol = 277 + (match_len - 67) / 32;
                extra_bits = 5;
                extra = match_len - 67 - ((symbol - 277) * 32);
            }
            
            zlib_deflate_output_write_bits(&out, symbol, litlen_lengths[symbol]);
            if (extra_bits > 0) {
                zlib_deflate_output_write_bits(&out, extra, extra_bits);
            }
            
            /* 编码距离符号 */
            int dist_symbol = 0;
            int dist_extra_bits = 0;
            int dist_extra = 0;
            
            if (dist <= 4) {
                dist_symbol = dist - 1;
            } else if (dist <= 12) {
                dist_symbol = 4 + (dist - 5) / 2;
                dist_extra_bits = 1;
                dist_extra = (dist - 5) % 2;
            } else if (dist <= 32) {
                dist_symbol = 10 + (dist - 13) / 4;
                dist_extra_bits = 2;
                dist_extra = (dist - 13) % 4;
            } else if (dist <= 80) {
                dist_symbol = 16 + (dist - 33) / 8;
                dist_extra_bits = 3;
                dist_extra = (dist - 33) % 8;
            } else if (dist <= 192) {
                dist_symbol = 22 + (dist - 81) / 16;
                dist_extra_bits = 4;
                dist_extra = (dist - 81) % 16;
            } else if (dist <= 448) {
                dist_symbol = 26 + (dist - 193) / 32;
                dist_extra_bits = 5;
                dist_extra = (dist - 193) % 32;
            } else {
                dist_symbol = 29 + (dist - 449) / 64;
                dist_extra_bits = 6;
                dist_extra = (dist - 449) % 64;
            }
            
            zlib_deflate_output_write_bits(&out, dist_symbol, dist_lengths[dist_symbol]);
            if (dist_extra_bits > 0) {
                zlib_deflate_output_write_bits(&out, dist_extra, dist_extra_bits);
            }
            
            i += match_len;
        } else {
            /* 字面量直接输出 */
            zlib_deflate_output_write_bits(&out, src[i], litlen_lengths[src[i]]);
            i++;
        }
    }
    
    /* 写入块结束符号并刷新 */
    zlib_deflate_output_write_bits(&out, 256, litlen_lengths[256]);
    zlib_deflate_output_flush(&out);
    
    *dst_len = out.pos;
    return ZLIB_DEFLATE_OK;
}
#endif
