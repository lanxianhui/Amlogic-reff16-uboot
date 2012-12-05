#include <asm/arch/firm/regs.h>
#include <asm/arch/firm/reg_addr.h>

#include <asm/arch/firm/timing.h>
#include <asm/arch/firm/uart.h>
#include <memtest.h>
#include <config.h>
#include <ddr_types.h>
#ifndef FIRMWARE_IN_ONE_FILE
#define STATIC_PREFIX_DATA
#else
#define STATIC_PREFIX_DATA static
#endif
static int init_pctl_ddr3(struct ddr_set * ddr_setting);

#if defined(M2C_DDR3_1GB)
	#define DDR3_4Gbx16
#elif defined(M2C_DDR3_512M)
	#define DDR3_2Gbx16
#else
	#error "Please define DDR3 memory capacity first in file aml_aio_m2c_haier.h!\n"
#endif


#ifdef DDR3_2Gbx16
	//row_size 00 : A0~A15.  01 : A0~A12, 10 : A0~A13, 11 : A0~A14. 
	#define ddr3_row_size 2 
	//col size 00 : A0~A7,   01 : A0~A8, 10: A0 ~A9.  11, A0~A9, A11. 
	#define ddr3_col_size 2
#elif defined DDR3_4Gbx16
	//row_size 00 : A0~A15.  01 : A0~A12, 10 : A0~A13, 11 : A0~A14. 
	#define ddr3_row_size 3
	//col size 00 : A0~A7,   01 : A0~A8, 10: A0 ~A9.  11, A0~A9, A11. 
	#define ddr3_col_size 2
#endif

//move following setting to following file
//trunk/include/configs/amlogic/aml_aio_m2c_haier.h

