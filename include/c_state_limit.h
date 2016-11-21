/***********************************************************************
 * Copyright (c) 2010-2016 Technische Universitaet Dresden             *
 *                                                                     *
 * This file is part of libadapt.                                      *
 *                                                                     *
 * libadapt is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by*
 * the Free Software Foundation, either version 3 of the License, or   *
 * (at your option) any later version.                                 *
 *                                                                     *
 * This program is distributed in the hope that it will be useful,     *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 * GNU General Public License for more details.                        *
 *                                                                     *
 * You should have received a copy of the GNU General Public License   *
 * along with this program. If not, see <http://www.gnu.org/licenses/>.*
 ***********************************************************************/

/*
 * c_state_limit.h
 *
 *  Created on: 09.01.2012
 *      Author: rschoene
 */

#ifndef CSTATE_LIMIT_H_
#define CSTATE_LIMIT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

int csl_init(void);
int csl_get_deepest_cstate(int cpu, int* nr_cstates);
int csl_set_max_cstate(int cpu, int state);
int csl_fini(void);

#ifdef __cplusplus
}
#endif

#endif /* CSTATE_LIMIT_H_ */
