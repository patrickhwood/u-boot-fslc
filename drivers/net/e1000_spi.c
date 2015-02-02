#include "e1000.h"
#include <linux/compiler.h>

/******************************************************************************
 * Raises the FLASH's clock input.
 *
 * hw - Struct containing variables accessed by shared code
 * fla - FLA's current value
 *****************************************************************************/
static void e1000_raise_fl_clk(struct e1000_hw *hw, uint32_t * fla)
{
	/* Raise the clock input to the FLASH (by setting the SK bit), and then
	 * wait 10 microseconds.
	 */
	*fla = *fla | E1000_FL_NVM_SK;
	E1000_WRITE_REG(hw, FLA, *fla);
	E1000_WRITE_FLUSH(hw);
	udelay(hw->eeprom.delay_usec);
}

/******************************************************************************
 * Lowers the FLASH's clock input.
 *
 * hw - Struct containing variables accessed by shared code
 * fla - FLA's current value
 *****************************************************************************/
static void e1000_lower_fl_clk(struct e1000_hw *hw, uint32_t * fla)
{
	/* Lower the clock input to the FLASH (by clearing the SK bit), and then
	 * wait 10 microseconds.
	 */
	*fla = *fla & ~E1000_FL_NVM_SK;
	E1000_WRITE_REG(hw, FLA, *fla);
	E1000_WRITE_FLUSH(hw);
	udelay(hw->eeprom.delay_usec);
}

/*-----------------------------------------------------------------------
 * SPI transfer
 *
 * This writes "bitlen" bits out the SPI MOSI port and simultaneously clocks
 * "bitlen" bits in the SPI MISO port.  That's just the way SPI works.
 *
 * The source of the outgoing bits is the "dout" parameter and the
 * destination of the input bits is the "din" parameter.  Note that "dout"
 * and "din" can point to the same memory location, in which case the
 * input data overwrites the output data (since both are buffered by
 * temporary variables, this is OK).
 *
 * This may be interrupted with Ctrl-C if "intr" is true, otherwise it will
 * never return an error.
 */
static int e1000_spi_xfer(struct e1000_hw *hw, unsigned int bitlen,
		const void *dout_mem, void *din_mem, bool intr)
{
	const uint8_t *dout = dout_mem;
	uint8_t *din = din_mem;

	uint8_t mask = 0;
	uint32_t fla;
	unsigned long i;

	/* Pre-read the control register */
	fla = E1000_READ_REG(hw, FLA);

	/* Iterate over each bit */
	for (i = 0, mask = 0x80; i < bitlen; i++, mask = (mask >> 1)?:0x80) {
		/* Check for interrupt */
		if (intr && ctrlc())
			return -1;

		/* Determine the output bit */
		if (dout && dout[i >> 3] & mask)
			fla |=  E1000_FL_SI;
		else
			fla &= ~E1000_FL_SI;

		/* Write the output bit and wait 1us */
		E1000_WRITE_REG(hw, FLA, fla);
		E1000_WRITE_FLUSH(hw);
		udelay(hw->eeprom.delay_usec);

		/* Poke the clock (waits 1us) */
		e1000_raise_fl_clk(hw, &fla);

		/* Now read the input bit */
		fla = E1000_READ_REG(hw, FLA);
		if (din) {
			if (fla & E1000_FL_SO)
				din[i >> 3] |=  mask;
			else
				din[i >> 3] &= ~mask;
		}

		/* Poke the clock again (waits 1us) */
		e1000_lower_fl_clk(hw, &fla);
	}

	/* Now clear any remaining bits of the input */
	if (din && (i & 7))
		din[i >> 3] &= ~((mask << 1) - 1);

	return 0;
}

/******************************************************************************
 * Returns FLASH to a "standby" state
 *
 * hw - Struct containing variables accessed by shared code
 *****************************************************************************/
void e1000_standby_flash(struct e1000_hw *hw)
{
	struct e1000_eeprom_info *eeprom = &hw->eeprom;
	uint32_t fla;

	fla = E1000_READ_REG(hw, FLA);

	if (eeprom->type == e1000_eeprom_flash) {
		/* Toggle CE to flush commands */
		fla |= E1000_FL_CEN;
		E1000_WRITE_REG(hw, FLA, fla);
		E1000_WRITE_FLUSH(hw);
		udelay(eeprom->delay_usec);
		fla &= ~E1000_FL_CEN;
		E1000_WRITE_REG(hw, FLA, fla);
		E1000_WRITE_FLUSH(hw);
		udelay(eeprom->delay_usec);
	}
}

