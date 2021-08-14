#include <stdint.h>
#include <stdio.h>
#include <string.h>


int main()
{


    uint32_t flag = 0x01020304;
    printf("flag:%x\n",flag);

    uint8_t flag_array[4] = {0};

    uint8_t ch_8 = 0;

    memcpy(flag_array, &flag, sizeof(flag_array));
    printf("flag_array:4:%02x %02x %02x %02x\n", flag_array[0],flag_array[1],flag_array[2],flag_array[3]);

    ch_8 = flag;
    printf("ch_8:%02x\n", ch_8);

    uint8_t *flag2 = &flag;

    printf("flag2 ptr:%02x\n", *flag2);


    return 0;

}
