/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996, 2011 Oracle and/or its affiliates.  All rights reserved.
 */
/*
 * Copyright (c) 1995, 1996
 *	The President and Fellows of Harvard University.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Margo Seltzer.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *  may be used to endorse or promote products derived from this software
 *  without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id$
 */
#include "db_config.h"
#include "db_int.h"
#pragma hdrstop

static int __db_txnlist_find_internal(ENV*, DB_TXNHEAD*, db_txnlist_type, uint32,  DB_TXNLIST**, int, uint32 *);
/*
 * __db_dispatch --
 *
 * This is the transaction dispatch function used by the db access methods.
 * It is designed to handle the record format used by all the access
 * methods (the one automatically generated by the db_{h,log,read}.sh
 * scripts in the tools directory).  An application using a different
 * recovery paradigm will supply a different dispatch function to txn_open.
 *
 * PUBLIC: int __db_dispatch __P((ENV *,
 * PUBLIC:     DB_DISTAB *, DBT *, DB_LSN *, db_recops, void *));
 */
int __db_dispatch(ENV * env, DB_DISTAB * dtab, DBT * db /* The log record upon which to dispatch */,
	DB_LSN * lsnp /* The lsn of the record being dispatched. */, db_recops redo /* Redo this op (or undo it) */, void * params)
{
	DB_LSN prev_lsn;
	uint32 rectype, status, txnid, urectype;
	int make_call = 0;
	int ret = 0;
	DB_ENV * dbenv = env->dbenv;
	DB_LOG_VRFY_INFO * lvh = NULL;
	DB_TXNHEAD * info = NULL; /* Transaction list. */
	LOGCOPY_32(env, &rectype, db->data);
	LOGCOPY_32(env, &txnid, (uint8 *)db->data+sizeof(rectype));
	/*
	 * Log verification passes a DB_LOG_VRFY_INFO structure, others
	 * pass a DB_TXNHEAD structure.
	 */
	if(redo != DB_TXN_LOG_VERIFY)
		info = static_cast<DB_TXNHEAD *>(params);
	else
		lvh = static_cast<DB_LOG_VRFY_INFO *>(params);
	/* If we don't have a dispatch table, it's hard to dispatch. */
	DB_ASSERT(env, dtab != NULL);
	/*
	 * If we find a record that is in the user's number space and they
	 * have specified a recovery routine, let them handle it.  If they
	 * didn't specify a recovery routine, then we expect that they've
	 * followed all our rules and registered new recovery functions.
	 */
	switch(redo) {
	    case DB_TXN_ABORT:
	    case DB_TXN_APPLY:
	    case DB_TXN_LOG_VERIFY:
	    case DB_TXN_PRINT:
		make_call = 1;
		break;
	    case DB_TXN_OPENFILES:
		/*
		 * We collect all the transactions that have
		 * "begin" records, those with no previous LSN,
		 * so that we do not abort partial transactions.
		 * These are known to be undone, otherwise the
		 * log would not have been freeable.
		 */
		LOGCOPY_TOLSN(env, &prev_lsn, (uint8 *)db->data + sizeof(rectype)+sizeof(txnid));
		if(txnid != 0 && prev_lsn.file == 0 && (ret = __db_txnlist_add(env, info, txnid, TXN_OK, NULL)) != 0)
			return ret;
	    // @fallthrough
	    case DB_TXN_POPENFILES:
		if(oneof4(rectype, DB___dbreg_register, DB___txn_child, DB___txn_ckp, DB___txn_recycle))
			return (dtab->int_dispatch[rectype])(env, db, lsnp, redo, info);
		break;
	    case DB_TXN_BACKWARD_ROLL:
		/*
		 * Running full recovery in the backward pass. In general,
		 * we only process records during this pass that belong
		 * to aborted transactions.  Unfortunately, there are several
		 * exceptions:
		 * 1. If this is a meta-record, one not associated with
		 *  a transaction, then we must always process it.
		 * 2. If this is a transaction commit/abort, we must
		 *  always process it, so that we know the status of
		 *  every transaction.
		 * 3. If this is a child commit, we need to process it
		 *  because the outcome of the child transaction depends
		 *  on the outcome of the parent.
		 * 4. If this is a dbreg_register record, we must always
		 *  process is because they contain non-transactional
		 *  closes that must be properly handled.
		 * 5. If this is a noop, we must always undo it so that we
		 *  properly handle any aborts before a file was closed.
		 * 6. If this a file remove, we need to process it to
		 *  determine if the on-disk file is the same as the
		 *  one being described.
		 */
		switch(rectype) {
		    /*
		 * These either do not belong to a transaction or (regop)
		 * must be processed regardless of the status of the
		 * transaction.
		     */
		    case DB___txn_regop:
		    case DB___txn_recycle:
		    case DB___txn_ckp:
			make_call = 1;
			break;
		    /*
		 * These belong to a transaction whose status must be
		 * checked.
		     */
		    case DB___txn_child:
		    case DB___db_noop:
		    case DB___fop_file_remove:
		    case DB___dbreg_register:
			make_call = 1;

		    // @fallthrough
		    default:
			if(txnid == 0)
				break;
			ret = __db_txnlist_find(env, info, txnid, &status);
			/* If not found, this is an incomplete abort.  */
			if(ret == DB_NOTFOUND)
				return __db_txnlist_add(env,
					info, txnid, TXN_IGNORE, lsnp);
			if(ret != 0)
				return ret;
			/*
			 * If we ignore the transaction, ignore the operation
			 * UNLESS this is a child commit in which case we need
			 * to make sure that the child also gets marked as
			 * ignore.
			 */
			if(status == TXN_IGNORE && rectype != DB___txn_child) {
				make_call = 0;
				break;
			}
			if(status == TXN_COMMIT)
				break;
			/* Set make_call in case we came through default */
			make_call = 1;
			if(status == TXN_OK &&
			   (ret = __db_txnlist_update(env,
				    info, txnid, rectype == DB___txn_prepare ?
				    TXN_PREPARE : TXN_ABORT, NULL, &status, 0)) != 0)
				return ret;
		}
		break;
	    case DB_TXN_FORWARD_ROLL:
		/*
		 * In the forward pass, if we haven't seen the transaction,
		 * do nothing, else recover it.
		 *
		 * We need to always redo DB___db_noop records, so that we
		 * properly handle any commits after the file was closed.
		 */
		switch(rectype) {
		    case DB___txn_recycle:
		    case DB___txn_ckp:
		    case DB___db_noop:
		    case DB___dbreg_register:
			make_call = 1;
			break;

		    default:
			if(txnid == 0)
				status = 0;
			else {
				ret = __db_txnlist_find(env,
					info, txnid, &status);
				if(ret == DB_NOTFOUND)
					/* Break out out of if clause. */
					;
				else if(ret != 0)
					return ret;
				else if(status == TXN_COMMIT) {
					make_call = 1;
					break;
				}
			}
		}
		break;
	    default:
		return __db_unknown_flag(
			env, "__db_dispatch", (uint32)redo);
	}
	if(make_call) {
		/*
		 * If the debug flag is set then we are logging
		 * records for a non-durable update so that they
		 * may be examined for diagnostic purposes.
		 * So only make the call if we are printing,
		 * otherwise we need to extract the previous
		 * lsn so undo will work properly.
		 */
		if(rectype&DB_debug_FLAG) {
			if(redo == DB_TXN_PRINT)
				rectype &= ~DB_debug_FLAG;
			else {
				LOGCOPY_TOLSN(env, lsnp,
					(uint8 *)db->data+
					sizeof(rectype)+
					sizeof(txnid));
				return 0;
			}
		}
		if(rectype >= DB_user_BEGIN) {
			/*
			 * Increment user log count, we can't pass any extra
			 * args into app_dispatch, so this has to be done here.
			 */
			if(redo == DB_TXN_LOG_VERIFY)
				lvh->external_logrec_cnt++;
			if(dbenv->app_dispatch)
				return dbenv->app_dispatch(dbenv, db, lsnp, redo);
			/* No application-specific dispatch */
			urectype = rectype-DB_user_BEGIN;
			if(urectype > dtab->ext_size ||
			   dtab->ext_dispatch[urectype] == NULL) {
				__db_errx(env, DB_STR_A("0512", "Illegal application-specific record type %lu in log", "%lu"), (ulong)rectype);
				return EINVAL;
			}
			return (dtab->ext_dispatch[urectype])(dbenv, db, lsnp, redo);
		}
		else {
			if(rectype > dtab->int_size || dtab->int_dispatch[rectype] == NULL) {
				__db_errx(env, DB_STR_A("0513", "Illegal record type %lu in log", "%lu"), (ulong)rectype);
				if(redo == DB_TXN_LOG_VERIFY)
					lvh->unknown_logrec_cnt++;
				return EINVAL;
			}
			return (dtab->int_dispatch[rectype])(env, db, lsnp, redo, params);
		}
	}
	return 0;
}

