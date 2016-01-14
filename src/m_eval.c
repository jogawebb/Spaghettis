
/* 
    Copyright (c) 1997-2015 Miller Puckette and others.
*/

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#include "m_pd.h"
#include "m_private.h"
#include "m_macros.h"
#include "m_alloca.h"
#include "s_system.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

extern t_pd pd_objectMaker;
extern t_pd pd_canvasMaker;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static t_symbol *buffer_getSymbolSubstituted (t_atom *v, int argc, t_atom *argv)
{
    t_symbol *s = NULL;
    
    if (IS_DOLLAR (v)) {
        int n = GET_DOLLAR (v);
        if (n > 0 && n <= argc && IS_SYMBOL (argv + n - 1)) { s = GET_SYMBOL (argv + n - 1); }
        
    } else if (IS_DOLLARSYMBOL (v)) {
        s = dollar_substituteDollarSymbol (GET_DOLLARSYMBOL (v), argc, argv);
        
    } else { 
        s = atom_getSymbol (v);
    }
    
    return s;
}

static int buffer_getMessage (t_atom *v, t_pd *object, t_pd **next, t_atom *m, int argc, t_atom *argv)
{
    t_symbol *s = NULL;
    int n, end = 0;
    
    switch (v->a_type) {
    //
    case A_SEMICOLON    :   if (object == &pd_objectMaker) { SET_SYMBOL (m, gensym (";")); }
                            else { 
                                *next = NULL; end = 1; 
                            }
                            break;
    case A_COMMA        :   if (object == &pd_objectMaker) { SET_SYMBOL (m, gensym (",")); }
                            else { 
                                end = 1; 
                            }
                            break;
    case A_FLOAT        :   *m = *v;
                            break;
    case A_SYMBOL       :   *m = *v;
                            break;
    case A_DOLLAR       :   n = GET_DOLLAR (v);
                            if (n > 0 && n <= argc) { *m = *(argv + n - 1); }
                            else if (n == 0) { SET_FLOAT (m, canvas_getdollarzero()); }
                            else {
                                post_error (PD_TRANSLATE ("$: invalid substitution"));
                                SET_FLOAT (m, 0); 
                            }
                            break;
    case A_DOLLARSYMBOL :   s = dollar_substituteDollarSymbol (GET_DOLLARSYMBOL (v), argc, argv);
                            if (s) { SET_SYMBOL (m, s); }
                            else {
                                SET_SYMBOL (m, GET_DOLLARSYMBOL (v));
                            }
                            break;
    default :               end = 1; PD_BUG; 
    //
    }
    
    return end;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void buffer_eval (t_buffer *x, t_pd *object, int argc, t_atom *argv)
{
    int size = x->b_size;
    t_atom *v = x->b_vector;
    t_atom *message = NULL;
    t_atom *m = NULL;
    t_pd *next = NULL;
    int args = 0;
    
    ATOMS_ALLOCA (message, x->b_size);
    
    while (1) {
    //
    while (!object) {       
    //
    t_symbol *s = NULL;
    
    while (size && (IS_SEMICOLON (v) || IS_COMMA (v))) { size--; v++; }
    
    if (size) { s = buffer_getSymbolSubstituted (v, argc, argv); }
    else {
        break;
    }
    
    if (s == NULL || !(object = s->s_thing)) {
        if (s) { post_error (PD_TRANSLATE ("%s: no such object"), s->s_name); }     // --
    } else {
        size--; v++; break;
    }
    //
    }
    
    m    = message; 
    args = 0;
    next = object;
        
    while (1) {
        if (!size || buffer_getMessage (v, object, &next, m, argc, argv)) { break; }
        else {
            m++; args++; size--; v++;
        }
    }
    
    if (args) {
        switch (message->a_type) {
            case A_SYMBOL   :   pd_message (object, GET_SYMBOL (message), args - 1, message + 1); break;
            case A_FLOAT    :   if (args == 1) { pd_float (object, GET_FLOAT (message)); }
                                else { 
                                    pd_list (object, args, message); 
                                } break;
        }
    }
    
    if (!size) { break; }
    
    object = next;
    size--;
    v++;
    //
    }
    
    ATOMS_FREEA (message, x->b_size);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

int binbuf_read(t_buffer *b, char *filename, char *dirname, int crflag)
{
    long length;
    int fd;
    int readret;
    char *buf;
    char namebuf[PD_STRING];
    
    namebuf[0] = 0;
    if (*dirname)
        strcat(namebuf, dirname), strcat(namebuf, "/");
    strcat(namebuf, filename);
    
    if ((fd = sys_open(namebuf, 0)) < 0)
    {
        fprintf(stderr, "open: ");
        perror(namebuf);
        return (1);
    }
    if ((length = lseek(fd, 0, SEEK_END)) < 0 || lseek(fd, 0, SEEK_SET) < 0 
        || !(buf = PD_MEMORY_GET(length)))
    {
        fprintf(stderr, "lseek: ");
        perror(namebuf);
        close(fd);
        return(1);
    }
    if ((readret = read(fd, buf, length)) < length)
    {
        fprintf(stderr, "read (%d %ld) -> %d\n", fd, length, readret);
        perror(namebuf);
        close(fd);
        PD_MEMORY_FREE(buf, length);
        return(1);
    }
        /* optionally map carriage return to semicolon */
    if (crflag)
    {
        int i;
        for (i = 0; i < length; i++)
            if (buf[i] == '\n')
                buf[i] = ';';
    }
    buffer_withString(b, buf, length);

#if 0
    post("binbuf_read "); post_atoms(b->b_size, b->b_vector);
#endif

    PD_MEMORY_FREE(buf, length);
    close(fd);
    return (0);
}

    /* read a binbuf from a file, via the search patch of a canvas */
int binbuf_read_via_canvas(t_buffer *b, char *filename, t_canvas *canvas,
    int crflag)
{
    int filedesc;
    char buf[PD_STRING], *bufptr;
    if ((filedesc = canvas_open(canvas, filename, "",
        buf, &bufptr, PD_STRING, 0)) < 0)
    {
        post_error ("%s: can't open", filename);
        return (1);
    }
    else close (filedesc);
    if (binbuf_read(b, bufptr, buf, crflag))
        return (1);
    else return (0);
}

    /* old version */
int binbuf_read_via_path(t_buffer *b, char *filename, char *dirname,
    int crflag)
{
    int filedesc;
    char buf[PD_STRING], *bufptr;
    if ((filedesc = open_via_path(
        dirname, filename, "", buf, &bufptr, PD_STRING, 0)) < 0)
    {
        post_error ("%s: can't open", filename);
        return (1);
    }
    else close (filedesc);
    if (binbuf_read(b, bufptr, buf, crflag))
        return (1);
    else return (0);
}

#define WBUFSIZE 4096

    /* write a binbuf to a text file.  If "crflag" is set we suppress
    semicolons. */
int binbuf_write(t_buffer *x, char *filename, char *dir, int crflag)
{
    FILE *f = 0;
    char sbuf[WBUFSIZE], fbuf[PD_STRING], *bp = sbuf, *ep = sbuf + WBUFSIZE;
    t_atom *ap;
    int indx, deleteit = 0;
    int ncolumn = 0;

    fbuf[0] = 0;
    if (*dir)
        strcat(fbuf, dir), strcat(fbuf, "/");
    strcat(fbuf, filename);
    
    /*if (!strcmp(filename + strlen(filename) - 4, ".pat") ||
        !strcmp(filename + strlen(filename) - 4, ".mxt"))
    {
        x = binbuf_convert(x, 0);
        deleteit = 1;
    }*/
    
    if (!(f = sys_fopen(fbuf, "w")))
    {
        fprintf(stderr, "open: ");
        /* sys_unixerror(fbuf); */
        goto fail;
    }
    for (ap = x->b_vector, indx = x->b_size; indx--; ap++)
    {
        int length;
            /* estimate how many characters will be needed.  Printing out
            symbols may need extra characters for inserting backslashes. */
        if (ap->a_type == A_SYMBOL || ap->a_type == A_DOLLARSYMBOL)
            length = 80 + strlen(ap->a_w.w_symbol->s_name);
        else length = 40;
        if (ep - bp < length)
        {
            if (fwrite(sbuf, bp-sbuf, 1, f) < 1)
            {
                /* sys_unixerror(fbuf); */
                goto fail;
            }
            bp = sbuf;
        }
        if ((ap->a_type == A_SEMICOLON || ap->a_type == A_COMMA) &&
            bp > sbuf && bp[-1] == ' ') bp--;
        if (!crflag || ap->a_type != A_SEMICOLON)
        {
            atom_toString(ap, bp, (ep-bp)-2);
            length = strlen(bp);
            bp += length;
            ncolumn += length;
        }
        if (ap->a_type == A_SEMICOLON || (!crflag && ncolumn > 65))
        {
            *bp++ = '\n';
            ncolumn = 0;
        }
        else
        {
            *bp++ = ' ';
            ncolumn++;
        }
    }
    if (fwrite(sbuf, bp-sbuf, 1, f) < 1)
    {
        /* sys_unixerror(fbuf); */
        goto fail;
    }

    if (fflush(f) != 0) 
    {
        /* sys_unixerror(fbuf); */
        goto fail;
    }

    if (deleteit)
        buffer_free(x);
    fclose(f);
    return (0);
fail:
    if (deleteit)
        buffer_free(x);
    if (f)
        fclose(f);
    return (1);
}

/* The following routine attempts to convert from max to pd or back.  The
max to pd direction is working OK but you will need to make lots of 
abstractions for objects like "gate" which don't exist in Pd.  conversion
from Pd to Max hasn't been tested for patches with subpatches yet!  */

#define MAXSTACK 1000

#define ISSYMBOL(a, b) ((a)->a_type == A_SYMBOL && \
    !strcmp((a)->a_w.w_symbol->s_name, (b)))

/* LATER make this evaluate the file on-the-fly. */
/* LATER figure out how to log errors */
void binbuf_evalfile(t_symbol *name, t_symbol *dir)
{
    t_buffer *b = buffer_new();
    /*int import = !strcmp(name->s_name + strlen(name->s_name) - 4, ".pat") ||
        !strcmp(name->s_name + strlen(name->s_name) - 4, ".mxt");*/
    int dspstate = canvas_suspend_dsp();
        /* set filename so that new canvases can pick them up */
    glob_setfilename(0, name, dir);
    if (binbuf_read(b, name->s_name, dir->s_name, 0))
        post_error ("%s: read failed; %s", name->s_name, strerror(errno));
    else
    {
            /* save bindings of symbols #N, #A (and restore afterward) */
        t_pd *bounda = gensym("#A")->s_thing, *boundn = s__N.s_thing;
        gensym("#A")->s_thing = 0;
        s__N.s_thing = &pd_canvasMaker;
        /*if (import)
        {
            t_buffer *newb = binbuf_convert(b, 1);
            buffer_free(b);
            b = newb;
        }*/
        buffer_eval(b, 0, 0, 0);
        gensym("#A")->s_thing = bounda;
        s__N.s_thing = boundn;
    }
    glob_setfilename(0, &s_, &s_);
    buffer_free(b);
    canvas_resume_dsp(dspstate);
}

void global_open(void *dummy, t_symbol *name, t_symbol *dir)
{
    t_pd *x = 0;
        /* even though binbuf_evalfile appears to take care of dspstate,
        we have to do it again here, because canvas_startdsp() assumes
        that all toplevel canvases are visible.  LATER check if this
        is still necessary -- probably not. */

    int dspstate = canvas_suspend_dsp();
    t_pd *boundx = s__X.s_thing;
        s__X.s_thing = 0;       /* don't save #X; we'll need to leave it bound
                                for the caller to grab it. */
    binbuf_evalfile(name, dir);
    while ((x != s__X.s_thing) && s__X.s_thing) 
    {
        x = s__X.s_thing;
        pd_vMessage(x, gensym("pop"), "i", 1);
    }
    pd_performLoadbang();
    canvas_resume_dsp(dspstate);
    s__X.s_thing = boundx;
}

    /* save a text object to a binbuf for a file or copy buf */
void binbuf_savetext(t_buffer *bfrom, t_buffer *bto)
{
    int k, n = buffer_getSize(bfrom);
    t_atom *ap = buffer_getAtoms(bfrom), at;
    for (k = 0; k < n; k++)
    {
        if (ap[k].a_type == A_FLOAT ||
            ap[k].a_type == A_SYMBOL &&
                !strchr(ap[k].a_w.w_symbol->s_name, ';') &&
                !strchr(ap[k].a_w.w_symbol->s_name, ',') &&
                !strchr(ap[k].a_w.w_symbol->s_name, '$'))
                    buffer_append(bto, 1, &ap[k]);
        else
        {
            char buf[PD_STRING+1];
            atom_toString(&ap[k], buf, PD_STRING);
            SET_SYMBOL(&at, gensym(buf));
            buffer_append(bto, 1, &at);
        }
    }
    buffer_appendSemicolon(bto);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
