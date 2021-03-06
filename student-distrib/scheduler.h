#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "task.h"

#define HUNDRED_HERTZ_DIVIDER 11932
#define CHANNEL_0 0x40
#define COMMAND_REGISTER 0x43
#define HIGH_EIGHT_BITS 0xFF00
#define LOW_EIGHT_BITS 0xFF
#define MODE_3 0x06
#define MODE_2 0x04
#define LOWBYTE_HIGHBYTE 0x30
#define TIMER_IRQ_NUM 0
/*
Until this point, task switching has been done by either 
executing a new task or by halting an existing one and returning
to the parent task. By adding a scheduler, your OS will 
actively preempt a task in order to switch to the next one. Your
OS scheduler should keep track of all tasks and schedule 
a timer interrupt every 10 to 50 milliseconds in order to
switch to the next task in a round-robin fashion.

When adding a scheduler, it is important to keep in mind 
that tasks running in an inactive terminal should not write to
the screen. In order to enforce this rule, a remapping of 
virtual memory needs to be done for each task. Specifically,
the page tables of a task need to be updated to have a task
 write to non-display memory rather than display memory
when the task is not the active one (see the previous section
*/

void init_PIT();
void next_scheduled_task();

/**
 * @brief The interrupt handler which will call next_scheduled_task only if scheduling flag is set
 * 
 */
void pit_interrupt_handler();

// set this to 1 once we are ready to start scheduling
extern int scheduling_on_flag;

/**
 *	Turn on scheduler
 */
void scheduling_start();

/**
 *	Update the regs for the current user process
 *
 *	@note This function is called at the beginning of the ISR, before any
 *		  handler is executed. Thus the `regs` field should already be valid
 *		  when any handler is executing. Handlers do not need to update `regs`
 *		  unless it is intended.
 *
 *	@param regs: pointer to the saved registers and iret structure
 */
void scheduler_update_taskregs(struct s_regs *regs);

/**
 *
 *	assembly function to do actual iret to switch task
 *
 *	@param reg: the whole structure to iret
 */
void scheduler_iret(struct s_regs* reg);

/**
 * @brief assembly function to get magic number and the regs structure behind it
 * 
 * @return regs_t* pointer to the regs_t structure that current process set up
 */
regs_t *scheduler_get_magic();

/**
 * @brief Given a task object, setup the pages. 
 * 
 * @param proc 
 */
void scheduler_page_clear(task *proc);

/**
 * @brief Given a task object, tear down the pages (by changing present bits)
 * 
 * @param proc 
 */
void scheduler_page_setup(task *proc);

#endif
