//=============================================================================					
//init script for i.MX6DL DDR3					
//=============================================================================					
// Revision History					
// v01					
//=============================================================================					
					
wait = on					
//=============================================================================					
// Disable	WDOG				
//=============================================================================					
// setmem /16	0x020bc000 =	0x30			
					
//=============================================================================					
// Enable all clocks (they are disabled by ROM code)					
//=============================================================================					
setmem /32	0x020c4068 =	0xffffffff			
setmem /32	0x020c406c =	0xffffffff			
setmem /32	0x020c4070 =	0xffffffff			
setmem /32	0x020c4074 =	0xffffffff			
setmem /32	0x020c4078 =	0xffffffff			
setmem /32	0x020c407c =	0xffffffff			
setmem /32	0x020c4080 =	0xffffffff			
					
					
//=============================================================================					
// IOMUX					
//=============================================================================					
//DDR IO TYPE:					
setmem /32	0x020e0774 =	0x000C0000	// IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE 		
setmem /32	0x020e0754 =	0x00000000	// IOMUXC_SW_PAD_CTL_GRP_DDRPKE 		
					
//CLOCK:					
setmem /32	0x020e04ac =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCLK_0		
setmem /32	0x020e04b0 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCLK_1		
					
//ADDRESS:					
setmem /32	0x020e0464 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_CAS		
setmem /32	0x020e0490 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_RAS		
setmem /32	0x020e074c =	0x00000030	// IOMUXC_SW_PAD_CTL_GRP_ADDDS 		
					
//CONTROL:					
setmem /32	0x020e0494 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_RESET		
					
setmem /32	0x020e04a0 =	0x00000000	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDBA2 - DSE can be configured using Group Control Register: IOMUXC_SW_PAD_CTL_GRP_CTLDS		
setmem /32	0x020e04b4 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDODT0		
setmem /32	0x020e04b8 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDODT1		
setmem /32	0x020e076c =	0x00000030	// IOMUXC_SW_PAD_CTL_GRP_CTLDS 		
					
//DATA STROBE:					
setmem /32	0x020e0750 =	0x00020000	// IOMUXC_SW_PAD_CTL_GRP_DDRMODE_CTL 		
					
setmem /32	0x020e04bc =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS0 		
setmem /32	0x020e04c0 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS1 		
setmem /32	0x020e04c4 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS2 		
setmem /32	0x020e04c8 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS3 		
setmem /32	0x020e04cc =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS4 		
setmem /32	0x020e04d0 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS5 		
setmem /32	0x020e04d4 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS6 		
setmem /32	0x020e04d8 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS7 		
					
//DATA:					
setmem /32	0x020e0760 =	0x00020000	// IOMUXC_SW_PAD_CTL_GRP_DDRMODE		
					
setmem /32	0x020e0764 =	0x00000030	// IOMUXC_SW_PAD_CTL_GRP_B0DS 		
setmem /32	0x020e0770 =	0x00000030	// IOMUXC_SW_PAD_CTL_GRP_B1DS 		
setmem /32	0x020e0778 =	0x00000030	// IOMUXC_SW_PAD_CTL_GRP_B2DS 		
setmem /32	0x020e077c =	0x00000030	// IOMUXC_SW_PAD_CTL_GRP_B3DS 		
setmem /32	0x020e0780 =	0x00000030	// IOMUXC_SW_PAD_CTL_GRP_B4DS 		
setmem /32	0x020e0784 =	0x00000030	// IOMUXC_SW_PAD_CTL_GRP_B5DS 		
setmem /32	0x020e078c =	0x00000030	// IOMUXC_SW_PAD_CTL_GRP_B6DS 		
setmem /32	0x020e0748 =	0x00000030	// IOMUXC_SW_PAD_CTL_GRP_B7DS 		
					
setmem /32	0x020e0470 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM0		
setmem /32	0x020e0474 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM1		
setmem /32	0x020e0478 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM2		
setmem /32	0x020e047c =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM3		
setmem /32	0x020e0480 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM4		
setmem /32	0x020e0484 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM5		
setmem /32	0x020e0488 =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM6		
setmem /32	0x020e048c =	0x00000030	// IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM7		
					
