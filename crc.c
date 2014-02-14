/**
 * \file    crc.c
 * \version 1.1
 */

#include <stdio.h>
#include <stdint.h>
#include <getopt.h>

static uint16_t crc16_table[256];
static uint32_t crc32_table[256];

/**
 * \brief Loads the polynominal table for crc16
 */
static void init_crc16() {
    for (int i = 0; i < 256; i++) {
        uint16_t crc = 0;
        uint16_t c = i;
        for (int j = 0; j < 8; j++) {
            if ((crc ^ c) & 0x1)
                crc = (crc >> 1) ^ 0xA001; // Polynomial
            else
                crc = crc >> 1;
            c = c >> 1;
        }
        crc16_table[i] = crc;
    }
}

/**
 * \brief Loads the polynominal table for crc32
 */
static void init_crc32() {
    for (int i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            if (crc & 0x1)
                crc = (crc >> 1) ^ 0xEDB88320; // Polynomial
            else
                crc = crc >> 1;
        }
        crc32_table[i] = crc;
    }
}

/**
 * \brief  Computates the crc16 of a character
 * \param  crc Previous crc16 as a starting value
 * \param  c   Character to apply the crc16 function
 * \return the crc16 result
 */
uint16_t crc16(uint16_t crc, uint8_t c) {
    uint16_t tmp = crc ^ c;
    return (crc >> 8) ^ crc16_table[tmp & 0xFF];
}

/**
 * \brief  Computates the crc32 of a character
 * \param  crc Previous crc32 as a starting value
 * \param  c   Character to apply the crc32 function
 * \return the crc32 result
 */
uint32_t crc32(uint32_t crc, uint8_t c) {
    uint32_t tmp = crc ^ c;
    return (crc >> 8) ^ crc32_table[tmp & 0xFF];
}

void print_usage() {
    printf("Usage: crc [-a|--16|--32] file [file2] ...\n"
           "\n"
           "   -a    Use every type of CRC\n"
           "   --16  Use CRC16\n"
           "   --32  Use CRC32\n");
}

void calc(char* name, int do_crc16, int do_crc32) {
    FILE* fp = fopen(name, "rb");

    if (fp != NULL) {
        int ch;
        uint16_t crc16out = 0;
        uint32_t crc32out = 0xFFFFFFFF;

        printf("\n%s\n", name);

        while ((ch = fgetc(fp)) != EOF) {
            if (do_crc16)
                crc16out = crc16(crc16out, ch);
            if (do_crc32)
                crc32out = crc32(crc32out, ch);
        }

        fclose(fp);
        
        crc32out ^= 0xFFFFFFFF;
        if (do_crc16)
            printf("CRC16 - %04X\n", crc16out);
        if (do_crc32)
            printf("CRC32 - %08lX\n", crc32out);
    } else {
        printf("\nCannot open file \"%s\"\n", name);
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    int do16 = 0, do32 = 0;

    const struct option options[] = {
        {"16", optional_argument, &do16, 1},
        {"32", optional_argument, &do32, 1}
    };

    int c, index, no_args = 1;
    while ((c = getopt_long(argc, argv, "a", options, &index)) != -1) {
        switch (c) {
        case 0:
            no_args = 0;
            break;
        case 'a':
            do16 = 1;
            do32 = 1;
            no_args = 0;
            break;
        case '?':
            print_usage();
            return 1;
        default:
            return 1;
        }
    }

    if (optind >= argc) {
        print_usage();
        return 1;
    }

    if (no_args)
        do32 = 1;

    if (do16)
        init_crc16();
    if (do32)
        init_crc32();

    for (int i = optind; i < argc; i++) {
        calc(argv[i], do16, do32);
    }

    return 0;
}