/*
 * __db_add_recovery -- Add recovery functions to the dispatch table.
 *
 * We have two versions of this, an external one and an internal one,
 * because application-specific functions take different arguments
 * for dispatch (ENV versus DB_ENV).
 *
 * This is the external version.
 *
 * PUBLIC: int __db_add_recovery __P((DB_ENV *, DB_DISTAB *,
 * PUBLIC:   int (*)(DB_ENV *, DBT *, DB_LSN *, db_recops), uint32));
 */
int __db_add_recovery(DB_ENV * dbenv, DB_DISTAB * dtab, int (*func)__P((DB_ENV*, DBT*, DB_LSN*, db_recops)), uint32 ndx)
{
	size_t i, nsize;
	int ret;
	/* Make sure this is an application-specific record. */
	if(ndx < DB_user_BEGIN) {
		__db_errx(dbenv->env, DB_STR_A("0514", "Attempting to add application-specific record with invalid type %lu", "%lu"), (ulong)ndx);
		return EINVAL;
	}
	ndx -= DB_user_BEGIN;
	/* Check if we have to grow the table. */
	if(ndx >= dtab->ext_size) {
		nsize = ndx+40;
		if((ret = __os_realloc(dbenv->env, nsize*sizeof((dtab->ext_dispatch)[0]), &dtab->ext_dispatch))
		   != 0)
			return ret;
		for(i = dtab->ext_size; i < nsize; ++i)
			(dtab->ext_dispatch)[i] = NULL;
		dtab->ext_size = nsize;
	}
	(dtab->ext_dispatch)[ndx] = func;
	return 0;
}
/*
 * __db_add_recovery_int --
 *
 * Internal version of dispatch addition function.
 *
 *
 * PUBLIC: int __db_add_recovery_int __P((ENV *, DB_DISTAB *,
 * PUBLIC:   int (*)(ENV *, DBT *, DB_LSN *, db_recops, void *), uint32));
 */
