/**CFile****************************************************************

  FileName    [fraigClass.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [And-Inverter Graph package.]

  Synopsis    []

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: fraigClass.c,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/

#include "aig.h"
#include "stmm.h"

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

static unsigned Aig_ManHashKey( unsigned * pData, int nWords, bool fPhase );

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Creates the equivalence classes of patterns.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Vec_Vec_t * Aig_ManDeriveClassesFirst( Aig_Man_t * p, Aig_SimInfo_t * pInfo )
{
    Vec_Vec_t * vClasses;    // equivalence classes
    stmm_table * tSim2Node;  // temporary hash table hashing key into the class number
    Aig_Node_t * pNode;       
    unsigned uKey;
    int i, * pSpot, Entry, ClassNum;
    assert( pInfo->Type == 1 );
    // fill in the hash table
    tSim2Node = stmm_init_table( stmm_numcmp, stmm_numhash );
    vClasses = Vec_VecAlloc( 100 );
    // enumerate the nodes considered in the equivalence classes
//    Aig_ManForEachNode( p, pNode, i )
    Vec_IntForEachEntry( p->vSat2Var, Entry, i )
    {
        pNode = Aig_ManNode( p, Entry );

        if ( Aig_NodeIsPo(pNode) )
            continue;
        uKey = Aig_ManHashKey( Aig_SimInfoForNode(pInfo, pNode), pInfo->nWords, pNode->fPhase );
        if ( !stmm_find_or_add( tSim2Node, (char *)uKey, (char ***)&pSpot ) ) // does not exist
            *pSpot = (pNode->Id << 1) | 1; // save the node, and do nothing
        else if ( (*pSpot) & 1 ) // this is a node
        {
            // create the class
            ClassNum = Vec_VecSize( vClasses );
            Vec_VecPush( vClasses, ClassNum, (void *)((*pSpot) >> 1) );
            Vec_VecPush( vClasses, ClassNum, (void *)pNode->Id );
            // save the class
            *pSpot = (ClassNum << 1); 
        }
        else // this is a class
        {
            ClassNum = ((*pSpot) >> 1);
            Vec_VecPush( vClasses, ClassNum, (void *)pNode->Id );
        }
    }
    stmm_free_table( tSim2Node );

    // print the classes
    {
        Vec_Ptr_t * vVec;
        printf( "PI/PO = %4d/%4d. Nodes = %7d. SatVars = %7d. Non-trivial classes = %5d: \n", 
            Aig_ManPiNum(p), Aig_ManPoNum(p), 
            Aig_ManNodeNum(p) - Aig_ManPoNum(p), 
            Vec_IntSize(p->vSat2Var), Vec_VecSize(vClasses) );

        Vec_VecForEachLevel( vClasses, vVec, i )
            printf( "%d ", Vec_PtrSize(vVec) );
        printf( "\n" );
    }
    return vClasses;
}

/**Function*************************************************************

  Synopsis    [Computes the hash key of the simulation info.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
unsigned Aig_ManHashKey( unsigned * pData, int nWords, bool fPhase )
{
    static int Primes[10] = { 1009, 2207, 3779, 4001, 4877, 5381, 6427, 6829, 7213, 7919 };
    unsigned uKey;
    int i;
    uKey = 0;
    if ( fPhase )
        for ( i = 0; i < nWords; i++ )
            uKey ^= Primes[i%10] * pData[i];
    else
        for ( i = 0; i < nWords; i++ )
            uKey ^= Primes[i%10] * ~pData[i];
    return uKey;
}


/**Function*************************************************************

  Synopsis    [Updates the equivalence classes using the simulation info.]

  Description [Records successful simulation patterns into the pattern
  storage.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Aig_ManUpdateClasses( Aig_Man_t * p, Aig_SimInfo_t * pInfo, Vec_Vec_t * vClasses )
{
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


