/********************************************************************\
 * Transaction.h -- defines transaction for xacc (X-Accountant)     *
 * Copyright (C) 1997 Robin D. Clark                                *
 * Copyright (C) 1997, 1998, 1999, 2000 Linas Vepstas               *
 *                                                                  *
 * This program is free software; you can redistribute it and/or    *
 * modify it under the terms of the GNU General Public License as   *
 * published by the Free Software Foundation; either version 2 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU General Public License for more details.                     *
 *                                                                  *
 * You should have received a copy of the GNU General Public License*
 * along with this program; if not, contact:                        *
 *                                                                  *
 * Free Software Foundation           Voice:  +1-617-542-5942       *
 * 59 Temple Place - Suite 330        Fax:    +1-617-542-2652       *
 * Boston, MA  02111-1307,  USA       gnu@gnu.org                   *
 *                                                                  *
\********************************************************************/

#ifndef __XACC_TRANSACTION_H__
#define __XACC_TRANSACTION_H__

#include "config.h"

#include <time.h>

#include "gnc-common.h"
#include "kvp_frame.h"
#include "GNCId.h"
#include "date.h"


/* Values for the reconciled field in Splits */
#define CREC 'c'              /* The Split has been cleared        */
#define YREC 'y'              /* The Split has been reconciled     */
#define FREC 'f'              /* frozen into accounting period     */
#define NREC 'n'              /* not reconciled or cleared         */

/** STRUCTS *********************************************************/
/* The debit & credit pointers are used to implement a double-entry 
 * accounting system.  Basically, the idea with double entry is that
 * there is always an account that is debited, and another that is
 * credited.  These two pointers identify the two accounts. 
 */

/* A split transaction is one which shows up as a credit (or debit) in
 * one account, and pieces of it show up as debits (or credits) in other
 * accounts.  Thus, a single credit-card transaction might be split
 * between "dining", "tips" and "taxes" categories.
 */

typedef struct _account       Account;
typedef struct _account_group AccountGroup;
typedef struct _split         Split;
typedef struct _transaction   Transaction;


/** PROTOTYPES ******************************************************/

/*
 * The xaccConfigSetForceDoubleEntry() and xaccConfigGetForceDoubleEntry()
 *    set and get the "force_double_entry" flag.  This flag determines how
 *    the splits in a transaction will be balanced.
 *
 *    The following values have significance:
 *    0 -- anything goes
 *    1 -- The sum of all splits in a transaction will be
 *         forced to be zero, even if this requires the
 *         creation of additional splits.  Note that a split
 *         whose value is zero (e.g. a stock price) can exist
 *         by itself. Otherwise, all splits must come in at
 *         least pairs.
 *    2 -- splits without parents will be forced into a
 *         lost & found account.  (Not implemented)
 */

void   xaccConfigSetForceDoubleEntry (int force);
int    xaccConfigGetForceDoubleEntry (void);

/*
 * The xaccMallocTransaction() will malloc memory and initialize it.
 *    Once created, it is usually unsafe to merely "free" this memory;
 *    the xaccTransDestroy() method should be called.
 *
 * The xaccInitTransaction() method will initialize the indicated memory 
 *    area.  Handy for on-stack transactions.
 */ 
Transaction * xaccMallocTransaction (void); 
void          xaccInitTransaction (Transaction *);

/* The xaccTransDestroy() method will remove all 
 *    of the splits from each of their accounts, free the memory
 *    associated with them.  This routine must be followed by either
 *    an xaccTransCommitEdit(), in which case the transaction 
 *    memory will be freed, or by xaccTransRollbackEdit(), in which 
 *    case nothing at all is freed, and everything is put back into 
 *    original order.
 */
void          xaccTransDestroy (Transaction *);

