/**
 ***********************************************************************************************************************
 * Copyright (c) 2022,Nanjing InnoChip Technology Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 *
 * @file        riscv_const.h
 *
 * @brief       Header of riscv constant defination.
 *
 * @par History:
 * <table>
 * <tr><th>Date                <th> Author              <th> Notes
 * <tr><td>2022-04-06          <td> ICT Team            <td> First version
 * </table>
 *
 ***********************************************************************************************************************
 */

/* Derived from <linux/const.h> */
#ifndef _RISCV_CONST_H
#define _RISCV_CONST_H

#ifdef __ASSEMBLER__
#define _AC(X, Y) X
#define _AT(T, X) X
#else
#define _AC(X, Y) (X##Y)
#define _AT(T, X) ((T)(X))
#endif /* !__ASSEMBLER__*/

#define _BITUL(x)  (_AC(1, UL) << (x))
#define _BITULL(x) (_AC(1, ULL) << (x))

#endif /* _NUCLEI_CONST_H */
