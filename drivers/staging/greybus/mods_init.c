/*
 * Copyright (C) 2015-2016 Motorola Mobility LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#define pr_fmt(fmt) "MODS_INIT: " fmt

#define CREATE_TRACE_POINTS
#include "greybus.h"
#include "mods_trace.h"

#include <linux/debugfs.h>
#include <linux/module.h>

#include "muc.h"

static struct dentry *mods_debug_root;

struct dentry *mods_debugfs_get(void)
{
	return mods_debug_root;
}

static int __init mods_init(void)
{
	int err;

	mods_debug_root = debugfs_create_dir("mods", NULL);
	if (!mods_debug_root)
		pr_warn("failed to create 'mods' debugfs\n");

	err = muc_core_init();
	if (err) {
		pr_err("muc_core_init failed: %d\n", err);
		goto exit;
	}

	err = muc_svc_init();
	if (err) {
		pr_err("muc_svc_init failed: %d\n", err);
		goto svc_fail;
	}

	err = mods_ap_init();
	if (err) {
		pr_err("mods_ap_init failed: %d\n", err);
		goto ap_fail;
	}

	err = muc_spi_init();
	if (err) {
		pr_err("muc_spi_init failed: %d\n", err);
		goto spi_fail;
	}

	return 0;

spi_fail:
	mods_ap_exit();
ap_fail:
	muc_svc_exit();
svc_fail:
	muc_core_exit();
exit:
	debugfs_remove_recursive(mods_debug_root);
	mods_debug_root = NULL;

	return err;
}

static void __exit mods_exit(void)
{
	muc_spi_exit();
	mods_ap_exit();
	muc_svc_exit();
	muc_core_exit();

	debugfs_remove_recursive(mods_debug_root);
	mods_debug_root = NULL;
}

module_init(mods_init);
module_exit(mods_exit);

MODULE_DESCRIPTION("Mods Driver");
MODULE_AUTHOR("Motorola");
MODULE_LICENSE("GPL");
