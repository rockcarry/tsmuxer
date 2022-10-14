#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "tsmuxer.h"

#define ALIGN(a, b)  (((a) + (b) - 1) & ~((b) - 1))
#define MAX(a, b)    ((a) > (b) ? (a) : (b))
#define MIN(a, b)    ((a) < (b) ? (a) : (b))
#ifndef ARRAYSIZE
#define ARRAYSIZE(a) (sizeof((a)) / sizeof((a)[0]))
#endif

#define offset_of(type, member) ((size_t)&((type*)0)->member)
#define container_of(ptr, type, member) (type*)((char*)(ptr) - offset_of(type, member))

#pragma pack(1)
typedef struct {
    uint32_t sync_byte                      : 8;
    uint32_t transport_error_indicator      : 1;
    uint32_t payload_unit_start_indicator   : 1;
    uint32_t transport_priority             : 1;
    uint32_t pid                            : 13;
    uint32_t transport_scrambling_control   : 2;
    uint32_t adaptation_field_control       : 2;
    uint32_t continuity_counter             : 4;
} ts_header_t;

typedef struct {
    uint32_t table_id                       : 8;
    uint32_t section_syntax_indicator       : 1;
    uint32_t zero                           : 1;
    uint32_t reserved_1                     : 2;
    uint32_t section_length                 : 12;
    uint32_t transport_stream_id            : 16;
    uint32_t reserved_2                     : 2;
    uint32_t version_number                 : 5;
    uint32_t current_next_indicator         : 1;
    uint32_t section_number                 : 8;
    uint32_t last_section_number            : 8;

    uint32_t program_number                 : 16;
    uint32_t reserved_3                     : 3;
    uint32_t pid                            : 13;

    uint32_t crc32;
} ts_pat_t;

typedef struct {
    uint32_t table_id                       : 8;
    uint32_t section_syntax_indicator       : 1;
    uint32_t zero                           : 1;
    uint32_t reserved_1                     : 2;
    uint32_t section_length                 : 12;
    uint32_t program_number                 : 16;
    uint32_t reserved_2                     : 2;
    uint32_t version_number                 : 5;
    uint32_t current_next_indicator         : 1;
    uint32_t section_number                 : 8;
    uint32_t last_section_number            : 8;
    uint32_t reserved_3                     : 3;
    uint32_t pcr_pid                        : 13;
    uint32_t reserved_4                     : 4;
    uint32_t program_info_length            : 12;

    uint8_t  audio_stream_data[5];
    uint8_t  video_stream_data[5];

    uint32_t crc32;
} ts_pmt_t;
#pragma pack()

#if 0
typedef struct {
    uint8_t  pes_start_code[3];
    uint8_t  stream_id;
    uint16_t pes_packet_length;
    uint16_t flags;
    uint8_t  pes_data_length
} ts_pes_t;

static void write_pat(FILE *fp)
{
    ts_header_t header = {};
    ts_pat_t    pat    = {};

    header.sync_byte                    = 0x47;
    header.payload_unit_start_indicator = 1;
    header.adaptation_field_control     = 0x01;

    pat.section_syntax_indicator = 1;
    pat.reserved_1               = 3;
    pat.section_length           = sizeof(ts_pat_t) - offset_of(ts_pat_t, section_length);
    pat.transport_stream_id      = 0x0001;
    pat.reserved_2               = 3;
    pat.version_number           = 0;
    pat.current_next_indicator   = 1;
    pat.program_number           = 0x0001;
    pat.reserved_3               = 7;
    pat.pid                      = 0;
    pat.crc32                    = crc32(&pat, sizeof(pat);
    fwrite(&pat, 1, sizeof(pat), fp);
}

static void write_pmt(FILE *fp)
{
    ts_header_t header = {};
    ts_pmt_t    pmt    = {};

    header.sync_byte                    = 0x47;
    header.payload_unit_start_indicator = 1;
    header.pid                          = 0x1000;
    header.adaptation_field_control     = 0x01;

    pmt.table_id                        = 0x02;
    pmt.section_syntax_indicator        = 1;
    pmt.reserved_1                      = 3;
    pmt.section_length                  = sizeof(ts_pmt_t) - offset_of(ts_pmt_t, section_length);
    pmt.program_number                  = 0x0001;
    pmt.reserved_2                      = 7;
    pmt.current_next_indicator          = 1;
    pmt.reserved_3                      = 7;
    pmt.pcr_pid                         = 0x1fff;
    pmt.reserved_4                      = 0xf;
    fwrite(&pat, 1, sizeof(pat), fp);
}

static void write_frame(FILE *fp, char *type, uint8_t *buf, int len)
{
}
#endif


void dump_ts_header(ts_header_t *header)
{
    printf("sync_byte                   : %02X\n", header->sync_byte);
    printf("transport_error_indicator   : %d\n"  , header->transport_error_indicator);
    printf("payload_unit_start_indicator: %d\n"  , header->payload_unit_start_indicator);
    printf("transport_priority          : %d\n"  , header->transport_priority);
    printf("pid                         : %02X\n", header->pid);
    printf("transport_scrambling_control: %d\n"  , header->transport_scrambling_control);
    printf("adaptation_field_control    : %d\n"  , header->adaptation_field_control);
    printf("continuity_counter          : %d\n"  , header->continuity_counter);
}

void dump_ts_pat(ts_pat_t *pat)
{
    printf("sync_byte                   : %02X\n", header->sync_byte);
    printf("transport_error_indicator   : %d\n"  , header->transport_error_indicator);
    printf("payload_unit_start_indicator: %d\n"  , header->payload_unit_start_indicator);
    printf("transport_priority          : %d\n"  , header->transport_priority);
    printf("pid                         : %02X\n", header->pid);
    printf("transport_scrambling_control: %d\n"  , header->transport_scrambling_control);
    printf("adaptation_field_control    : %d\n"  , header->adaptation_field_control);
    printf("continuity_counter          : %d\n"  , header->continuity_counter);
}

int main(void)
{
    uint8_t buf[188];
    FILE *fp = fopen("test-0.ts", "rb");
    ts_header_t *header = NULL;
    ts_pat_t    *pat    = NULL;
    if (fp) {
        fread(buf, 1, sizeof(buf), fp);
        header = (ts_header_t*)buf;
        pat    = (ts_pat_t*)(header + 1)
        dump_ts_header(header);
        printf("\n");
        fread(buf, 1, sizeof(buf), fp);
        header = (ts_header_t*)buf;
        dump_ts_header(header);
        fclose(fp);
    }
    return 0;
}
