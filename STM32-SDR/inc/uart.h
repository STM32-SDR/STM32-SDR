/**
 ******************************************************************************
 * @file    uart_debug.h
 * @author  Yuuichi Akagawa
 * @version V1.0.0
 * @date    2012/02/27
 * @brief   This file contains all the prototypes for the uart_debug.c
 ******************************************************************************
 * @attention
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * <h2><center>&copy; COPYRIGHT (C)2012 Yuuichi Akagawa</center></h2>
 *
 ******************************************************************************
 */

void uart_init(void);
void uart_deinit(void);
void uart_putc(unsigned char c);