//#define DDR3_9_9_9
//#define DDR3_7_7_7 
//533~400 Elpida
//#define DDR3_533_SAMSUNG
//#define DDR3_533
//#define SAMSUNG
//#define Elpida
static struct ddr_set __ddr_setting={
#ifdef	DDR3_533
                    .cl             =   8,
                    .t_faw          =  30,
                    .t_mrd          =   4,
                    .t_1us_pck      = 133,//533,
                    .t_100ns_pck    =  13,//53,
                    .t_init_us      = 511,
                    .t_ras          =  24,//20,
                    .t_rc           =  33,//28,
                    .t_rcd          =   8,
                    .t_refi_100ns   =  39,
                    .t_rfc          = 110,
                    .t_rp           =   8,
                    .t_rrd          =   5,
                    .t_rtp          =   5,
                    .t_wr           =   10,//8,
                    .t_wtr          =   5,
                    .t_xp           =   5,//4,
                    .t_xsrd         =   0,   // init to 0 so that if only one of them is defined, this is chosen
                    .t_xsnr         =   0,
                    .t_exsr         = 512,
                    .t_al           =   0,   // Additive Latency
                    .t_clr          =   8,   // cas_latency for DDR2 (nclk cycles)
                    .t_dqs          =   2,   // distance between data phases to different ranks
                    .t_cwl          =   6,
                    .t_mod          =   12,//8,
                    .t_zqcl         = 512,
                    .t_rtw          =   6,
                    .t_cksrx        =   7,
                    .t_cksre        =   7,
                    .t_cke          =   4,
                    .mrs={  [0]=0,
                            [1]=(0<<6)|(1<<2)| //rtt_nominal;      //(A9, A6, A2) 000 : disabled. 001 : RZQ/4   010:RZQ/2 (A6:A2)
#ifdef SAMSUNG                            
                                (0<<5)|(0<<1)| //(A5 A1),Output driver impedance control 00:RZQ/6,01:RZQ/7,10£ºRZQ/5 11:Reserved
#endif
#ifdef Elpida
                                (0<<5)|(1<<1)| //(A5 A1),Output driver impedance control 00:RZQ/6,01:RZQ/7,10£ºRZQ/5 11:Reserved
#endif                                
                                (0<<12)|(0<<7),
                            [2]=0,
                            [3]=0    
                        },
                    .mcfg = {  1 |              // burst length 0 = 4; 1 = 8
                                (0 << 2) |      // bl8int_en.   enable bl8 interrupt function.
                                //(1 << 3) |        //2T mode, default is disable
                                (1 << 5) |      // 1: ddr3 protocal; 0 : ddr2 protocal
                                //(tFAW <<18) | //tFAW will be set according to the calculation with t_rrd and t_faw
                                                // in file /firmware/ddr_init_pctl.c
                                (1 << 17) |     // power down exit which fast exit.
                                (0xf << 8)      // 0xf cycle empty will entry power down mode.
                             },
                     .zqcr  = (( 1 << 24) | 0x11dd),   //0x11dd->22 ohm;0x1155->0 ohm
                    //.ddr_pll_cntl=0x00110685,
                    //.ddr_clk=532, 
                    //.ddr_pll_cntl=0x0011067d,
                    //.ddr_clk=500,
                    //.ddr_pll_cntl=0x00110228,
                    //.ddr_clk=480,
                    //.ddr_pll_cntl=0x00110673,
                    //.ddr_clk=460,
                    .ddr_pll_cntl=0x0011022c,
                    .ddr_clk=528,
         .ddr_ctrl= (0 << 24 ) |    //pctl_brst 4,
                    (0xff << 16) |  //reorder en for the 8 channel.
                    (0 << 15 ) |     // pctl16 mode = 0.  pctl =   32bits data pins
                    (0 << 14 ) |     // page policy = 0. 
                    (1 << 13 ) |     // command reorder enabled. 
                    (0 << 12 ) |     // bank map = 0, bank sweep between 4 banks. 
                    (0 << 11 ) |     // Block size.  0 = 32x32 bytes.  1 = 64x32 bytes.
                    (0 << 10 )  |     // ddr burst 0 = 8 burst. 1 = 4 burst.
                    (3 << 8 )  |      // ddr type.  2 = DDR2 SDRAM.  3 = DDR3 SDRAM.
                    (0 << 7 )  |     // ddr 16 bits mode.  0 = 32bits mode. 
                    (1 << 6 )  |     // 1 = 8 banks.  0 = 4 banks.
                    (0 << 4 )  |     // rank size.   0= 1 rank.   1 = 2 rank. 
                    (ddr3_row_size << 2) |  
                    (ddr3_col_size),
         .init_pctl=init_pctl_ddr3
#elif defined(DDR3_533_SAMSUNG)
                    .cl             =   9,
                    .t_faw          =  30,
                    .t_mrd          =   4,
                    .t_1us_pck      = 132,
                    .t_100ns_pck    =  14,
                    .t_init_us      = 511,
                    .t_ras          =  24,
                    .t_rc           =  33,
                    .t_rcd          =   9,
                    .t_refi_100ns   =  39,
                    .t_rfc          =  107,//110,
                    .t_rp           =   9,
                    .t_rrd          =   5,
                    .t_rtp          =   5,
                    .t_wr           =   10,
                    .t_wtr          =   5,
                    .t_xp           =   4,
                    .t_xsrd         =   0,   // init to 0 so that if only one of them is defined, this is chosen
                    .t_xsnr         =   0,
                    .t_exsr         = 512,
                    .t_al           =   7,   // Additive Latency
                    .t_clr          =   8,   // cas_latency for DDR2 (nclk cycles)
                    .t_dqs          =   2,   // distance between data phases to different ranks
                    .t_cwl          =   6,//7,
                    .t_mod          =   12,
                    .t_zqcl         = 512,
                    .t_rtw          =   2,
                    .t_cksrx        =   7,
                    .t_cksre        =   7,
                    .t_cke          =   4,
                    .mrs={  [0]=0,
                            [1]=(0<<6)|(1<<2)| //rtt_nominal;      //(A9, A6, A2) 000 : disabled. 001 : RZQ/4   010:RZQ/2 (A6:A2)
                                (0<<5)|(0<<1)| //(A5 A1),Output driver impedance control 00:RZQ/6,01:RZQ/7,10£ºRZQ/5 11:Reserved
                                (0<<12)|(0<<7),
                            [2]=0,
                            [3]=0
                        },
                    .mcfg = {  1 |              // burst length 0 = 4; 1 = 8
                                (0 << 2) |      // bl8int_en.   enable bl8 interrupt function.
                                //(1 << 3) |        //2T mode, default is disable
                                (1 << 5) |      // 1: ddr3 protocal; 0 : ddr2 protocal
                                //(tFAW <<18) | //tFAW will be set according to the calculation with t_rrd and t_faw
                                                // in file /firmware/ddr_init_pctl.c
                                (1 << 17) |     // power down exit which fast exit.
                                (0xf << 8)      // 0xf cycle empty will entry power down mode.
                             }, 
                    .zqcr  = (( 1 << 24) | 0x11dd),   //0x11dd->22 ohm;0x1155->0 ohm
         .ddr_pll_cntl=0x0011022c,
         .ddr_clk=528,         
         .ddr_ctrl= (0 << 24 ) |    //pctl_brst 4,
                    (0xff << 16) |  //reorder en for the 8 channel.
                    (0 << 15 ) |     // pctl16 mode = 0.  pctl =   32bits data pins
                    (0 << 14 ) |     // page policy = 0.
                    (1 << 13 ) |     // command reorder enabled.
                    (0 << 12 ) |     // bank map = 0, bank sweep between 4 banks.
                    (0 << 11 ) |     // Block size.  0 = 32x32 bytes.  1 = 64x32 bytes.
                    (0 << 9 )  |     // ddr burst 0 = 8 burst. 1 = 4 burst.
                    (3 << 8 )  |      // ddr type.  2 = DDR2 SDRAM.  3 = DDR3 SDRAM.
                    (0 << 7 )  |     // ddr 16 bits mode.  0 = 32bits mode.
                    (1 << 6 )  |     // 1 = 8 banks.  0 = 4 banks.
                    (0 << 4 )  |     // rank size.   0= 1 rank.   1 = 2 rank.
                    (ddr3_row_size << 2) |
                    (ddr3_col_size),
         .init_pctl=init_pctl_ddr3       
#elif defined(DDR3_9_9_9)
                    .cl             =   9,
                    .t_faw          =  30,   //8bit:20, 16bit:30
                    .t_mrd          =   4,
                    .t_1us_pck      = 132,
                    .t_100ns_pck    =  14,
                    .t_init_us      = 511,
                    .t_ras          =  24,
                    .t_rc           =  33,
                    .t_rcd          =   9,
                    .t_refi_100ns   =  39,
                    .t_rfc          = 107,  //2Gb:107, 4Gb:174, 1Gb:74
                    .t_rp           =   9,
                    .t_rrd          =   5,   //8bit:4, 16bit:5
                    .t_rtp          =   5,
                    .t_wr           =  10,
                    .t_wtr          =   5,
                    .t_xp           =   4,
                    .t_xsrd         =   0,   // init to 0 so that if only one of them is defined, this is chosen
                    .t_xsnr         =   0,
                    .t_exsr         = 512,
                    .t_al           =   7,   // Additive Latency
                    .t_clr          =   8,   // cas_latency for DDR2 (nclk cycles)
                    .t_dqs          =   2,   // distance between data phases to different ranks
                    .t_cwl          =   6,   //7,
                    .t_mod          =  12,
                    .t_zqcl         = 512,
                    .t_rtw          =   2,
                    .t_cksrx        =   7,
                    .t_cksre        =   7,
                    .t_cke          =   4,
                    .mrs={  [0]=0,
                            [1]=(0<<6)|(1<<2)| //rtt_nominal;      //(A9, A6, A2) 000 : disabled. 001 : RZQ/4   010:RZQ/2 (A6:A2)
                                (0<<5)|(0<<1)| //(A5 A1),Output driver impedance control 00:RZQ/6,01:RZQ/7,10：RZQ/5 11:Reserved
                                (0<<12)|(0<<7),
                            [2]=0,
                            [3]=0
                        },
                    .mcfg = {  1 |              // burst length 0 = 4; 1 = 8
                                (0 << 2) |      // bl8int_en.   enable bl8 interrupt function.
                                //(1 << 3) |        //2T mode, default is disable
                                (1 << 5) |      // 1: ddr3 protocal; 0 : ddr2 protocal
                                //(tFAW <<18) | //tFAW will be set according to the calculation with t_rrd and t_faw
                                                // in file /firmware/ddr_init_pctl.c
                                (1 << 17) |     // power down exit which fast exit.
                                (0xf << 8)      // 0xf cycle empty will entry power down mode.
                             },
                     .zqcr  = (( 1 << 24) | 0x11dd),   //0x11dd->22 ohm;0x1155->0 ohm
         .ddr_pll_cntl=0x0011022c,
         .ddr_clk=528,
         .ddr_ctrl= (0 << 24 ) |    //pctl_brst 4,
                    (0xff << 16) |  //reorder en for the 8 channel.
                    (0 << 15 ) |     // pctl16 mode = 0.  pctl =   32bits data pins
                    (0 << 14 ) |     // page policy = 0.
                    (1 << 13 ) |     // command reorder enabled.
                    (0 << 12 ) |     // bank map = 0, bank sweep between 4 banks.
                    (0 << 11 ) |     // Block size.  0 = 32x32 bytes.  1 = 64x32 bytes.
                    (0 << 10 )  |     // ddr burst 0 = 8 burst. 1 = 4 burst.
                    (3 << 8 )  |      // ddr type.  2 = DDR2 SDRAM.  3 = DDR3 SDRAM.
                    (0 << 7 )  |     // ddr 16 bits mode.  0 = 32bits mode.
                    (1 << 6 )  |     // 1 = 8 banks.  0 = 4 banks.
                    (0 << 4 )  |     // rank size.   0= 1 rank.   1 = 2 rank.
                    (ddr3_row_size << 2) |
                    (ddr3_col_size),
         .init_pctl=init_pctl_ddr3
#elif defined(DDR3_7_7_7)
                    .cl             =   7,
                    .t_faw          =  20,   //8bit:20, 16bit:27
                    .t_mrd          =   4,
                    .t_1us_pck      = 120, //132 for 528MHz, 120 for 480MHz
                    .t_100ns_pck    =  12,
                    .t_init_us      = 511,
                    .t_ras          =  20,
                    .t_rc           =  27,
                    .t_rcd          =   7,
                    .t_refi_100ns   =  39,
                    .t_rfc          = 59,   //4Gb:139, 2Gb:86, 1Gb:59
                    .t_rp           =   7,
                    .t_rrd          =   4,   //8bit:4, 16bit:6
                    .t_rtp          =   4,
                    .t_wr           =   8,
                    .t_wtr          =   4,
                    .t_xp           =   4,
                    .t_xsrd         =   0,   // init to 0 so that if only one of them is defined, this is chosen
                    .t_xsnr         =   0,
                    .t_exsr         = 512,
                    .t_al           =   0,   // Additive Latency
                    .t_clr          =   8,   // cas_latency for DDR2 (nclk cycles)
                    .t_dqs          =   2,   // distance between data phases to different ranks
                    .t_cwl          =   5,
                    .t_mod          =  12,
                    .t_zqcl         = 512,
                    .t_rtw          =   2,
                    .t_cksrx        =   6,
                    .t_cksre        =   6,
                    .t_cke          =   3,
                    .mrs={  [0]=0,
                            [1]=(1<<6)|(1<<2)| //rtt_nominal;      //(A9, A6, A2) 000 : disabled. 001 : RZQ/4   010:RZQ/2 (A6:A2)
                                (0<<5)|(1<<1)| //(A5 A1),Output driver impedance control 00:RZQ/6,01:RZQ/7,10：RZQ/5 11:Reserved
                                (0<<12)|(0<<7),
                            [2]=0,
                            [3]=0
                        },
                    .mcfg = {  1 |              // burst length 0 = 4; 1 = 8
                                (0 << 2) |      // bl8int_en.   enable bl8 interrupt function.
                                (1 << 3) |        //2T mode, default is disable
                                (1 << 5) |      // 1: ddr3 protocal; 0 : ddr2 protocal
                                //(tFAW <<18) | //tFAW will be set according to the calculation with t_rrd and t_faw
                                                // in file /firmware/ddr_init_pctl.c
                                (1 << 17) |     // power down exit which fast exit.
                                (0xf << 8)      // 0xf cycle empty will entry power down mode.
                             },
                    .zqcr  = (( 1 << 24) | 0x11cc),   //0x11dd->22 ohm;0x1155->0 ohm
         .ddr_pll_cntl=0x00110228, //0x0011022c is 528MHz   0x00110228 is 480MHz
         .ddr_clk=480,
         .ddr_ctrl= (0 << 24 ) |    //pctl_brst 4,
                    (0xff << 16) |  //reorder en for the 8 channel.
                    (0 << 15 ) |     // pctl16 mode = 0.  pctl =   32bits data pins
                    (0 << 14 ) |     // page policy = 0.
                    (1 << 13 ) |     // command reorder enabled.
                    (0 << 12 ) |     // bank map = 0, bank sweep between 4 banks.
                    (0 << 11 ) |     // Block size.  0 = 32x32 bytes.  1 = 64x32 bytes.
                    (0 << 10 )  |     // ddr burst 0 = 8 burst. 1 = 4 burst.
                    (3 << 8 )  |      // ddr type.  2 = DDR2 SDRAM.  3 = DDR3 SDRAM.
                    (0 << 7 )  |     // ddr 16 bits mode.  0 = 32bits mode.
                    (1 << 6 )  |     // 1 = 8 banks.  0 = 4 banks.
                    (0 << 4 )  |     // rank size.   0= 1 rank.   1 = 2 rank.
                    (ddr3_row_size << 2) |
                    (ddr3_col_size),
         .init_pctl=init_pctl_ddr3  
#else
	#error "Please define DDR3 type in file aml_aio_m2c_haier.h!\n"
#endif         
};