/* The xaccTransBeginEdit() method must be called before any changes
 *    are made to a transaction or any of its component splits.  If 
 *    this is not done, errors will result.  If the defer flag is set, 
 *    then the automated re-balancing of all splits in this transaction
 *    is deferred until the xaccTransCommitEdit() call. This allows 
 *    multiple splits to be edited, and prices fiddled with, and the whole
 *    system sent temporarily out of balance, up until the Commit
 *    call is made when double-entry is once again enforced.
 *
 * The xaccTransCommitEdit() method should be used to indicate that 
 *    all of the manipulations on the transaction are complete, and
 *    that these should be made permanent.  Note that this routine
 *    may result in the deletion of the transaction, if the transaction 
 *    is "empty" (has no splits, or * has a single split in it whose 
 *    value is non-zero.)
 *
 * The xaccTransRollbackEdit() routine rejects all edits made, and 
 *    sets the transaction back to where it was before the editing 
 *    started.  This includes restoring any deleted splits, removing
 *    any added splits, and undoing the effects of xaccTransDestroy,
 *    as well as restoring prices, memo's descriptions, etc.
 *
 * The xaccTransIsOpen() method returns TRUE if the transaction
 *    is open for editing. Otherwise, it returns false.
 */
void          xaccTransBeginEdit (Transaction *, int defer);
void          xaccTransCommitEdit (Transaction *);
void          xaccTransRollbackEdit (Transaction *);

gboolean      xaccTransIsOpen (Transaction *trans);

/*
 * The xaccTransGetGUID() subroutine will return the
 *    globally unique id associated with that transaction.
 *
 * The xaccTransLookup() subroutine will return the
 *    transaction associated with the given id, or NULL
 *    if there is no such transaction.
 */
const GUID  * xaccTransGetGUID (Transaction *trans);
Transaction * xaccTransLookup (const GUID *guid);


/* xaccTransGetSlot and xaccTransSetSlot reference the kvp_data 
 * field of the transaction.  kvp_data is used to store arbitrary 
 * strings, numbers, and structures which aren't "official" members
 * of the transaction structure. */

kvp_value   * xaccTransGetSlot(Transaction * trans, const char * key);
void          xaccTransSetSlot(Transaction * trans, const char * key, 
                               const kvp_value * value);

/* The xaccTransSetDateSecs() method will modify the posted date 
 *    of the transaction.  (Footnote: this shouldn't matter to a user,
 *    but anyone modifying the engine should understand that when
 *    xaccTransCommitEdit() is called, the date order of each of the 
 *    component splits will be checked, and will be restored in 
 *    ascending date order.)
 *
 * The xaccTransSetDate() method does the same thing as 
 *    xaccTransSetDateSecs(), but takes a convenient day-month-year format.
 *
 * The xaccTransSetDateTS() method does the same thing as 
 *    xaccTransSetDateSecs(), but takes a struct timespec64.
 *
 * The xaccTransSetDateToday() method does the same thing as 
 *    xaccTransSetDateSecs(), but sets the date to the current system
 *    date/time.
 */
void          xaccTransSetDate (Transaction *, int day, int mon, int year);
void          xaccTransSetDateSecs (Transaction *, time_t);
void          xaccTransSetDateToday (Transaction *);
void          xaccTransSetDateTS (Transaction *, const Timespec *);

void          xaccTransSetDateEnteredSecs (Transaction *, time_t);
void          xaccTransSetDateEnteredTS (Transaction *, const Timespec *);


/* set the Num and Description fields ... */
void          xaccTransSetNum (Transaction *, const char *);
void          xaccTransSetDescription (Transaction *, const char *);

/* The xaccTransSetMemo() and xaccTransSetAction() methods are
 * convenience routines to keep the memo and action fields
 * of two-split transactions in sync.  If the transaction has
 * precisely two splits, then these routines will set the memo 
 * and action of both splits.   Otherwise, they will set the
 * memo and action of the first split (source split) only.
 */
void          xaccTransSetMemo (Transaction *, const char *);
void          xaccTransSetAction (Transaction *, const char *);
void          xaccTransSetDocref (Transaction *, const char *);

/* The xaccTransAppendSplit() method will append the indicated 
 *    split to the collection of splits in this transaction.
 *    If the split is already a part of another transaction,
 *    it will be removed from that transaction first.
 */
void          xaccTransAppendSplit (Transaction *, Split *);