int __db_add_recovery_int(ENV * env, DB_DISTAB * dtab, int (*func)__P((ENV*, DBT*, DB_LSN*, db_recops, void *)), uint32 ndx)
{
	size_t i, nsize;
	int ret;
	if(ndx >= DB_user_BEGIN) {
		__db_errx(env, DB_STR_A("0515", "Attempting to add internal record with invalid type %lu", "%lu"), (ulong)ndx);
		return EINVAL;
	}
	/* Check if we have to grow the table. */
	if(ndx >= dtab->int_size) {
		nsize = ndx+40;
		if((ret = __os_realloc(env, nsize*sizeof((dtab->int_dispatch)[0]), &dtab->int_dispatch)) != 0)
			return ret;
		for(i = dtab->int_size; i < nsize; ++i)
			(dtab->int_dispatch)[i] = NULL;
		dtab->int_size = nsize;
	}
	(dtab->int_dispatch)[ndx] = func;
	return 0;
}
/*
 * __db_txnlist_init --
 *	Initialize transaction linked list.
 *
 * PUBLIC: int __db_txnlist_init __P((ENV *, DB_THREAD_INFO *,
 * PUBLIC:     uint32, uint32, DB_LSN *, DB_TXNHEAD **));
 */
int __db_txnlist_init(ENV * env, DB_THREAD_INFO * ip, uint32 low_txn, uint32 hi_txn, DB_LSN * trunc_lsn, DB_TXNHEAD ** retp)
{
	DB_TXNHEAD * headp;
	uint32 size, tmp;
	int ret;
	/*
	 * Size a hash table.
	 *	If low is zero then we are being called during rollback
	 * and we need only one slot.
	 *	Hi maybe lower than low if we have recycled txnid's.
	 *	The numbers here are guesses about txn density, we can afford
	 * to look at a few entries in each slot.
	 */
	if(low_txn == 0)
		size = 1;
	else {
		if(hi_txn < low_txn) {
			tmp = hi_txn;
			hi_txn = low_txn;
			low_txn = tmp;
		}
		tmp = hi_txn-low_txn;
		/* See if we wrapped around. */
		if(tmp > (TXN_MAXIMUM-TXN_MINIMUM)/2)
			tmp = (low_txn-TXN_MINIMUM)+(TXN_MAXIMUM-hi_txn);
		size = tmp/5;
		if(size < 100)
			size = 100;
	}
	if((ret = __os_malloc(env,
		    sizeof(DB_TXNHEAD)+size*sizeof(headp->head), &headp)) != 0)
		return ret;
	memzero(headp, sizeof(DB_TXNHEAD)+size*sizeof(headp->head));
	headp->maxid = hi_txn;
	headp->generation = 0;
	headp->nslots = size;
	headp->gen_alloc = 8;
	headp->thread_info = ip;
	if((ret = __os_malloc(env, headp->gen_alloc*sizeof(headp->gen_array[0]), &headp->gen_array)) != 0) {
		__os_free(env, headp);
		return ret;
	}
	headp->gen_array[0].generation = 0;
	headp->gen_array[0].txn_min = TXN_MINIMUM;
	headp->gen_array[0].txn_max = TXN_MAXIMUM;
	if(trunc_lsn) {
		headp->trunc_lsn = *trunc_lsn;
		headp->maxlsn = *trunc_lsn;
	}
	else {
		ZERO_LSN(headp->trunc_lsn);
		ZERO_LSN(headp->maxlsn);
	}
	ZERO_LSN(headp->ckplsn);

	*retp = headp;
	return 0;
}

