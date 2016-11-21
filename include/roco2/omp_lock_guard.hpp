#ifndef OMP_GUARD_HPP_
#define OMP_GUARD_HPP_

#include <omp.h>

class omp_lock {
public:
    omp_lock()
    {
        omp_init_lock(&lock_);
    }
    
    omp_lock(const omp_lock&) = delete;
    omp_lock(omp_lock&&) = delete;
    
    omp_lock& operator=(const omp_lock&) = delete;
    omp_lock& operator=(omp_lock&&) = delete;
    
    omp_lock_t& get()
    {
        return lock_;
    }
    
    explicit operator omp_lock_t&()
    {
        return lock_;
    }
    
    
    
private:
    omp_lock_t lock_;
};

/** This is a class for guard objects using OpenMP
 *  It is adapted from the book 
 *  "Pattern-Oriented Software Architecture". */
class omp_guard {
public:
    /** Acquire the lock and store a pointer to it */
    omp_guard (omp_lock_t &lock)
    : lock_ (&lock)
    , owner_ (false) 
    {
        acquire ();
    }
    
    omp_guard(omp_lock& lock) : lock_ (&(lock.get())), owner_(false)
    {
        
    }
    
    /** Set the lock explicitly */
    void acquire () 
    {
        omp_set_lock (lock_);
        owner_ = true;
    }
    /** Release the lock explicitly (owner thread only!) */
    void release () 
    {
        if (owner_) {
            owner_ = false;
            omp_unset_lock (lock_);
        }
    }
    
    /** Destruct guard object */
    ~omp_guard () 
    { 
        release ();
    }
    
private:
    omp_lock_t *lock_;  // pointer to our lock
    bool owner_;   // is this object the owner of the lock?
    
    // Disallow copies or assignment
    omp_guard (const omp_guard &);
    void operator= (const omp_guard &);
};

#endif /*OMP_GUARD_HPP_*/