/* The xaccSplitDestroy() method will update its parent account and 
 *    transaction in a consistent manner, resulting in the complete 
 *    unlinking of the split, and the freeing of its associated memory.
 *    The goal of this routine is to perform the removal and destruction
 *    of the split in an atomic fashion, with no chance of accidentally
 *    leaving the accounting structure out-of-balance or otherwise
 *    inconsistent.
 *
 *    If the deletion of the split leaves the transaction "empty",
 *    then the transaction will be marked for deletion.  (It will
 *    not be deleted until the xaccTransCommitEdit() routine is called.)
 *    The transaction is considered "empty" if it has no splits in it, 
 *    or it has only one split left, and that split is not a price split
 *    (i.e. has a non-zero value).  Transactions with only one split in 
 *    them are valid if and only if the value of that split is zero.
 */
void          xaccSplitDestroy (Split *);

/* ------------- gets --------------- */
/* The xaccTransGetSplit() method returns a pointer to each of the 
 *    splits in this transaction.  Valid values for i are zero to 
 *    (number_of__splits-1).  An invalid value of i will cause NULL to
 *    be returned.  A convenient way of cycling through all splits is
 *    to start at zero, and keep incrementing until a null value is returned.
 */
Split *       xaccTransGetSplit (Transaction *trans, int i);

/* These routines return the Num (or ID field), the description, 
 * and the date field.
 */
const char *  xaccTransGetNum (Transaction *);
const char *  xaccTransGetDescription (Transaction *);
const char *  xaccTransGetDocref (Transaction *);
time_t        xaccTransGetDate (Transaction *);
#ifndef SWIG  /* swig chokes on long long */
long long     xaccTransGetDateL (Transaction *);
#endif
void          xaccTransGetDateTS (Transaction *, Timespec *);
void          xaccTransGetDateEnteredTS (Transaction *, Timespec *);

/* The xaccTransGetDateStr() method will return a malloc'ed string
 *    representing the posted date of the transaction, or NULL if
 *    the argument is NULL.
 */
char *        xaccTransGetDateStr (Transaction *trans);

/* The xaccTransCountSplits() method returns the number of splits
 * in a transaction.
 */
int           xaccTransCountSplits (Transaction *trans);


/* xaccIsCommonCurrency returns true if the given currency/security
 * pairs have a currency in common. It uses the same method as
 * xaccTransFindCommonCurrency. This method is useful for determining
 * whether two accounts can have transactions in common.
 */
gboolean xaccIsCommonCurrency(const char *currency_1, const char *security_1,
                              const char *currency_2, const char *security_2);

/* The xaccTransFindCommonCurrency () method returns a string value 
 *    indicating a currency denomination that all of the splits in this
 *    transaction have in common.  This routine is useful in dealing
 *    with currency trading accounts and/or with "stock boxes", where
 *    securities of differing types are moved across accounts.
 *    It returns NULL if the transaction is internally inconsistent.
 *    (This should never ??? happen, as it would be an internal error).
 *
 *    If all of the splits share both a common security and a common currency,
 *    then the string name for the currency is returned.
 */
const char * xaccTransFindCommonCurrency (Transaction *trans);

/* The xaccTransIsCommonCurrency () method compares the input string
 *    to the currency/security denominations of all splits in the
 *    transaction, and returns the input string if it is common with
 *    all the splits, otherwise, it returns NULL.
 *
 *    Note that this routine is *not* merely a string compare on the
 *    value returned by TransFindCommonCurrency().  This is because
 *    all of the splits in a transaction may share *both* a common
 *    currency and a common security.  If the desired match is the
 *    security, a simple string match won't reveal this fact.
 *
 *    This routine is useful in dealing with currency trading accounts
 *    and/or with "stock boxes", where transaction have a security in
 *    common. This routine is useful in dealing with securities of
 *    differing types as they are moved across accounts.
 */
const char * xaccTransIsCommonCurrency (Transaction *trans,
                                        const char * currency);

/* The xaccTransIsCommonExclSCurrency () method compares the input
 *    string to the currency/security denominations of all splits in
 *    the transaction except the one given as parameter, and returns
 *    the input string if it is common with all splits except the one
 *    given, otherwise, it returns NULL.
 *   
 * This is useful when changing one split such that the old entry in
 *    that split is of no relevance when determining whether the new
 *    entry has a common currency with the other splits.
 */
const char * xaccTransIsCommonExclSCurrency (Transaction *trans,
                                             const char * currency, 
					     Split *excl_split);

