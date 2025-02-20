#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_MITIGATION_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const char ____versions[]
__used __section("__versions") =
	"\x14\x00\x00\x00\x5e\xc3\x64\xaa"
	"cdev_add\0\0\0\0"
	"\x18\x00\x00\x00\x6b\xaa\x0f\xe7"
	"class_create\0\0\0\0"
	"\x18\x00\x00\x00\x65\xc2\x96\x7c"
	"device_create\0\0\0"
	"\x14\x00\x00\x00\x63\xec\x3d\xbc"
	"proc_mkdir\0\0"
	"\x14\x00\x00\x00\x2b\x3a\x21\x7f"
	"proc_create\0"
	"\x1c\x00\x00\x00\xca\x39\x82\x5b"
	"__x86_return_thunk\0\0"
	"\x24\x00\x00\x00\x33\xb3\x91\x60"
	"unregister_chrdev_region\0\0\0\0"
	"\x18\x00\x00\x00\x7f\xe7\x31\xb2"
	"class_destroy\0\0\0"
	"\x1c\x00\x00\x00\x48\x9f\xdb\x88"
	"__check_object_size\0"
	"\x18\x00\x00\x00\xc2\x9c\xc4\x13"
	"_copy_from_user\0"
	"\x28\x00\x00\x00\xb3\x1c\xa2\x87"
	"__ubsan_handle_out_of_bounds\0\0\0\0"
	"\x10\x00\x00\x00\x94\xb6\x16\xa9"
	"strnlen\0"
	"\x18\x00\x00\x00\xe1\xbe\x10\x6b"
	"_copy_to_user\0\0\0"
	"\x18\x00\x00\x00\xb5\x79\xca\x75"
	"__fortify_panic\0"
	"\x14\x00\x00\x00\x65\xc9\xf2\x1d"
	"proc_remove\0"
	"\x18\x00\x00\x00\x3c\xd6\xa8\x07"
	"device_destroy\0\0"
	"\x14\x00\x00\x00\x69\x1b\x25\x12"
	"cdev_del\0\0\0\0"
	"\x14\x00\x00\x00\xbb\x6d\xfb\xbd"
	"__fentry__\0\0"
	"\x1c\x00\x00\x00\x2b\x2f\xec\xe3"
	"alloc_chrdev_region\0"
	"\x10\x00\x00\x00\x7e\x3a\x2c\x12"
	"_printk\0"
	"\x14\x00\x00\x00\x1e\x70\xe8\x75"
	"cdev_init\0\0\0"
	"\x18\x00\x00\x00\xde\x9f\x8a\x25"
	"module_layout\0\0\0"
	"\x00\x00\x00\x00\x00\x00\x00\x00";

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "C83A1C9ED0F758584F80A0B");
