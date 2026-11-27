#include "rg_system.h"
#include "rg_audio.h"
#include "driver/i2s_pdm.h"
#include <string.h>

#ifndef RG_AUDIO_PDM_PIN
#ifdef RG_GPIO_SND_I2S_DATA
#define RG_AUDIO_PDM_PIN RG_GPIO_SND_I2S_DATA
#else
#define RG_AUDIO_PDM_PIN 17
#endif
#endif

static i2s_chan_handle_t s_pdm_tx_chan = NULL;
static int s_volume = 50;
static bool s_muted = false;
static int s_sample_rate = 22050;
static const char *s_last_error = NULL;

static bool pdm_init(int device, int sample_rate)
{
    (void)device;
    s_last_error = NULL;
    s_sample_rate = (sample_rate > 0) ? sample_rate : 22050;

    if (s_pdm_tx_chan)
    {
        return true;
    }

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    esp_err_t err = i2s_new_channel(&chan_cfg, &s_pdm_tx_chan, NULL);
    if (err != ESP_OK)
    {
        s_last_error = "i2s_new_channel failed";
        s_pdm_tx_chan = NULL;
        return false;
    }

    i2s_pdm_tx_config_t pdm_cfg = {
        .clk_cfg = I2S_PDM_TX_CLK_DEFAULT_CONFIG((uint32_t)s_sample_rate),
        .slot_cfg = I2S_PDM_TX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .clk = -1,
            .dout = (gpio_num_t)RG_AUDIO_PDM_PIN,
            .invert_flags = {
                .clk_inv = false,
            },
        },
    };

    err = i2s_channel_init_pdm_tx_mode(s_pdm_tx_chan, &pdm_cfg);
    if (err != ESP_OK)
    {
        s_last_error = "i2s_channel_init_pdm_tx_mode failed";
        i2s_del_channel(s_pdm_tx_chan);
        s_pdm_tx_chan = NULL;
        return false;
    }

    err = i2s_channel_enable(s_pdm_tx_chan);
    if (err != ESP_OK)
    {
        s_last_error = "i2s_channel_enable failed";
        i2s_del_channel(s_pdm_tx_chan);
        s_pdm_tx_chan = NULL;
        return false;
    }

    return true;
}

static bool pdm_deinit(void)
{
    if (s_pdm_tx_chan)
    {
        i2s_channel_disable(s_pdm_tx_chan);
        i2s_del_channel(s_pdm_tx_chan);
        s_pdm_tx_chan = NULL;
    }
    return true;
}

static void pdm_submit(const rg_audio_frame_t *frames, size_t count)
{
    if (!s_pdm_tx_chan || !frames || count == 0)
    {
        return;
    }

    int16_t pcm_buf[256];
    size_t idx = 0;

    for (size_t i = 0; i < count; ++i)
    {
        int32_t sample = ((int32_t)frames[i].left + (int32_t)frames[i].right) / 2;
        if (s_muted || s_volume <= 0)
        {
            sample = 0;
        }
        else
        {
            sample = (sample * s_volume) / 100;
        }

        if (sample > 32767)
        {
            sample = 32767;
        }
        else if (sample < -32768)
        {
            sample = -32768;
        }

        pcm_buf[idx++] = (int16_t)sample;

        if (idx == RG_COUNT(pcm_buf))
        {
            size_t bytes_written = 0;
            i2s_channel_write(s_pdm_tx_chan, pcm_buf, sizeof(pcm_buf), &bytes_written, portMAX_DELAY);
            idx = 0;
        }
    }

    if (idx > 0)
    {
        size_t bytes_written = 0;
        i2s_channel_write(s_pdm_tx_chan, pcm_buf, idx * sizeof(int16_t), &bytes_written, portMAX_DELAY);
    }
}

static void pdm_set_volume(int percent)
{
    s_volume = RG_MIN(RG_MAX(percent, 0), 100);
}

static void pdm_set_mute(bool mute)
{
    s_muted = mute;
}

static const char *pdm_get_error(void)
{
    return s_last_error ? s_last_error : "OK";
}

const rg_audio_driver_t rg_audio_driver_pdm = {
    .name = "PDM",
    .init = pdm_init,
    .deinit = pdm_deinit,
    .submit = pdm_submit,
    .set_volume = pdm_set_volume,
    .set_mute = pdm_set_mute,
    .get_error = pdm_get_error,
};
