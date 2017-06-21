
/* Copyright (c) 1997-2017 Miller Puckette and others. */

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#ifndef __m_outlet_h_
#define __m_outlet_h_

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

struct _outconnect {
    struct _outconnect          *oc_next;
    t_pd                        *oc_receiver;
    };

struct _outlet {
    struct _outlet              *o_next;
    t_object                    *o_owner;
    t_outconnect                *o_connections;
    t_symbol                    *o_type;
    };

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

t_outconnect    *outlet_addConnection           (t_outlet *x, t_pd *receiver);

void            outlet_removeConnection         (t_outlet *x, t_pd *receiver);
void            outlet_free                     (t_outlet *x);
void            outlet_moveFirst                (t_outlet *x);
int             outlet_isSignal                 (t_outlet *x);
int             outlet_getSignalIndex           (t_outlet *x);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

static inline t_outlet *outlet_getNext (t_outlet *x)
{
    return x->o_next;
}

static inline t_outconnect *outlet_getConnections (t_outlet *x)
{
    return x->o_connections;
}

static inline t_outconnect *connection_getNext (t_outconnect *x)
{
    return x->oc_next;
}

static inline t_pd *connection_getReceiver (t_outconnect *x)
{
    return x->oc_receiver;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#endif // __m_outlet_h_
