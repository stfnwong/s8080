/* DISASSEM
 *
 * Actually not much here since this disassembler only
 * has one method.
 */

#ifndef __DISASSEM_H
#define __DISASSEM_H

int disassemble_8080_op(unsigned char *codebuffer, int pc);

#endif /*__DISASSEM_H*/
