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

#include "c_state_limit.h"

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


/* an fd for every cstate from every cpu, and its original setting */
struct c_state_file
{
    FILE* fd;
    uint8_t default_setting;
};

/* all cstate information for a single cpu */
struct per_cpu
{
    struct c_state_file* c_state_files;
    int current_max;
    int nr_cstates;
};

static struct per_cpu* per_cpu_cstates = NULL;
static int nr_per_cpu_cstates = 0;

int csl_init(void)
{
    unsigned int current_cpu, num_cpus;
    int nr_chars;

    /* get number of CPUs */
    num_cpus = sysconf(_SC_NPROCESSORS_CONF);

    /* calloc is a lot faster than malloc + memset */
    per_cpu_cstates = calloc(num_cpus, sizeof(struct per_cpu));
    /* alloc ok? */
    if (per_cpu_cstates == NULL)
    {
        return ENOMEM;
    }

    nr_per_cpu_cstates = num_cpus;

    /* get number of cstates */
    for (current_cpu = 0; current_cpu < num_cpus; current_cpu++)
    {
        char path_string[256];
        struct dirent** namelist;
        int cpuidle_file;

        nr_chars =
            snprintf(path_string, 256, "/sys/devices/system/cpu/cpu%d/cpuidle/", current_cpu);

        if (nr_chars >= 256)
        {
            free(per_cpu_cstates);
            return ENOMEM;
        }

        /* get array with all possibile states */
        int number_cpuidle_files = scandir(path_string, &namelist, NULL, alphasort);
        /* scandir return -1 if someting went wrong */
        /* namelist shouldn't be a NULL Pointer */
        if (number_cpuidle_files <= 0 && namelist == NULL)
        {
            /* if we are here something went wrong */
            free(per_cpu_cstates);
            return EPERM;
        }

        for (cpuidle_file = 0; cpuidle_file < number_cpuidle_files; cpuidle_file++)
        {
            /* check if file name is state* */
            if (strstr(namelist[cpuidle_file]->d_name, "state") == namelist[cpuidle_file]->d_name)
            {
                /* found a state */
                /* get the state id */
                int state_id = atoi(&(namelist[cpuidle_file]->d_name[5]));
                /* allocate space for state information*/
                if (per_cpu_cstates[current_cpu].nr_cstates < (state_id + 1))
                {
                    per_cpu_cstates[current_cpu].c_state_files =
                        realloc(per_cpu_cstates[current_cpu].c_state_files,
                                (state_id + 1) * sizeof(struct c_state_file));
                    if (per_cpu_cstates[current_cpu].c_state_files == NULL)
                        /* not enough space for an integer?
                         * so we schould break here */
                        return ENOMEM;
                    per_cpu_cstates[current_cpu].nr_cstates = state_id;
                }

                /* now open the "disabled" file and store its value */

                nr_chars = snprintf(path_string, 256,
                                    "/sys/devices/system/cpu/cpu%d/cpuidle/state%d/disable",
                                    current_cpu, state_id);

                if (nr_chars >= 256)
                {
                    free(per_cpu_cstates);
                    for (cpuidle_file = 0; cpuidle_file < number_cpuidle_files; cpuidle_file++)
                    {
                        free(namelist[cpuidle_file]);
                    }
                    return ENOMEM;
                }

                per_cpu_cstates[current_cpu].c_state_files[state_id].fd = fopen(path_string, "rw");

                if (per_cpu_cstates[current_cpu].c_state_files[state_id].fd == NULL)
                {
                    int error = errno;
                    free(per_cpu_cstates);
                    for (cpuidle_file = 0; cpuidle_file < number_cpuidle_files; cpuidle_file++)
                    {
                        free(namelist[cpuidle_file]);
                    }
                    return error;
                }

                /* read disabled to path_string */
                while (fgets(path_string, 256,
                             per_cpu_cstates[current_cpu].c_state_files[state_id].fd) != NULL ||
                       errno == EAGAIN)
                    ;
                per_cpu_cstates[current_cpu].c_state_files->default_setting = atoi(path_string);

                /* set current_max */
                if (state_id > per_cpu_cstates[current_cpu].current_max)
                    per_cpu_cstates[current_cpu].current_max = state_id;

                /* free memory that has been allocated in scandir */
                free(namelist[cpuidle_file]);
            }
        }
    }
    return 0;
}

int csl_get_deepest_cstate(int cpu, int* nr_cstates)
{
    if ((cpu < 0) || (cpu > nr_per_cpu_cstates))
        return EINVAL;

    *nr_cstates = per_cpu_cstates[cpu].nr_cstates;
    return 0;
}