#define FIND_GENERATION(hp, txnid, gen) do {                            \
		uint32 __i;                                                  \
		for(__i = 0; __i <= (hp)->generation; __i++)                   \
			/* The range may wrap around the end. */                \
			if((hp)->gen_array[__i].txn_min <                      \
			   (hp)->gen_array[__i].txn_max ?                      \
			   ((txnid) >= (hp)->gen_array[__i].txn_min &&         \
			    (txnid) <= (hp)->gen_array[__i].txn_max) :          \
			   ((txnid) >= (hp)->gen_array[__i].txn_min ||         \
			    (txnid) <= (hp)->gen_array[__i].txn_max))           \
				break;                                          \
		DB_ASSERT(env, __i <= (hp)->generation);                        \
		gen = (hp)->gen_array[__i].generation;                          \
} while(0)

/*
 * __db_txnlist_add --
 *	Add an element to our transaction linked list.
 *
 * PUBLIC: int __db_txnlist_add __P((ENV *,
 * PUBLIC:     DB_TXNHEAD *, uint32, uint32, DB_LSN *));
 */
int __db_txnlist_add(ENV * env, DB_TXNHEAD * hp, uint32 txnid, uint32 status, DB_LSN * lsn)
{
	DB_TXNLIST * elp;
	int ret;
	if((ret = __os_malloc(env, sizeof(DB_TXNLIST), &elp)) != 0)
		return ret;
	LIST_INSERT_HEAD(&hp->head[DB_TXNLIST_MASK(hp, txnid)], elp, links);

	/* Find the most recent generation containing this ID */
	FIND_GENERATION(hp, txnid, elp->u.t.generation);
	elp->type = TXNLIST_TXNID;
	elp->u.t.txnid = txnid;
	elp->u.t.status = status;
	if(txnid > hp->maxid)
		hp->maxid = txnid;
	if(lsn && IS_ZERO_LSN(hp->maxlsn) && status == TXN_COMMIT)
		hp->maxlsn = *lsn;
	DB_ASSERT(env, lsn == NULL || status != TXN_COMMIT || LOG_COMPARE(&hp->maxlsn, lsn) >= 0);
	return 0;
}
/*
 * __db_txnlist_remove --
 *	Remove an element from our transaction linked list.
 *
 * PUBLIC: int __db_txnlist_remove __P((ENV *, DB_TXNHEAD *, uint32));
 */