/* The xaccTransGetImbalance() method returns the total value of the
 *    transaction.  In a pure double-entry system, this imbalance
 *    should be exactly zero, and if it is not, something is broken.
 *    However, when double-entry semantics are not enforced, unbalanced
 *    transactions can sneak in, and this routine can be used to find
 *    out how much things are off by.  The value returned is denominated
 *    in the currency that is returned by the xaccTransFindCommonCurrency()
 *    method.
 */
double xaccTransGetImbalance (Transaction * trans);

/* ------------- splits --------------- */
Split       * xaccMallocSplit (void);


/* xaccSplitGetSlot and xaccSplitSetSlot reference the kvp_data field
 * of the split.  kvp_data is used to store arbitrary strings,
 * numbers, and structures which aren't "official" members of the
 * split structure. */

kvp_value * xaccSplitGetSlot(Split * split, const char * key);
void        xaccSplitSetSlot(Split * split, const char * key, 
                             const kvp_value * value);

/* The xaccSplitGetGUID() subroutine will return the
 *    globally unique id associated with that split.
 *
 * The xaccSplitLookup() subroutine will return the
 *    split associated with the given id, or NULL
 *    if there is no such split.
 */
const GUID * xaccSplitGetGUID (Split *split);
Split      * xaccSplitLookup (const GUID *guid);

/* The memo is an arbitrary string associated with a split.
 *    Users typically type in free form text from the GUI.
 */
void          xaccSplitSetMemo (Split *split, const char *memo);

/* The Action is essentially an arbitrary string, but is 
 * meant to be conveniently limited to a menu of selections 
 * such as  "Buy", "Sell", "Interest", etc.  However,
 * as far as the engine is concerned, its an arbitrary string.
 */
void          xaccSplitSetAction (Split *split, const char *action);

/* docref ==  hook for additional data, etc */
void          xaccSplitSetDocref (Split *, const char *);

/* The Reconcile is a single byte, whose values are typically
 * are "N", "C" and "R"
 */
void          xaccSplitSetReconcile (Split *split, char reconciled_flag);
void          xaccSplitSetDateReconciledSecs (Split *split, time_t time);
void          xaccSplitSetDateReconciledTS (Split *split, Timespec *ts);
void          xaccSplitGetDateReconciledTS (Split *split, Timespec *ts);


/* 
 * The following four functions set the prices and amounts.
 * All of the routines always maintain balance: that is, 
 * invoking any of them will cause other splits in the transaction
 * to be modified so that the net value of the transaction is zero. 
 *
 * The xaccSplitSetShareAmount() method sets the number of shares
 *     that the split should have.  
 *
 * The xaccSplitSetSharePrice() method sets the price of the split.
 *
 * The xaccSplitSetValue() method adjusts the number of shares in 
 *     the split so that the number of shares times the share price
 *     equals the value passed in.
 *
 * The xaccSplitSetSharePriceAndAmount() method will simultaneously
 *     update the share price and the number of shares. This 
 *     is a utility routine that is equivalent to a xaccSplitSetSharePrice()
 *     followed by and xaccSplitSetAmount(), except that it incurs the
 *     processing overhead of balancing only once, instead of twice.
 */

void         xaccSplitSetSharePriceAndAmount (Split *split, double price,
                                              double amount);
void         xaccSplitSetShareAmount (Split *split, double amount);
void         xaccSplitSetSharePrice (Split *split, double price);
void         xaccSplitSetValue (Split *split, double value);
void         xaccSplitSetBaseValue (Split *split, double value,
                                    const char * base_currency);


/* The following four subroutines return the running balance up
 * to & including the indicated split.
 * 
 * The balance is the currency-denominated balance.  For accounts
 * with non-unit share prices, it is correctly adjusted for
 * share prices.
 * 
 * The share-balance is the number of shares. 
 * Price fluctuations do not change the share balance.
 * 
 * The cleared-balance is the currency-denominated balance 
 * of all transactions that have been marked as cleared or reconciled.
 * It is correctly adjusted for price fluctuations.
 * 
 * The reconciled-balance is the currency-denominated balance
 * of all transactions that have been marked as reconciled.
 */