int csl_set_max_cstate(int cpu, int state)
{
    if ((cpu < 0) || (cpu > nr_per_cpu_cstates) || (state > per_cpu_cstates[cpu].nr_cstates) ||
        (state < 0))
        return EINVAL;

    if (state > per_cpu_cstates[cpu].current_max)
    {
        /* enable everything from per_cpu_cstates[cpu].current_max to state */
        int current_state;
        for (current_state = per_cpu_cstates[cpu].current_max; current_state <= state;
             current_state++)
        {
            /* write 0 to per_cpu_cstates[cpu].c_state_files[current.state].fd */
            rewind(per_cpu_cstates[cpu].c_state_files[current_state].fd);
            if (fputs("0", per_cpu_cstates[cpu].c_state_files[current_state].fd))
                return ferror(per_cpu_cstates[cpu].c_state_files[current_state].fd);
            if (fflush(per_cpu_cstates[cpu].c_state_files[current_state].fd))
                return ferror(per_cpu_cstates[cpu].c_state_files[current_state].fd);
        }
    }
    else if (state < per_cpu_cstates[cpu].current_max)
    {
        /* disable everything from per_cpu_cstates[cpu].current_max to state */
        int current_state;
        for (current_state = state + 1; current_state <= per_cpu_cstates[cpu].current_max;
             current_state++)
        {
            /* write 1 to per_cpu_cstates[cpu].c_state_files[current.state].fd */
            rewind(per_cpu_cstates[cpu].c_state_files[current_state].fd);
            if (fputs("1", per_cpu_cstates[cpu].c_state_files[current_state].fd))
                return ferror(per_cpu_cstates[cpu].c_state_files[current_state].fd);
            if (fflush(per_cpu_cstates[cpu].c_state_files[current_state].fd))
                return ferror(per_cpu_cstates[cpu].c_state_files[current_state].fd);
        }
    }

    per_cpu_cstates[cpu].current_max = state;

    /* if state == per_cpu_cstates[cpu].current_max do nothing :) */
    return 0;
}

#ifdef THIS_IS_WHERE_THE_DRAGON_LIVES

int csl_read_from_config(void* vp, struct config_t* cfg, char* buffer, char* prefix)
{
    int was_set = 0;
    config_setting_t* setting;
    struct csl_information* info = vp;

    info->csl_before = -1;
    info->csl_after = -1;
    sprintf(buffer, "%s.%s_before", prefix, CSTATE_LIMIT_CONFIG_STRING);
    setting = config_lookup(cfg, buffer);
    if (setting)
    {
        info->csl_before = config_setting_get_int(setting);
#ifdef VERBOSE
        fprintf(stderr, "%s = %d \n", buffer, info->freq_before);
#endif
        was_set = 1;
    }
    sprintf(buffer, "%s.%s_after", prefix, CSTATE_LIMIT_CONFIG_STRING);
    setting = config_lookup(cfg, buffer);
    if (setting)
    {
        info->csl_after = config_setting_get_int(setting);
#ifdef VERBOSE
        fprintf(stderr, "%s = %d \n", buffer, info->freq_after);
#endif
        was_set = 1;
    }
    return was_set;
}

int csl_process_before(void* vp, int32_t cpu)
{
    int ok = 0;
    struct csl_information* info = vp;
    if (info->csl_before >= 0)
    {
#ifdef VERBOSE
        fprintf(stderr, "changing maximal cstate to %d\n", info->csl_before);
#endif
        ok |= set_max_cstate(cpu, info->csl_before);
    }

#ifdef VERBOSE
    if (ok)
        fprintf(stderr, "Setting max cstate failed %i!\n", ok);
#endif

    return ok;
}

int csl_process_after(void* vp, int32_t cpu)
{
    int ok = 0;
    struct csl_information* info = vp;
    if (info->csl_after >= 0)
    {
#ifdef VERBOSE
        fprintf(stderr, "changing maximal cstate to %d\n", info->csl_after);
#endif
        ok |= set_max_cstate(cpu, info->csl_after);
    }

#ifdef VERBOSE
    if (ok)
        fprintf(stderr, "Setting max cstate failed %i!\n", ok);
#endif

    return ok;
}

#endif

int csl_fini(void)
{
    /* reset original max_cstate and free structures */
    int state = 0, cpu = 0;
    int error = 0;
    for (cpu = 0; cpu < nr_per_cpu_cstates; cpu++)
    {
        for (state = 0; state < per_cpu_cstates[cpu].nr_cstates; state++)
        {
            char buffer[8];
            snprintf(buffer, 8, "%" PRIu8,
                     per_cpu_cstates[cpu].c_state_files[state].default_setting);
            rewind(per_cpu_cstates[cpu].c_state_files[state].fd);
            fputs(buffer, per_cpu_cstates[cpu].c_state_files[state].fd);
            error |= ferror(per_cpu_cstates[cpu].c_state_files[state].fd);
            fflush(per_cpu_cstates[cpu].c_state_files[state].fd);
            error |= ferror(per_cpu_cstates[cpu].c_state_files[state].fd);
            fclose(per_cpu_cstates[cpu].c_state_files[state].fd);
        }
        free(per_cpu_cstates[cpu].c_state_files);
    }
    free(per_cpu_cstates);
    return error;
}