/******************************************************************************
 * Prepares FLASH for access
 *
 * hw - Struct containing variables accessed by shared code
 *
 * Lowers FLASH clock. Clears input pin. Sets the chip select pin. This
 * function should be called before issuing a command to the FLASH.
 *****************************************************************************/
static int32_t e1000_acquire_flash(struct e1000_hw *hw)
{
	struct e1000_eeprom_info *eeprom = &hw->eeprom;
	uint32_t fla, i;

	DEBUGFUNC();

	fla = E1000_READ_REG(hw, FLA);

	/* Setup FLASH for Read/Write */

	if (eeprom->type == e1000_eeprom_flash) {
		fla |= E1000_FL_REQ;
		E1000_WRITE_REG(hw, FLA, fla);
		for (i = 0; (!(fla & E1000_FL_GNT)) &&
		       (i < E1000_EEPROM_GRANT_ATTEMPTS); i++) {
                udelay(5);
                fla = E1000_READ_REG(hw, FLA);
		}
		if (!(fla & E1000_FL_GNT)) {
			fla &= ~E1000_FL_REQ;
			E1000_WRITE_REG(hw, FLA, fla);
			E1000_DBG(hw->nic, "Could not acquire FLASH grant: fla=%x\n", fla);
			return -E1000_ERR_EEPROM;
		}

		/* Clear SI, SK and enable CEN */
		fla &= ~(E1000_FL_SI | E1000_FL_CEN | E1000_FL_NVM_SK);
		E1000_WRITE_REG(hw, FLA, fla);
		udelay(1);
	}
	E1000_DBG(hw->nic, "acquire succeeded\n");

	return E1000_SUCCESS;
}

void e1000_release_flash(struct e1000_hw *hw)
{
	uint32_t fla;

	DEBUGFUNC();

	fla = E1000_READ_REG(hw, FLA);

	if (hw->eeprom.type == e1000_eeprom_flash) {
		fla |= E1000_FL_CEN;  /* Drive CEN high */
		fla &= ~E1000_FL_NVM_SK; /* Lower SCK */

		E1000_WRITE_REG(hw, FLA, fla);

		udelay(hw->eeprom.delay_usec);

		fla &= ~E1000_FL_REQ;
		E1000_WRITE_REG(hw, FLA, fla);
	}
}

#ifdef CONFIG_E1000_SPI_GENERIC
static inline struct e1000_hw *e1000_hw_from_spi(struct spi_slave *spi)
{
	return container_of(spi, struct e1000_hw, spi);
}

/* Not sure why all of these are necessary */
void spi_init_r(void) { /* Nothing to do */ }
void spi_init_f(void) { /* Nothing to do */ }
void spi_init(void)   { /* Nothing to do */ }

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
		unsigned int max_hz, unsigned int mode)
{
	/* Find the right PCI device */
	struct e1000_hw *hw = e1000_find_card(bus);
	if (!hw) {
		E1000_ERR(hw->nic, "No such e1000 device: e1000#%u\n", bus);
		return NULL;
	}

	/* Make sure it has an SPI chip */
	if (hw->eeprom.type != e1000_eeprom_flash) {
		E1000_ERR(hw->nic, "No attached SPI FLASH found!\n");
		return NULL;
	}

	/* Argument sanity checks */
	if (cs != 0) {
		E1000_ERR(hw->nic, "No such SPI chip: %u\n", cs);
		return NULL;
	}
	if (mode != SPI_MODE_0) {
		E1000_ERR(hw->nic, "Only SPI MODE-0 is supported!\n");
		return NULL;
	}

	/* TODO: Use max_hz somehow */
	E1000_DBG(hw->nic, "FLASH SPI access requested\n");
	return &hw->spi;
}

void spi_free_slave(struct spi_slave *spi)
{
	__maybe_unused struct e1000_hw *hw = e1000_hw_from_spi(spi);
	E1000_DBG(hw->nic, "FLASH SPI access released\n");
}

