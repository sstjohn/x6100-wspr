// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022, Jet Yee.
 * Author: Jet Yee <xieyi@cqxiegu.com>
 * Author: Rui Oliveira <ruimail24@gmail.com>
 */

#include <linux/delay.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <drm/drm_panel.h>
#include <drm/drm_modes.h>
#include <drm/drm_device.h>
#include <drm/drm_connector.h>
#include <linux/spi/spi.h>
#include <linux/regulator/consumer.h>
#include <linux/gpio/consumer.h>
#include <linux/media-bus-format.h>

static const struct of_device_id st7701s_of_match[] = {
	{ .compatible = "sitronix,st7701s" },
	{ .compatible = "jinglitai,jlt4013a" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, st7701s_of_match);

// DTB that we found for this driver,
// from the Xiegu X6100
// panel@0
// {
//  compatible = "jinglitai,jlt4013a";;
//  reg = <0x00>;
//  power-supply = <0x1b>;
//  reset-gpios = <0x45 0x00 0x0b 0x01>;
//  dcx-gpios = <0x45 0x00 0x0a 0x01>;
//  backlight = <0x46>;
//  spi-max-frequency = <0x186a0>;
//
//  port {
//        endpoint {
//                  remote-endpoint = <0x47>;
//                  phandle = <0x0c>;
//       };
//  };
// };

struct st7701s {
	struct drm_panel panel;
	struct spi_device *spi;
	struct gpio_desc *reset;
	struct regulator *supply;
};

static inline struct st7701s *panel_to_st7701s(struct drm_panel *panel)
{
	return container_of(panel, struct st7701s, panel);
}

static int st7701s_prepare(struct drm_panel *panel)
{
	struct st7701s *ctx = panel_to_st7701s(panel);

	int ret = regulator_enable(ctx->supply);
	if (ret == 0) {
		msleep(120);
	}
	return ret;
}

static int st7701s_unprepare(struct drm_panel *panel)
{
	struct st7701s *ctx = panel_to_st7701s(panel);

	int ret = regulator_disable(ctx->supply);

	return ret;
}

static int st7701s_enable(struct drm_panel *panel)
{
	return 0;
}

static int st7701s_disable(struct drm_panel *panel)
{
	return 0;
}

// This is essentially the display mode of the Jinglitai JLT4013A.
static const struct drm_display_mode st7701s_default_display_mode = {
	.clock = 14616,
	.hdisplay = 480,
	.hsync_start = 512,
	.hsync_end = 523,
	.htotal = 525,
	.vdisplay = 800,
	.vsync_start = 854,
	.vsync_end = 895,
	.vtotal = 928,
	.width_mm = 52,
	.height_mm = 86,
};

static int st7701s_get_modes(struct drm_panel *panel,
			     struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	static const u32 bus_format = MEDIA_BUS_FMT_RGB888_1X24;

	mode = drm_mode_duplicate(connector->dev,
				  &st7701s_default_display_mode);
	if (mode == NULL) {
		dev_err(panel->dev, "Failed to add default mode\n");
		return -EAGAIN;
	}

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_PREFERRED | DRM_MODE_TYPE_DRIVER;

	drm_mode_probed_add(connector, mode);

	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	connector->display_info.bpc = 8;
	connector->display_info.bus_flags = DRM_BUS_FLAG_PIXDATA_DRIVE_POSEDGE;

	drm_display_info_set_bus_formats(&connector->display_info, &bus_format,
					 1);

	return 1;
}

static const struct drm_panel_funcs st7701sfuncs = {
	.prepare = st7701s_prepare,
	.unprepare = st7701s_unprepare,
	.enable = st7701s_enable,
	.disable = st7701s_disable,
	.get_modes = st7701s_get_modes,
};

static int st7701s_probe(struct spi_device *spi)
{
	struct device *dev;
	struct st7701s *ctx;
	int err;

	dev = &spi->dev;

	// In the original code `gfp: 0xdc0`, which I hope is the same as `GFP_KERNEL`
	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (ctx == NULL) {
		return -EAGAIN;
	}

	ctx->spi = spi;
	spi_set_drvdata(spi, ctx);

	ctx->supply = devm_regulator_get(dev, "power");
	if (IS_ERR(ctx->supply)) {
		dev_err(dev, "Failed to get power supply\n");
		return PTR_ERR(ctx->supply);
	}

	ctx->reset = devm_gpiod_get(dev, "reset", GPIOD_OUT_LOW);
	if (IS_ERR(ctx->reset)) {
		dev_err(dev, "Failed to get reset GPIO\n");
		return PTR_ERR(ctx->reset);
	}

	drm_panel_init(&ctx->panel, dev, &st7701sfuncs, DRM_MODE_CONNECTOR_DPI);

	err = drm_panel_of_backlight(&ctx->panel);
	if (err)
		return err;

	drm_panel_add(&ctx->panel);

	return 0;
}

static int st7701s_remove(struct spi_device *spi)
{
	struct st7701s *ctx = spi_get_drvdata(spi);

	drm_panel_remove(&(ctx->panel));
	return 0;
}

static struct spi_driver st7701s_driver = {
	.probe		= st7701s_probe,
	.remove		= st7701s_remove,
	.driver		= {
		.name	= "st7701s",
		.of_match_table = st7701s_of_match,
	},
};
module_spi_driver(st7701s_driver);

MODULE_AUTHOR("Jet Yee <xieyi@cqxiegu.com>");
MODULE_DESCRIPTION("Driver for ST7701S-based LCD panels, used in the JLT4013A");
MODULE_LICENSE("GPL v2");