int __db_txnlist_remove(ENV * env, DB_TXNHEAD * hp, uint32 txnid)
{
	DB_TXNLIST * entry;
	uint32 status;
	return __db_txnlist_find_internal(env, hp, TXNLIST_TXNID, txnid, &entry, 1, &status);
}
/*
 * __db_txnlist_ckp --
 *	Used to record the maximum checkpoint that will be retained
 * after recovery.  Typically this is simply the max checkpoint, but
 * if we are doing client replication recovery or timestamp-based
 * recovery, we are going to virtually truncate the log and we need
 * to retain the last checkpoint before the truncation point.
 *
 * PUBLIC: void __db_txnlist_ckp __P((ENV *, DB_TXNHEAD *, DB_LSN *));
 */
void __db_txnlist_ckp(ENV * env, DB_TXNHEAD * hp, DB_LSN * ckp_lsn)
{

	COMPQUIET(env, 0);
	if(IS_ZERO_LSN(hp->ckplsn) && !IS_ZERO_LSN(hp->maxlsn) && LOG_COMPARE(&hp->maxlsn, ckp_lsn) >= 0)
		hp->ckplsn = *ckp_lsn;
}
/*
 * __db_txnlist_end --
 *	Discard transaction linked list.
 *
 * PUBLIC: void __db_txnlist_end __P((ENV *, DB_TXNHEAD *));
 */
void __db_txnlist_end(ENV * env, DB_TXNHEAD * hp)
{
	DB_TXNLIST * p;
	if(hp) {
		for(uint32 i = 0; i < hp->nslots; i++) {
			while(hp && (p = LIST_FIRST(&hp->head[i])) != NULL) {
				switch(p->type) {
					case TXNLIST_LSN:
					__os_free(env, p->u.l.lsn_stack);
					break;
					case TXNLIST_DELETE:
					case TXNLIST_TXNID:
					default:
					/*
					* Possibly an incomplete DB_TXNLIST; just
					* free it.
					*/
					break;
				}
				LIST_REMOVE(p, links);
				__os_free(env, p);
			}
		}
		__os_free(env, hp->gen_array);
		__os_free(env, hp);
	}
}
/*
 * __db_txnlist_find --
 *	Checks to see if a txnid with the current generation is in the
 *	txnid list.  This returns DB_NOTFOUND if the item isn't in the
 *	list otherwise it returns (like __db_txnlist_find_internal)
 *	the status of the transaction.  A txnid of 0 means the record
 *	was generated while not in a transaction.
 *
 * PUBLIC: int __db_txnlist_find __P((ENV *,
 * PUBLIC:     DB_TXNHEAD *, uint32, uint32 *));
 */
int __db_txnlist_find(ENV * env, DB_TXNHEAD * hp, uint32 txnid, uint32 * statusp)
{
	DB_TXNLIST * entry;
	if(txnid == 0)
		return DB_NOTFOUND;
	return __db_txnlist_find_internal(env, hp, TXNLIST_TXNID, txnid, &entry, 0, statusp);
}
/*
 * __db_txnlist_update --
 *	Change the status of an existing transaction entry.
 *	Returns DB_NOTFOUND if no such entry exists.
 *
 * PUBLIC: int __db_txnlist_update __P((ENV *, DB_TXNHEAD *,
 * PUBLIC:     uint32, uint32, DB_LSN *, uint32 *, int));
 */
int __db_txnlist_update(ENV * env, DB_TXNHEAD * hp, uint32 txnid, uint32 status, DB_LSN * lsn, uint32 * ret_status, int add_ok)
{
	DB_TXNLIST * elp;
	int ret;
	if(txnid == 0)
		return DB_NOTFOUND;
	ret = __db_txnlist_find_internal(env, hp, TXNLIST_TXNID, txnid, &elp, 0, ret_status);
	if(ret == DB_NOTFOUND && add_ok) {
		*ret_status = status;
		return __db_txnlist_add(env, hp, txnid, status, lsn);
	}
	if(ret != 0)
		return ret;
	if(*ret_status == TXN_IGNORE)
		return 0;
	elp->u.t.status = status;
	if(lsn && IS_ZERO_LSN(hp->maxlsn) && status == TXN_COMMIT)
		hp->maxlsn = *lsn;
	return ret;
}
/*
 * __db_txnlist_find_internal --
 *	Find an entry on the transaction list.  If the entry is not there or
 *	the list pointer is not initialized we return DB_NOTFOUND.  If the
 *	item is found, we return the status.  Currently we always call this
 *	with an initialized list pointer but checking for NULL keeps it general.
 */