//=============================================================================					
// DDR Controller Registers					
//=============================================================================					
// Manufacturer:	Micron				
// Device Part Number:	MT41J128M16HA-15E				
// Clock Freq.: 	360MHz				
// Density per CS in Gb: 	8				
// Chip Selects used:	1				
// Number of Banks:	8				
// Row address:    	14				
// Column address: 	10				
// Data bus width	64				
//=============================================================================					
setmem /32	0x021b0800 =	0xa1390003 	// DDR_PHY_P0_MPZQHWCTRL, enable both one-time & periodic HW ZQ calibration.	0x00000002	0xa1390003 
					
// write leveling, based on Freescale board layout and T topology					
// For target board, may need to run write leveling calibration 					
// to fine tune these settings					
// If target board does not use T topology, then these registers					
// should either be cleared or write leveling calibration can be run					
setmem /32	0x021b080c = 	0x001F001F			
setmem /32	0x021b0810 = 	0x001F001F			
setmem /32	0x021b480c = 	0x001F001F			
setmem /32	0x021b4810 = 	0x001F001F			
					
//######################################################					
//calibration values based on calibration compare of 0x00ffff00:					
//Note, these calibration values are based on Freescale's board					
//May need to run calibration on target board to fine tune these 					
//######################################################					
					
//Read DQS Gating calibration					
setmem /32	0x021b083c =	0x42180218	// MPDGCTRL0 PHY0	0x00320000	0x4220021F
setmem /32	0x021b0840 =	0x020C020C	// MPDGCTRL1 PHY0	0x00000000	0x0207017E
setmem /32	0x021b483c =	0x42080210	// MPDGCTRL0 PHY1	0x00000000	0x4201020C
setmem /32	0x021b4840 =	0x02000200	// MPDGCTRL1 PHY1	0x02000000	0x01660172
					
//Read calibration					
setmem /32	0x021b0848 =	0x44444442	// MPRDDLCTL PHY0	0x00000100	0x4A4D4E4D
setmem /32	0x021b4848 =	0x484A4A48	// MPRDDLCTL PHY1	0x02400040	0x4A4F5049
					
//Write calibration					
setmem /32	0x021b0850 =	0x34322C32	// MPWRDLCTL PHY0	0x01000000	0x3F3C3D31
setmem /32	0x021b4850 =	0x3838382E	// MPWRDLCTL PHY1	0x0a000100	0x3238372B
					
//read data bit delay: (3 is the reccommended default value, although out of reset value is 0):					
setmem /32	0x021b081c =	0x33333333	// DDR_PHY_P0_MPREDQBY0DL3	Original Values (Delete Later)	Difference?
setmem /32	0x021b0820 =	0x33333333	// DDR_PHY_P0_MPREDQBY1DL3	0x00009c40	0x33333333
setmem /32	0x021b0824 =	0x33333333	// DDR_PHY_P0_MPREDQBY2DL3	0x00000000	0x33333333
setmem /32	0x021b0828 =	0x33333333	// DDR_PHY_P0_MPREDQBY3DL3	0x00000000	0x33333333
setmem /32	0x021b481c =	0x33333333	// DDR_PHY_P1_MPREDQBY0DL3	0x02000000	0x33333333
setmem /32	0x021b4820 =	0x33333333	// DDR_PHY_P1_MPREDQBY1DL3	0x01010706	0x33333333
setmem /32	0x021b4824 =	0x33333333	// DDR_PHY_P1_MPREDQBY2DL3	0x080b0201	0x33333333
setmem /32	0x021b4828 =	0x33333333	// DDR_PHY_P1_MPREDQBY3DL3	0x02000303	0x33333333
					
//For i.mx6qd parts of versions A & B (v1.0, v1.1), uncomment the following lines. For version C (v1.2), keep commented					
//setmem /32	0x021b08c0 =	0x24911492	// fine tune SDCLK duty cyc to low - seen to improve measured duty cycle of i.mx6		
//setmem /32	0x021b48c0 =	0x24911492			
					
// Complete calibration by forced measurement:					
setmem /32	0x021b08b8 =	0x00000800 	// DDR_PHY_P0_MPMUR0, frc_msr	0x02020101	0x00000800 
setmem /32	0x021b48b8 =	0x00000800 	// DDR_PHY_P0_MPMUR0, frc_msr	0x00000000	0x00000800 
					
