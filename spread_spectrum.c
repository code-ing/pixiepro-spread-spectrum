#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/io.h>
#include <linux/clk.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/regmap.h>

#define PLL_528_SS_OFFSET 0x40
#define ANADIG_PLL_528_SYS_SS_ENABLE 0x8000
#define PLL_528_DENOM_OFFSET 0x60

#define ANADIG_DIGPROG 0x260

#define TRUE 1
#define FALSE 0


static struct regmap *anatop;

static int spread_spectrum_set_state(int state) {

  unsigned long sys_ss=0x2EE0002;
  unsigned long denom=0x4B0;

  u32 val;

  anatop = syscon_regmap_lookup_by_compatible("fsl,imx6q-anatop");
  if (IS_ERR(anatop)) {
    pr_err("%s: failed to find imx6q-anatop regmap!\n", __func__);
  }

  /* Disable spread spectrum mode */
  regmap_read(anatop,  PLL_528_SS_OFFSET, &val );
  regmap_write(anatop,  PLL_528_SS_OFFSET, val & ~ANADIG_PLL_528_SYS_SS_ENABLE);

  /* Write new values */
  regmap_write(anatop,PLL_528_SS_OFFSET,sys_ss);
  regmap_write(anatop,PLL_528_DENOM_OFFSET,denom);
  
  if (state){

    regmap_read(anatop,  PLL_528_SS_OFFSET, &val );
    regmap_write(anatop,  PLL_528_SS_OFFSET, val  | ANADIG_PLL_528_SYS_SS_ENABLE);
    printk("i.MX6 PixiePro Spread Spectrum module loaded\n");
  }

  return 0;
}


static int __init spread_spectrum_init(void) {
  spread_spectrum_set_state(TRUE);
  return 0;
}

static void __exit spread_spectrum_exit(void) {
  spread_spectrum_set_state(FALSE);

}

MODULE_LICENSE("GPL");
module_init(spread_spectrum_init);
module_exit(spread_spectrum_exit);
