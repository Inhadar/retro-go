// Target definition
#define RG_TARGET_NAME             "ESP32-S3-MUX"

// ---------------------------------------------------------------------------
// PIN OZETI (ESP32-S3)
//   Ortak SPI hatti : MOSI=11  SCK=12  MISO=13
//   Ekran           : CS=10  DC=5  RST=4  (LED -> 3.3V, yazilimdan kontrol yok)
//   SD kart         : CS=14
//   Mux CD74HC4067  : SIG=1  S0=40  S1=41  S2=42  S3=2
//   Ses (I2S DAC)   : BCK=15  WS=16  DATA=17  (DAC baglayinca)
// ---------------------------------------------------------------------------

// Storage
// SD kart ekranla AYNI SPI hattini kullaniyor, bu yuzden ikisi de SPI2_HOST
#define RG_STORAGE_ROOT             "/sd"
#define RG_STORAGE_SDSPI_HOST       SPI2_HOST
#define RG_STORAGE_SDSPI_SPEED      SDMMC_FREQ_DEFAULT
// #define RG_STORAGE_SDMMC_HOST       SDMMC_HOST_SLOT_1
// #define RG_STORAGE_SDMMC_SPEED      SDMMC_FREQ_DEFAULT
// #define RG_STORAGE_FLASH_PARTITION  "vfs"

// Audio
#define RG_AUDIO_USE_INT_DAC        0   // 0 = Disable, 1 = GPIO25, 2 = GPIO26, 3 = Both
#define RG_AUDIO_USE_EXT_DAC        1   // 0 = Disable, 1 = Enable

// Video
#define RG_SCREEN_DRIVER            0   // 0 = ILI9341/ST7789 (ayni surucu, sadece init dizisi farkli)
#define RG_SCREEN_HOST              SPI2_HOST
#define RG_SCREEN_SPEED             SPI_MASTER_FREQ_40M // ST7789 icin 40M guvenli. 80M cipin resmi siniri ustunde
#define RG_SCREEN_BACKLIGHT         1
#define RG_SCREEN_WIDTH             320
#define RG_SCREEN_HEIGHT            240
#define RG_SCREEN_ROTATE            0
#define RG_SCREEN_VISIBLE_AREA      {0, 0, 0, 0}
#define RG_SCREEN_SAFE_AREA         {0, 0, 0, 0}
#define RG_SCREEN_INIT()                                                                                      \
    ILI9341_CMD(0x36, 0x60);                 /* MADCTL: yatay (MX|MV), RGB sirasi */                            \
    ILI9341_CMD(0x3A, 0x55);                 /* 16 bit renk (RGB565) */                                         \
    ILI9341_CMD(0xB2, 0x0C, 0x0C, 0x00, 0x33, 0x33); /* Porch ayari */                                         \
    ILI9341_CMD(0xB7, 0x14);                 /* Gate control */                                                 \
    ILI9341_CMD(0xBB, 0x37);                 /* VCOM */                                                         \
    ILI9341_CMD(0xC0, 0x2C);                 /* LCM control */                                                  \
    ILI9341_CMD(0xC2, 0x01);                 /* VDV ve VRH komutlarini etkinlestir */                           \
    ILI9341_CMD(0xC3, 0x12);                 /* VRH */                                                          \
    ILI9341_CMD(0xC4, 0x20);                 /* VDV */                                                          \
    ILI9341_CMD(0xC6, 0x0F);                 /* Kare hizi 60 Hz */                                              \
    ILI9341_CMD(0xD0, 0xA4, 0xA1);           /* Power control */                                                \
    ILI9341_CMD(0xE0, 0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23);     \
    ILI9341_CMD(0xE1, 0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23);     \
    ILI9341_CMD(0x20);                       /* Inversion ON (IPS ST7789 paneller icin gerekli; renkler negatifse 0x20 yap) */


