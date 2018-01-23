#include <linux/init.h>
#include <linux/pci.h>
#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>
#include <linux/etherdevice.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/irq.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include <linux/platform_data/phy-at803x.h>
#include <linux/ar8216_platform.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-wmac.h"
#include "dev-usb.h"
#include "machtypes.h"


#define NANOSTATIONACL_KEYS_POLL_INTERVAL	20
#define NANOSTATIONACL_KEYS_DEBOUNCE_INTERVAL	(3 * NANOSTATIONACL_KEYS_POLL_INTERVAL)

#define NANOSTATIONACL_GPIO_BTN_RESET	12

#define NANOSTATIONACL_ETH0_MAC_OFFSET         0x0000
#define NANOSTATIONACL_WMAC_CALDATA_OFFSET     0x1000
#define NANOSTATIONACL_PCI_CALDATA_OFFSET      0x5000


static struct flash_platform_data ubnt_nanostationacl_flash_data = {
	/* mx25l12805d and mx25l12835f have the same JEDEC ID */
	.type = "mx25l12805d",
};

static struct gpio_keys_button ubnt_nanostationacl_gpio_keys[] __initdata = {
	{
		.desc			= "reset",
		.type			= EV_KEY,
		.code			= KEY_RESTART,
		.debounce_interval	= NANOSTATIONACL_KEYS_DEBOUNCE_INTERVAL,
		.gpio			= NANOSTATIONACL_GPIO_BTN_RESET,
		.active_low		= 1,
	}
};

static void __init ubnt_nanostationacl_setup(void)
{
	u8 *eeprom = (u8 *) KSEG1ADDR(0x1fff0000);

	ath79_register_m25p80(&ubnt_nanostationacl_flash_data);

	// Register mdio interface
	ath79_register_mdio(0, 0x0);

	ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_RGMII_GMAC0
		| AR934X_ETH_CFG_SW_ONLY_MODE);

	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ath79_eth0_data.mii_bus_dev = &ath79_mdio0_device.dev;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_eth0_pll_data.pll_1000 = 0x06000000;

	ath79_init_mac(ath79_eth0_data.mac_addr, eeprom
		+ NANOSTATIONACL_ETH0_MAC_OFFSET, 0);
	ath79_register_eth(0);


	ath79_register_wmac(eeprom 
		+ NANOSTATIONACL_WMAC_CALDATA_OFFSET, NULL);


	ap91_pci_init(eeprom + NANOSTATIONACL_PCI_CALDATA_OFFSET, NULL);



	ath79_register_gpio_keys_polled(-1, NANOSTATIONACL_KEYS_POLL_INTERVAL,
	                                ARRAY_SIZE(ubnt_nanostationacl_gpio_keys),
	                                ubnt_nanostationacl_gpio_keys);

}

MIPS_MACHINE(ATH79_MACH_UBNT_NANOSTATIONACL, "UBNT-NANOSTATION-ACL",
	     "Ubiquiti Nanostation AC loco", ubnt_nanostationacl_setup);