static int __db_txnlist_find_internal(ENV * env, DB_TXNHEAD * hp, db_txnlist_type type, uint32 txnid, DB_TXNLIST ** txnlistp, int del, uint32 * statusp)
{
	struct __db_headlink * head;
	DB_TXNLIST * p;
	uint32 generation, hash;
	int ret = 0;
	if(hp == NULL)
		return DB_NOTFOUND;
	switch(type) {
	    case TXNLIST_TXNID:
		hash = txnid;
		FIND_GENERATION(hp, txnid, generation);
		break;
	    case TXNLIST_DELETE:
	    case TXNLIST_LSN:
	    default:
		return __env_panic(env, EINVAL);
	}
	head = &hp->head[DB_TXNLIST_MASK(hp, hash)];
	LIST_FOREACH(p, head, links) {
		if(p->type != type)
			continue;
		switch(type) {
		    case TXNLIST_TXNID:
			if(p->u.t.txnid != txnid || generation != p->u.t.generation)
				continue;
			*statusp = p->u.t.status;
			break;

		    case TXNLIST_DELETE:
		    case TXNLIST_LSN:
		    default:
			return __env_panic(env, EINVAL);
		}
		if(del == 1) {
			LIST_REMOVE(p, links);
			__os_free(env, p);
			*txnlistp = NULL;
		}
		else if(p != LIST_FIRST(head)) {
			/* Move it to head of list. */
			LIST_REMOVE(p, links);
			LIST_INSERT_HEAD(head, p, links);
			*txnlistp = p;
		}
		else
			*txnlistp = p;
		return ret;
	}
	return DB_NOTFOUND;
}
/*
 * __db_txnlist_gen --
 *	Change the current generation number.
 *
 * PUBLIC: int __db_txnlist_gen __P((ENV *,
 * PUBLIC:       DB_TXNHEAD *, int, uint32, uint32));
 */
int __db_txnlist_gen(ENV * env, DB_TXNHEAD * hp, int incr, uint32 min, uint32 max)
{
	int ret;
	/*
	 * During recovery generation numbers keep track of "restart"
	 * checkpoints and recycle records.  Restart checkpoints occur
	 * whenever we take a checkpoint and there are no outstanding
	 * transactions.  When that happens, we can reset transaction IDs
	 * back to TXNID_MINIMUM.  Currently we only do the reset
	 * at then end of recovery.  Recycle records occur when txnids
	 * are exhausted during runtime.  A free range of ids is identified
	 * and logged.  This code maintains a stack of ranges.  A txnid
	 * is given the generation number of the first range it falls into
	 * in the stack.
	 */
	if(incr < 0) {
		--hp->generation;
		memmove(hp->gen_array, &hp->gen_array[1],
			(hp->generation+1)*sizeof(hp->gen_array[0]));
	}
	else {
		++hp->generation;
		if(hp->generation >= hp->gen_alloc) {
			hp->gen_alloc *= 2;
			if((ret = __os_realloc(env, hp->gen_alloc*
				    sizeof(hp->gen_array[0]), &hp->gen_array)) != 0)
				return ret;
		}
		memmove(&hp->gen_array[1], &hp->gen_array[0], hp->generation*sizeof(hp->gen_array[0]));
		hp->gen_array[0].generation = hp->generation;
		hp->gen_array[0].txn_min = min;
		hp->gen_array[0].txn_max = max;
	}
	return 0;
}
/*
 * __db_txnlist_lsnadd --
 *	Save the prev_lsn from a txn_child record.
 *
 * PUBLIC: int __db_txnlist_lsnadd __P((ENV *, DB_TXNHEAD *, DB_LSN *));
 */