int spi_claim_bus(struct spi_slave *spi)
{
	struct e1000_hw *hw = e1000_hw_from_spi(spi);

	if (e1000_acquire_flash(hw)) {
		E1000_ERR(hw->nic, "FLASH SPI cannot be acquired!\n");
		return -1;
	}

	return 0;
}

void spi_release_bus(struct spi_slave *spi)
{
	struct e1000_hw *hw = e1000_hw_from_spi(spi);
	e1000_release_flash(hw);
}

/* Skinny wrapper around e1000_spi_xfer */
int spi_xfer(struct spi_slave *spi, unsigned int bitlen,
		const void *dout_mem, void *din_mem, unsigned long flags)
{
	struct e1000_hw *hw = e1000_hw_from_spi(spi);
	int ret;

	if (flags & SPI_XFER_BEGIN)
		e1000_standby_flash(hw);

	ret = e1000_spi_xfer(hw, bitlen, dout_mem, din_mem, true);

	if (flags & SPI_XFER_END)
		e1000_standby_flash(hw);

	return ret;
}

#endif /* not CONFIG_E1000_SPI_GENERIC */

#ifdef CONFIG_CMD_E1000

/* The FLASH opcodes */
#define SPI_FLASH_ENABLE_WR	0x06
#define SPI_FLASH_DISABLE_WR	0x04
#define SPI_FLASH_WRITE_STATUS	0x01
#define SPI_FLASH_READ_STATUS	0x05
#define SPI_FLASH_WRITE_PAGE	0x02
#define SPI_FLASH_READ_PAGE		0x03
#define SPI_FLASH_CHIP_ERASE	0x60

/* The FLASH status bits */
#define SPI_FLASH_STATUS_BUSY	0x01
#define SPI_FLASH_STATUS_WREN	0x02
#define SPI_FLASH_BPL_MASK		0x3C
#define SPI_FLASH_BPL_RO		0x80

static int e1000_spi_flash_enable_wr(struct e1000_hw *hw, bool intr)
{
	u8 op[] = { SPI_FLASH_ENABLE_WR };
	e1000_standby_flash(hw);
	return e1000_spi_xfer(hw, 8*sizeof(op), op, NULL, intr);
}

static int e1000_spi_flash_write_status(struct e1000_hw *hw,
		u8 status, bool intr)
{
	u8 op[] = { SPI_FLASH_WRITE_STATUS, status };
	e1000_spi_flash_enable_wr(hw, intr);
	e1000_standby_flash(hw);
	return e1000_spi_xfer(hw, 8*sizeof(op), op, NULL, intr);
}

static int e1000_spi_flash_read_status(struct e1000_hw *hw, bool intr)
{
	u8 op[] = { SPI_FLASH_READ_STATUS, 0 };
	e1000_standby_flash(hw);
	if (e1000_spi_xfer(hw, 8*sizeof(op), op, op, intr))
		return -1;
	return op[1];
}

static int e1000_spi_flash_write_page(struct e1000_hw *hw,
		const void *data, u16 off, u16 len, bool intr)
{
	u8 op[] = {
		SPI_FLASH_WRITE_PAGE,
		(off >> 16) & 0xff, (off >> 8) & 0xff, off & 0xff
	};

	e1000_standby_flash(hw);

	if (e1000_spi_xfer(hw, 8 + 24, op, NULL, intr))
		return -1;
	if (e1000_spi_xfer(hw, len << 3, data, NULL, intr))
		return -1;

	return 0;
}

static int e1000_spi_flash_read_page(struct e1000_hw *hw,
		void *data, u16 off, u16 len, bool intr)
{
	u8 op[] = {
		SPI_FLASH_READ_PAGE,
		(off >> 16) & 0xff, (off >> 8) & 0xff, off & 0xff
	};

	e1000_standby_flash(hw);

	if (e1000_spi_xfer(hw, 8 + 24, op, NULL, intr))
		return -1;
	if (e1000_spi_xfer(hw, len << 3, NULL, data, intr))
		return -1;

	return 0;
}

static int e1000_spi_flash_poll_ready(struct e1000_hw *hw, bool intr)
{
	int status;
	while ((status = e1000_spi_flash_read_status(hw, intr)) >= 0) {
		if (!(status & SPI_FLASH_STATUS_BUSY))
			return 0;
	}
	return -1;
}

