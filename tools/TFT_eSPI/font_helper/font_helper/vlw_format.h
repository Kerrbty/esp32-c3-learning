#pragma once
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;

// vlw ͷ���ṹ 
typedef struct _vlw_header{
    uint32_t   font_count;  // �ַ����� 
    uint32_t   vlw_version; // vlw �ļ���ʽ�汾�� 
    uint32_t   font_size;   // �����С 
    uint32_t   reserved;
    uint32_t   ascent;      // �����߶� 
    uint32_t   descent;     // �½��߶� 
}vlw_header, *pvlw_header;

// vlw ������Ϣ���� 
typedef struct _vlw_font_info{
    uint32_t  uncode_data;  // �ַ� utf16 ���� 
    uint32_t  Height;       // ����߶� 
    uint32_t  Width;        // ������ 
    uint32_t  xAdvance;     // �ƶ� x ��� 
    uint32_t  dy;           // y������ڻ��ߵ����� 
    uint32_t  dx;           // x������ڻ��ߵ����� 
    uint32_t  ignore;       // δʹ�� 
}vlw_font_info, *pvlw_font_info;

// vlw ����λͼ�ṹ 
typedef struct _vlw_bitmap{
    uint8_t  bit[0];
}vlw_bitmap, *pvlw_bitmap;

// ����vlw�ṹ 
typedef struct _vlw_format 
{
    vlw_header header;
    vlw_font_info fonts[0];
    vlw_bitmap bits[0];
}vlw_format, *pvlw_format;