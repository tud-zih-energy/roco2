/*
 * The function assumes the following:
 * - the userspace governor is always set
 * - no two threads will call fcf_set_frequency on the same cpu concurrently
 *   nothing really bad will happen if you do, but you might not set the right frequency
 * - cpu is an actual cpu number, never -1 or something stupid
 * - target_frequency is an actual available frequency number
 * - init is called at least once before
 * - finalize will be called once after
 *
 * returns the set frequency on success, -1 on error
 */
long fcf_set_frequency(unsigned int cpu, unsigned long target_frequency);

/*
 * May be called more than one time, but only has an effect once.
 * NOT thread safe!
 *
 * returns 0 on success, -1 on error
 */
int fcf_init_once(int* cpu_status, size_t cpu_status_size);

/*
 * returns 0 on success, -1 on error
 */
int fcf_finalize();