// Input
// Tuslar CD74HC4067 coklayicisindan okunuyor.
// ADC ve GPIO haritalari kaldirildi (eski pinleri ekran/mux ile cakisiyordu).
// NOT: Mux okuma kodu rg_input.c icine ayrica eklenmeli, o yama yapilana kadar tuslar calismaz.

// Mux pinleri (S0 = en dusuk bit)
#define RG_GPIO_MUX_S0              GPIO_NUM_40
#define RG_GPIO_MUX_S1              GPIO_NUM_41
#define RG_GPIO_MUX_S2              GPIO_NUM_42
#define RG_GPIO_MUX_S3              GPIO_NUM_2
#define RG_GPIO_MUX_SIG             GPIO_NUM_1

#define RG_GAMEPAD_MUX_MAP {            \
    {RG_KEY_UP,    6},                  \
    {RG_KEY_RIGHT, 7},                  \
    {RG_KEY_DOWN,  10},                 \
    {RG_KEY_LEFT,  9},                  \
    {RG_KEY_A,     4},                  \
    {RG_KEY_B,     1},                  \
    {RG_KEY_L,     8},                  \
    {RG_KEY_R,     5},                  \
}

// Battery (kapali)
// Eski ayar ADC_CHANNEL_3 kullaniyordu, bu S3'te GPIO4 = ekran RST pini. Cakisma oldugu icin kapatildi.
// #define RG_BATTERY_DRIVER           1
// #define RG_BATTERY_ADC_UNIT         ADC_UNIT_1
// #define RG_BATTERY_ADC_CHANNEL      ADC_CHANNEL_3
// #define RG_BATTERY_CALC_PERCENT(raw) (((raw) * 2.f - 3500.f) / (4200.f - 3500.f) * 100.f)
// #define RG_BATTERY_CALC_VOLTAGE(raw) ((raw) * 2.f * 0.001f)
#define RG_GAMEPAD_VIRT_MAP {                         \
    {RG_KEY_SELECT, RG_KEY_B | RG_KEY_A},             \
    {RG_KEY_START,  RG_KEY_R | RG_KEY_A},             \
    {RG_KEY_MENU,   RG_KEY_L | RG_KEY_R},             \
    {RG_KEY_OPTION, RG_KEY_L | RG_KEY_A},             \
}

// Status LED (bagli degilse zararsiz)
#define RG_GPIO_LED                 GPIO_NUM_38

// SPI Display
// MISO, SD ile ayni pin (13). Ortak bus MISO ile baslatilmali ki SD calissin.
// Ekran modulunun SDO ucunu FIZIKSEL olarak baglama (Retro-Go ekrandan veri okumaz).
#define RG_GPIO_LCD_MISO            GPIO_NUM_13
#define RG_GPIO_LCD_MOSI            GPIO_NUM_11
#define RG_GPIO_LCD_CLK             GPIO_NUM_12
#define RG_GPIO_LCD_CS              GPIO_NUM_10
#define RG_GPIO_LCD_DC              GPIO_NUM_5
#define RG_GPIO_LCD_BCKL            GPIO_NUM_39   // bagli degil, LED 3.3V'a sabit. Parlaklik menusu calismaz.
#define RG_GPIO_LCD_RST             GPIO_NUM_4

// SPI SD Card (ekranla ortak hat)
#define RG_GPIO_SDSPI_MISO          GPIO_NUM_13
#define RG_GPIO_SDSPI_MOSI          GPIO_NUM_11
#define RG_GPIO_SDSPI_CLK           GPIO_NUM_12
#define RG_GPIO_SDSPI_CS            GPIO_NUM_14

// External I2S DAC
// Eski pinler 40/41/42 mux ile cakisiyordu, 15/16/17'ye tasindi.
#define RG_GPIO_SND_I2S_BCK         15
#define RG_GPIO_SND_I2S_WS          16
#define RG_GPIO_SND_I2S_DATA        17
// #define RG_GPIO_SND_AMP_ENABLE      18
