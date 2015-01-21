/*
 * Copyright (C) 2012 Freescale Semiconductor, Inc.
 *
 * Configuration settings for the i.MX6DL UIB board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __MX6UIB_CONFIG_H
#define __MX6UIB_CONFIG_H

#include <asm/arch/imx-regs.h>
#include <asm/imx-common/gpio.h>

#define CONFIG_MACH_TYPE	3980
#define CONFIG_MXC_UART_BASE	UART1_BASE
#define CONFIG_CONSOLE_DEV		"ttymxc0"
#define CONFIG_MMCROOT			"/dev/mmcblk0p2"
#if defined(CONFIG_MX6Q)
#define CONFIG_DEFAULT_FDT_FILE	"imx6q-sabresd.dtb"
#elif defined(CONFIG_MX6DL)
#define CONFIG_DEFAULT_FDT_FILE	"imx6dl-sabresd.dtb"
#endif
#define PHYS_SDRAM_SIZE		(2u * 1024 * 1024 * 1024)

#define CONFIG_SUPPORT_EMMC_BOOT /* eMMC specific */

#include "mx6uib_common.h"

#define CONFIG_SYS_FSL_USDHC_NUM	3
#if defined(CONFIG_ENV_IS_IN_MMC)
#define CONFIG_SYS_MMC_ENV_DEV		0	/* SDHC2 */
#endif

#define CONFIG_CMD_PCI
#ifdef CONFIG_CMD_PCI
#define CONFIG_PCI
#define CONFIG_PCI_PNP
#define CONFIG_PCI_SCAN_SHOW
#define CONFIG_PCIE_IMX
#define CONFIG_PCIE_IMX_PERST_GPIO	IMX_GPIO_NR(7, 12)
#define CONFIG_PCIE_IMX_POWER_GPIO	IMX_GPIO_NR(3, 19)
#define CONFIG_E1000
#endif

/* I2C Configs */
#define CONFIG_CMD_I2C
#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_MXC
#define CONFIG_SYS_I2C_SPEED		  100000

/* PMIC */
#define CONFIG_POWER
#define CONFIG_POWER_I2C
#define CONFIG_POWER_LTC3676
#define CONFIG_POWER_LTC3676_I2C_ADDR 0x3C
#endif                         /* __MX6UIB_CONFIG_H */
