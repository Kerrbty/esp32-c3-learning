#pragma once
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;

// vlw 头部结构 
typedef struct _vlw_header{
    uint32_t   font_count;  // 字符个数 
    uint32_t   vlw_version; // vlw 文件格式版本号 
    uint32_t   font_size;   // 字体大小 
    uint32_t   reserved;
    uint32_t   ascent;      // 上升高度 
    uint32_t   descent;     // 下降高度 
}vlw_header, *pvlw_header;

// vlw 字体信息部分 
typedef struct _vlw_font_info{
    uint32_t  uncode_data;  // 字符 utf16 编码 
    uint32_t  Height;       // 字体高度 
    uint32_t  Width;        // 字体宽度 
    uint32_t  xAdvance;     // 移动 x 光标 
    uint32_t  dy;           // y轴相对于基线的增量 
    uint32_t  dx;           // x轴相对于基线的增量 
    uint32_t  ignore;       // 未使用 
}vlw_font_info, *pvlw_font_info;

// vlw 字体位图结构 
typedef struct _vlw_bitmap{
    uint8_t  bit[0];
}vlw_bitmap, *pvlw_bitmap;

// 整个vlw结构 
typedef struct _vlw_format 
{
    vlw_header header;
    vlw_font_info fonts[0];
    vlw_bitmap bits[0];
}vlw_format, *pvlw_format;