int __db_txnlist_lsnadd(ENV * env, DB_TXNHEAD * hp, DB_LSN * lsnp)
{
	DB_TXNLIST * elp;
	int ret;
	if(IS_ZERO_LSN(*lsnp))
		return 0;
	LIST_FOREACH(elp, &hp->head[0], links)
	if(elp->type == TXNLIST_LSN)
		break;
	if(elp == NULL) {
		if((ret = __db_txnlist_lsninit(env, hp, lsnp)) != 0)
			return ret;
		return DB_SURPRISE_KID;
	}
	if(elp->u.l.stack_indx == elp->u.l.stack_size) {
		elp->u.l.stack_size <<= 1;
		if((ret = __os_realloc(env, sizeof(DB_LSN)*elp->u.l.stack_size, &elp->u.l.lsn_stack)) != 0) {
			__db_txnlist_end(env, hp);
			return ret;
		}
	}
	elp->u.l.lsn_stack[elp->u.l.stack_indx++] = *lsnp;
	return 0;
}
/*
 * __db_txnlist_lsnget --
 *
 * PUBLIC: int __db_txnlist_lsnget __P((ENV *,
 * PUBLIC:     DB_TXNHEAD *, DB_LSN *, uint32));
 *	Get the lsn saved from a txn_child record.
 */
int __db_txnlist_lsnget(ENV * env, DB_TXNHEAD * hp, DB_LSN * lsnp, uint32 flags)
{
	DB_TXNLIST * elp;
	COMPQUIET(env, 0);
	COMPQUIET(flags, 0);
	LIST_FOREACH(elp, &hp->head[0], links)
	if(elp->type == TXNLIST_LSN)
		break;
	if(elp == NULL || elp->u.l.stack_indx == 0) {
		ZERO_LSN(*lsnp);
		return 0;
	}
	*lsnp = elp->u.l.lsn_stack[--elp->u.l.stack_indx];
	return 0;
}
/*
 * __db_txnlist_lsninit --
 *	Initialize a transaction list with an lsn array entry.
 *
 * PUBLIC: int __db_txnlist_lsninit __P((ENV *, DB_TXNHEAD *, DB_LSN *));
 */
int __db_txnlist_lsninit(ENV * env, DB_TXNHEAD * hp, DB_LSN * lsnp)
{
	int ret;
	DB_TXNLIST * elp = NULL;
	if((ret = __os_malloc(env, sizeof(DB_TXNLIST), &elp)) != 0)
		goto err;
	LIST_INSERT_HEAD(&hp->head[0], elp, links);
	elp->type = TXNLIST_LSN;
	if((ret = __os_malloc(env, sizeof(DB_LSN)*DB_LSN_STACK_SIZE, &elp->u.l.lsn_stack)) != 0)
		goto err;
	elp->u.l.stack_indx = 1;
	elp->u.l.stack_size = DB_LSN_STACK_SIZE;
	elp->u.l.lsn_stack[0] = *lsnp;
	return 0;
err:
	__db_txnlist_end(env, hp);
	return ret;
}

#ifdef DEBUG
//
// __db_txnlist_print --
// Print out the transaction list.
//
void __db_txnlist_print(DB_TXNHEAD * hp)
{
	DB_TXNLIST * p;
	uint32 i;
	char * txntype;
	printf("Maxid: %lu Generation: %lu\n", (ulong)hp->maxid, (ulong)hp->generation);
	for(i = 0; i < hp->nslots; i++)
		LIST_FOREACH(p, &hp->head[i], links) {
			if(p->type != TXNLIST_TXNID) {
				printf("Unrecognized type: %d\n", p->type);
				continue;
			}
			switch(p->u.t.status) {
			    case TXN_OK: txntype = "OK"; break;
			    case TXN_COMMIT: txntype = "commit"; break;
			    case TXN_PREPARE: txntype = "prepare"; break;
			    case TXN_ABORT: txntype = "abort"; break;
			    case TXN_IGNORE: txntype = "ignore"; break;
			    case TXN_EXPECTED: txntype = "expected"; break;
			    case TXN_UNEXPECTED: txntype = "unexpected"; break;
			    default: txntype = "UNKNOWN"; break;
			}
			printf("TXNID: %lx(%lu): %s\n", (ulong)p->u.t.txnid, (ulong)p->u.t.generation, txntype);
		}
		}
#endif