//MMDC init:					
setmem /32	0x021b0004 =	0x0002002D	// MMDC0_MDPDC 	0x00000100	0x0002002D
setmem /32	0x021b0008 =	0x00333030	// MMDC0_MDOTC	0x00050612	0x00333030
setmem /32	0x021b000c =	0x393D5102	// MMDC0_MDCFG0	0x06030301	0x393D5102
setmem /32	0x021b0010 =	0x922C8B63	// MMDC0_MDCFG1	0x00000000	0x922C8B63
setmem /32	0x021b0014 =	0x01FF00DB	// MMDC0_MDCFG2	0x00000a02	0x01FF00DB
setmem /32	0x021b0018 =	0x00001740	// MMDC0_MDMISC	0x015dc002	0x00001740
//NOTE about MDMISC RALAT:					
//MDMISC: RALAT kept to the high level of 5 to ensure stable operation at 528MHz. 					
//MDMISC: consider reducing RALAT if your 528MHz board design allow that. Lower RALAT benefits: 					
//a. better operation at low frequency					
//b. Small performence improvment					
					
setmem /32	0x021b001c =	0x00008000	// MMDC0_MDSCR, set the Configuration request bit during MMDC set up	0x00000606	0x00008000
setmem /32	0x021b002c =	0x000026d2	// MMDC0_MDRWD; recommend to maintain the default values	0x00000003	0x000026d2
setmem /32	0x021b0030 =	0x003D1023	// MMDC0_MDOR	0x00001401	0x003D1023
setmem /32	0x021b0040 =	0x00000027	// CS0_END 	0x0018001b	0x00000027
					
setmem /32	0x021b0000 =	0x831A0000	// MMDC0_MDCTL	0x00010000	0x831A0000
					
// Mode register writes					
setmem /32	0x021b001c =	0x04008032	// MMDC0_MDSCR, MR2 write, CS0	0x00000000	0x04008032
setmem /32	0x021b001c =	0x00008033	// MMDC0_MDSCR, MR3 write, CS0	0x00000000	0x00008033
setmem /32	0x021b001c =	0x00048031	// MMDC0_MDSCR, MR1 write, CS0	0x00000000	0x00048031
setmem /32	0x021b001c =	0x05108030	// MMDC0_MDSCR, MR0 write, CS0	0x02000000	0x05108030
setmem /32	0x021b001c =	0x04008040	// MMDC0_MDSCR, ZQ calibration command sent to device on CS0		
					
//setmem /32	0x021b001c =	0x0400803A	// MMDC0_MDSCR, MR2 write, CS1	0x00000000	0x0400803A
//setmem /32	0x021b001c =	0x0000803B	// MMDC0_MDSCR, MR3 write, CS1	0x00000000	0x0000803B
//setmem /32	0x021b001c =	0x00048039	// MMDC0_MDSCR, MR1 write, CS1	0x00000000	0x00048039
//setmem /32	0x021b001c =	0x05108038	// MMDC0_MDSCR, MR0 write, CS1	0x02000000	0x05108038
//setmem /32	0x021b001c =	0x04008048	// MMDC0_MDSCR, ZQ calibration command sent to device on CS1		
					
					
setmem /32	0x021b0020 =	0x00005800	// MMDC0_MDREF	0x00320000	0x00005800
					
setmem /32	0x021b0818 =	0x00011117	// DDR_PHY_P0_MPODTCTRL	0x00000000	0x00011117
setmem /32	0x021b4818 =	0x00011117	// DDR_PHY_P1_MPODTCTRL	0x00000000	0x00011117
					
					
setmem /32	0x021b0004 =	0x0002556D	// MMDC0_MDPDC with PWDT bits set		
setmem /32	0x021b0404 = 	0x00011006	// MMDC0_MAPSR ADOPT power down enabled, MMDC will enter automatically to self-refresh while the number of idle cycle reached.		
					
setmem /32	0x021b001c =	0x00000000	// MMDC0_MDSCR, clear this register (especially the configuration bit as initialization is complete)	0x01000202	0x00000000