STATIC_PREFIX_DATA struct pll_clk_settings __plls __attribute__((section(".setting")))
={
    .sys_pll_cntl=0x232,//1200M
    .other_pll_cntl=0x00000219,//0x19*24/1=600M
//    .other_pll_cntl=0x0000042d,//0x2d*24/2=540M
    .mpeg_clk_cntl= (1 << 12) |                     // select other PLL
                   ((3- 1) << 0 ) |    // div1
//                    ((4- 1) << 0 ) |    // div1
                    (1 << 7 ) |                     // cntl_hi_mpeg_div_en, enable gating
                    (1 << 8 ) |(1<<15),                    // Connect clk81 to the PLL divider output
    .demod_pll400m_cntl=(1<<9)  | //n 1200=xtal*m/n 
            (50<<0),    //m 50*24
    .clk81=200000000,   //750/4=180M
    .a9_clk=1200000000/2,
    .spi_setting=0xea949,

    .nfc_cfg=(((0)&0xf)<<10) | (0<<9) | (0<<5) | 5,
    .sdio_cmd_clk_divide=5,
    .sdio_time_short=(250*180000)/(2*(12)),
    .uart=
        (200000000/(CONFIG_BAUDRATE*4) -1)
        | UART_STP_BIT 
        | UART_PRTY_BIT
        | UART_CHAR_LEN 
        //please do not remove these setting , jerry.yu
        | UART_CNTL_MASK_TX_EN
        | UART_CNTL_MASK_RX_EN
        | UART_CNTL_MASK_RST_TX
        | UART_CNTL_MASK_RST_RX
        | UART_CNTL_MASK_CLR_ERR,
};
#define DEBUGROM_CMD_BUF_SIZE ((0x1b0-0xc0-sizeof(__ddr_setting)-sizeof(__plls))&~0x1f)

STATIC_PREFIX_DATA char init_script[DEBUGROM_CMD_BUF_SIZE] __attribute__((section(".setting")))
="r c1107d54";