int e1000_spi_flash_clear_bp(struct e1000_hw *hw, bool intr)
{
	/* clear block protect bits */
	return e1000_spi_flash_write_status(hw, (u8) ~(SPI_FLASH_BPL_MASK|SPI_FLASH_BPL_RO), intr);
}

static int e1000_spi_flash_dump(struct e1000_hw *hw,
		void *data, u16 off, unsigned int len, bool intr)
{
	/* Interruptibly wait for the FLASH to be ready */
	if (e1000_spi_flash_poll_ready(hw, intr))
		return -1;

	/* Dump each page in sequence */
	while (len) {
		/* Calculate the data bytes on this page */
		u16 pg_off = off & (hw->eeprom.page_size - 1);
		u16 pg_len = hw->eeprom.page_size - pg_off;
		if (pg_len > len)
			pg_len = len;

		/* Now dump the page */
		if (e1000_spi_flash_read_page(hw, data, off, pg_len, intr))
			return -1;

		/* Otherwise go on to the next page */
		len  -= pg_len;
		off  += pg_len;
		data += pg_len;
	}

	/* We're done! */
	return 0;
}

static int e1000_spi_flash_program(struct e1000_hw *hw,
		const void *data, u16 off, u16 len, bool intr)
{
	/* clear the block protect bits in the status register */
	if (e1000_spi_flash_clear_bp(hw, true) < 0) {
		E1000_ERR(hw->nic, "clear_bp failed!\n");
		e1000_release_flash(hw);
		return 1;
	}

	/* Program each page in sequence */
	while (len) {
		/* Calculate the data bytes on this page */
		u16 pg_len = 1;

		/* Interruptibly wait for the FLASH to be ready */
		if (e1000_spi_flash_poll_ready(hw, intr))
			return -1;

		/* Enable write access */
		if (e1000_spi_flash_enable_wr(hw, intr))
			return -1;

		/* Now program the page */
		if (e1000_spi_flash_write_page(hw, data, off, pg_len, intr))
			return -1;

		/* Otherwise go on to the next page */
		len  -= pg_len;
		off  += pg_len;
		data += pg_len;
	}

	/* Wait for the last write to complete */
	if (e1000_spi_flash_poll_ready(hw, intr))
		return -1;

	/* We're done! */
	return 0;
}

static int do_e1000_spi_show(cmd_tbl_t *cmdtp, struct e1000_hw *hw,
		int argc, char * const argv[])
{
	unsigned int length = 0;
	u16 i, offset = 0;
	u8 *buffer;
	int err;

	if (argc > 2) {
		cmd_usage(cmdtp);
		return 1;
	}

	/* Parse the offset and length */
	if (argc >= 1)
		offset = simple_strtoul(argv[0], NULL, 0);
	if (argc == 2)
		length = simple_strtoul(argv[1], NULL, 0);
	else if (offset < (hw->eeprom.word_size << 1))
		length = (hw->eeprom.word_size << 1) - offset;

	/* Extra sanity checks */
	if (!length) {
		E1000_ERR(hw->nic, "Requested zero-sized dump!\n");
		return 1;
	}
	if ((0x10000 < length) || (0x10000 - length < offset)) {
		E1000_ERR(hw->nic, "Can't dump past 0xFFFF!\n");
		return 1;
	}

	/* Allocate a buffer to hold stuff */
	buffer = malloc(length);
	if (!buffer) {
		E1000_ERR(hw->nic, "Out of Memory!\n");
		return 1;
	}

	/* Acquire the FLASH and perform the dump */
	if (e1000_acquire_flash(hw)) {
		E1000_ERR(hw->nic, "FLASH SPI cannot be acquired!\n");
		free(buffer);
		return 1;
	}
	err = e1000_spi_flash_dump(hw, buffer, offset, length, true);
	e1000_release_flash(hw);
	if (err) {
		E1000_ERR(hw->nic, "Interrupted!\n");
		free(buffer);
		return 1;
	}

	/* Now hexdump the result */
	printf("%s: ===== Intel e1000 FLASH (0x%04hX - 0x%04hX) =====",
			hw->nic->name, offset, offset + length - 1);
	for (i = 0; i < length; i++) {
		if ((i & 0xF) == 0)
			printf("\n%s: %04hX: ", hw->nic->name, offset + i);
		else if ((i & 0xF) == 0x8)
			printf(" ");
		printf(" %02hx", buffer[i]);
	}
	printf("\n");

	/* Success! */
	free(buffer);
	return 0;
}

