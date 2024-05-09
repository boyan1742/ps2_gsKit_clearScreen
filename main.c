#include <sys/unistd.h>
#include <stdio.h>
#include <kernel.h>
#include <time.h>

#include "external/gsKit/include/gsKit.h"
#include "external/gsKit/include/dmaKit.h"

GSGLOBAL* settings = NULL;
uint8_t red = 0;
uint8_t green = 0;
uint8_t blue = 0;

void InitScreenSettings() {
    settings = gsKit_init_global();
    settings->PrimAlphaEnable = GS_SETTING_ON;

    dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
                D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
    dmaKit_chan_init(DMA_CHANNEL_GIF);

    gsKit_set_primalpha(settings, GS_SETREG_ALPHA(0, 1, 0, 1, 0), 0);

    gsKit_set_clamp(settings, GS_CMODE_REPEAT);
    gsKit_vram_clear(settings);
    gsKit_init_screen(settings);
    gsKit_mode_switch(settings, GS_ONESHOT);
}

void flipScreen(GSGLOBAL* gsGlobal)
{
    gsKit_queue_exec(gsGlobal);
    gsKit_sync_flip(gsGlobal);
}

int main(void)
{
    InitScreenSettings();

    // A struct for use in the `nanosleep` function.
    struct timespec tc;
    tc.tv_nsec = 500;
    tc.tv_sec = 0;

    // This array holds the state of each part of the color: 0 = red, 1 = green, 2 = blue.
    bool colorStates[3];
    colorStates[0] = colorStates[1] = colorStates[2] = false;

    while(1) {
        printf("Sleeping for 1 second!\n");
        nanosleep(&tc, NULL);

        if (!colorStates[0])
        {
            red++;
            if (red == 255)
                colorStates[0] = true;

        }
        else
        {
            if (!colorStates[1])
            {
                green++;
                red--;

                if (green == 255)
                    colorStates[1] = true;

            }
            else
            {
                if (!colorStates[2])
                {
                    blue++;
                    green--;

                    if (blue == 255)
                        colorStates[2] = true;

                }

                if (colorStates[0] && colorStates[1] && colorStates[2])
                {
                    colorStates[0] = colorStates[1] = colorStates[2] = false;
                    red = green = blue = 0;
                }
            }
        }

        printf("Red: %d, Green: %d, Blue: %d\n", red, green, blue);
        printf("Red State: %d, Green State: %d, Blue State: %d\n", colorStates[0], colorStates[1], colorStates[2]);

        gsKit_clear(settings, GS_SETREG_RGBAQ(red, green, blue, 0x80, 0x00));

        //Drawing function goes here!

        flipScreen(settings);
    }

    return 0;
}