double xaccSplitGetBalance (Split *split);
double xaccSplitGetClearedBalance (Split *split);
double xaccSplitGetReconciledBalance (Split *split);
double xaccSplitGetShareBalance (Split *split);
double xaccSplitGetShareClearedBalance (Split *split);
double xaccSplitGetShareReconciledBalance (Split *split);
double xaccSplitGetCostBasis (Split *split);
double xaccSplitGetBaseValue (Split *split, const char *base_currency);


/* return the parent transaction of the split */
Transaction * xaccSplitGetParent (Split *split);

/* return the memo, action strings */
const char *  xaccSplitGetMemo (Split *split);
const char *  xaccSplitGetAction (Split *split);
const char *  xaccSplitGetDocref (Split *split);

/* return the value of the reconcile flag */
char          xaccSplitGetReconcile (Split *split);
double        xaccSplitGetShareAmount (Split * split);
double        xaccSplitGetSharePrice (Split * split);
double        xaccSplitGetValue (Split * split);

Account *     xaccSplitGetAccount (Split *split);

/********************************************************************\
 * sorting comparison function
 *
 * The xaccTransOrder(ta,tb) method is useful for sorting.
 *    return a negative value if transaction ta is dated earlier than tb, 
 *    return a positive value if transaction ta is dated later than tb,
 *    then compares num, description and docref values, using the strcmp()
 *    c-library routine, returning  what strcmp would return.
 *    Finally, it returns zero if all of the above match.
 *    Note that it does *NOT* compare its member splits.
 *
 * The xaccSplitOrder(sa,sb) method is useful for sorting.
 *    return a negative value if split sa has a smaller currency-value than sb,
 *    return a positive value if split sa has a larger currency-value than sb,
 *    return a negative value if split sa has a smaller share-price than sb,  
 *    return a positive value if split sa has a larger share-price than sb,  
 *    then compares memo and action using the strcmp()
 *    c-library routine, returning  what strcmp would return.
 *    Then it compares the reconciled flags, then the reconciled dates,
 *    Then it strcmps() the docref fields.
 *    Finally, it returns zero if all of the above match.
 *    Note that it does *NOT* compare its parent transaction.
 *
 * The xaccSplitDateOrder(sa,sb) method is useful for sorting.
 *    It is just like the xaccSplitOrder() routine, except that it first
 *    calls xaccTransOrder(), and then does split comparisons only if
 *    xaccTransOrder returned zero (i.e. that everything matched).
 *
 */

int  xaccTransOrder     (Transaction **ta, Transaction **tb);
int  xaccSplitOrder     (Split **sa, Split **sb);
int  xaccSplitDateOrder (Split **sa, Split **sb);

/********************************************************************\
 * Miscellaneous utility routines.
\********************************************************************/
/*
 * count the number of transactions in the null-terminated array
 */
int xaccCountTransactions (Transaction **tarray);

/* count the number of splits in the indicated array */
int xaccCountSplits (Split **sarray);

/* 
 * The xaccGetAccountByName() is a convenience routine that 
 *    is essentially identical to xaccGetPeerAccountFromName(),
 *    except that it accepts the handy transaction as root.
 *
 * The xaccGetAccountByFullName routine is similar, but uses
 *    full names using the given separator.
 */
Account * xaccGetAccountByName (Transaction *, const char *);
Account * xaccGetAccountByFullName (Transaction *trans,
                                    const char *name,
                                    const char separator);

/* 
 * The xaccGetOtherSplit() is a convenience routine that returns
 *    the other of a pair of splits.  If there are more than two 
 *    splits, it returns NULL.
 */
Split * xaccGetOtherSplit (Split *split);

/* The xaccIsPeerSplit() is a convenience routine that returns
 *    a non-zero value if the two splits share a common 
 *    parent transaction, else it returns zero.
 */
int xaccIsPeerSplit (Split *split_1, Split *split_2);

/* The IthSplit() and IthTransaction() routines merely dereference
 *    the lists supplied as arguments; i.e. they return list[i].
 *    These routines are needed by the perl swig wrappers, which
 *    are unable to dereference on their own.
 */
Transaction * IthTransaction (Transaction **tarray, int i);
Split       * IthSplit       (Split **sarray,       int i);

#endif /* __XACC_TRANSACTION_H__ */