static int do_e1000_spi_dump(cmd_tbl_t *cmdtp, struct e1000_hw *hw,
		int argc, char * const argv[])
{
	unsigned int length;
	u16 offset;
	void *dest;

	if (argc != 3) {
		cmd_usage(cmdtp);
		return 1;
	}

	/* Parse the arguments */
	dest = (void *)simple_strtoul(argv[0], NULL, 16);
	offset = simple_strtoul(argv[1], NULL, 0);
	length = simple_strtoul(argv[2], NULL, 0);

	/* Extra sanity checks */
	if (!length) {
		E1000_ERR(hw->nic, "Requested zero-sized dump!\n");
		return 1;
	}
	if ((0x10000 < length) || (0x10000 - length < offset)) {
		E1000_ERR(hw->nic, "Can't dump past 0xFFFF!\n");
		return 1;
	}

	/* Acquire the FLASH */
	if (e1000_acquire_flash(hw)) {
		E1000_ERR(hw->nic, "FLASH SPI cannot be acquired!\n");
		return 1;
	}

	/* Perform the dump operation */
	if (e1000_spi_flash_dump(hw, dest, offset, length, true) < 0) {
		E1000_ERR(hw->nic, "Interrupted!\n");
		e1000_release_flash(hw);
		return 1;
	}

	e1000_release_flash(hw);
	printf("%s: ===== FLASH DUMP COMPLETE =====\n", hw->nic->name);
	return 0;
}

static int do_e1000_spi_program(cmd_tbl_t *cmdtp, struct e1000_hw *hw,
		int argc, char * const argv[])
{
	unsigned int length;
	const void *source;
	u16 offset;

	if (argc != 3) {
		cmd_usage(cmdtp);
		return 1;
	}

	/* Parse the arguments */
	source = (const void *)simple_strtoul(argv[0], NULL, 16);
	offset = simple_strtoul(argv[1], NULL, 0);
	length = simple_strtoul(argv[2], NULL, 0);

	/* Acquire the FLASH */
	if (e1000_acquire_flash(hw)) {
		E1000_ERR(hw->nic, "FLASH SPI cannot be acquired!\n");
		return 1;
	}

	/* Perform the programming operation */
	if (e1000_spi_flash_program(hw, source, offset, length, true) < 0) {
		E1000_ERR(hw->nic, "Interrupted e1000_spi_flash_program!\n");
		e1000_release_flash(hw);
		return 1;
	}

	e1000_release_flash(hw);
	printf("%s: ===== FLASH PROGRAMMED =====\n", hw->nic->name);
	return 0;
}

static int do_e1000_spi_checksum(cmd_tbl_t *cmdtp, struct e1000_hw *hw,
		int argc, char * const argv[])
{
	uint16_t i, length, checksum = 0, checksum_reg;
	uint16_t *buffer;
	bool upd;

	if (argc == 0)
		upd = 0;
	else if ((argc == 1) && !strcmp(argv[0], "update"))
		upd = 1;
	else {
		cmd_usage(cmdtp);
		return 1;
	}

	/* Allocate a temporary buffer */
	length = sizeof(uint16_t) * (EEPROM_CHECKSUM_REG + 1);
	buffer = malloc(length);
	if (!buffer) {
		E1000_ERR(hw->nic, "Unable to allocate FLASH buffer!\n");
		return 1;
	}

	/* Acquire the FLASH */
	if (e1000_acquire_flash(hw)) {
		E1000_ERR(hw->nic, "FLASH SPI cannot be acquired!\n");
		return 1;
	}

	/* Read the FLASH */
	if (e1000_spi_flash_dump(hw, buffer, 0, length, true) < 0) {
		E1000_ERR(hw->nic, "Interrupted!\n");
		e1000_release_flash(hw);
		return 1;
	}

	/* Compute the checksum and read the expected value */
	for (i = 0; i < EEPROM_CHECKSUM_REG; i++)
		checksum += le16_to_cpu(buffer[i]);
	checksum = ((uint16_t)EEPROM_SUM) - checksum;
	checksum_reg = le16_to_cpu(buffer[i]);

	/* Verify it! */
	if (checksum_reg == checksum) {
		printf("%s: INFO: FLASH checksum is correct! (0x%04hx)\n",
				hw->nic->name, checksum);
		e1000_release_flash(hw);
		return 0;
	}

	/* Hrm, verification failed, print an error */
	E1000_ERR(hw->nic, "FLASH checksum is incorrect!\n");
	E1000_ERR(hw->nic, "  ...register was 0x%04hx, calculated 0x%04hx\n",
			checksum_reg, checksum);

	/* If they didn't ask us to update it, just return an error */
	if (!upd) {
		e1000_release_flash(hw);
		return 1;
	}

	/* Ok, correct it! */
	printf("%s: Reprogramming the FLASH checksum...\n", hw->nic->name);
	buffer[i] = cpu_to_le16(checksum);
	if (e1000_spi_flash_program(hw, &buffer[i], i * sizeof(uint16_t),
			sizeof(uint16_t), true)) {
		E1000_ERR(hw->nic, "Interrupted!\n");
		e1000_release_flash(hw);
		return 1;
	}

	e1000_release_flash(hw);
	return 0;
}

