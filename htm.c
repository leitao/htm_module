/*  
 *  hello-1.c - The simplest kernel module.
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */

void msr_tm_enable(void){
	asm (
			"mfmsr   4 \n ;"
			"li      3, 1 \n;"  	// MSR_TM is at 32th position
			"sldi    3, 3, 32 \n;"  // 1 << 32
			"or      4, 4, 3 \n;"
			"mtmsrd  4 \n;"
			:
			:
			:
	);
}

int init_module(void)
{
	printk(KERN_INFO "HTM in kernel sapce.\n");
	
	msr_tm_enable();

	asm goto (
		"li  0, 22 ;\n"
		"li  9, 44 ;\n"

		"tbegin.  ;\n"
		"beq %[failure] ;\n"
		"tsr. 0 ;\n" // suspend
		"treclaim.  0;\n"
		"li 0, 1 ;\n"
		"tend. ;\n"
		"b %[success] ;\n"
		:
		:
		:		
		: success, failure
		);


failure:
	printk("Test failed\n");
	return -1;
success:
	printk("Test worked\n");
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Goodbye world 1.\n");
}