static int do_e1000_spi_erase(cmd_tbl_t *cmdtp, struct e1000_hw *hw, bool intr)
{
	u8 op[] = { SPI_FLASH_CHIP_ERASE };

	/* Acquire the FLASH */
	if (e1000_acquire_flash(hw)) {
		E1000_ERR(hw->nic, "FLASH SPI cannot be acquired!\n");
		return 1;
	}

	/* clear the block protect bits in the status register */
	e1000_standby_flash(hw);
	if (e1000_spi_flash_clear_bp(hw, true) < 0) {
		E1000_ERR(hw->nic, "clear_bp failed!\n");
		e1000_release_flash(hw);
		return 1;
	}

	e1000_spi_flash_enable_wr(hw, intr);

	e1000_standby_flash(hw);
	e1000_spi_xfer(hw, 8*sizeof(op), op, NULL, intr);

	e1000_release_flash(hw);

	return 0;
}

static int do_e1000_spi_unlock(cmd_tbl_t *cmdtp, struct e1000_hw *hw, bool intr)
{
	/* Acquire the FLASH */
	if (e1000_acquire_flash(hw)) {
		E1000_ERR(hw->nic, "FLASH SPI cannot be acquired!\n");
		return 1;
	}

	E1000_DBG(hw->nic, "status = %x\n", e1000_spi_flash_read_status(hw, intr));
	/* clear the block protect bits in the status register */
	e1000_standby_flash(hw);
	if (e1000_spi_flash_clear_bp(hw, true) < 0) {
		E1000_ERR(hw->nic, "clear_bp failed!\n");
		e1000_release_flash(hw);
		return 1;
	}
	E1000_DBG(hw->nic, "status = %x\n", e1000_spi_flash_read_status(hw, intr));
	e1000_release_flash(hw);
	return 0;
}

int do_e1000_spi(cmd_tbl_t *cmdtp, struct e1000_hw *hw,
		int argc, char * const argv[])
{
	if (argc < 1) {
		cmd_usage(cmdtp);
		return 1;
	}

	/* Make sure it has an SPI chip */
	if (hw->eeprom.type != e1000_eeprom_flash) {
		E1000_ERR(hw->nic, "No attached SPI FLASH found!\n");
		return 1;
	}

	/* Check the eeprom sub-sub-command arguments */
	if (!strcmp(argv[0], "show"))
		return do_e1000_spi_show(cmdtp, hw, argc - 1, argv + 1);

	if (!strcmp(argv[0], "dump"))
		return do_e1000_spi_dump(cmdtp, hw, argc - 1, argv + 1);

	if (!strcmp(argv[0], "program"))
		return do_e1000_spi_program(cmdtp, hw, argc - 1, argv + 1);

	if (!strcmp(argv[0], "checksum"))
		return do_e1000_spi_checksum(cmdtp, hw, argc - 1, argv + 1);

	if (!strcmp(argv[0], "erase"))
		return do_e1000_spi_erase(cmdtp, hw, true);

	if (!strcmp(argv[0], "unlock"))
		return do_e1000_spi_unlock(cmdtp, hw, true);

	cmd_usage(cmdtp);
	return 1;
}

#endif /* not CONFIG_CMD_E1000 */
