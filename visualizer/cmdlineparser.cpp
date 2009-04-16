/*****************************************************************************
 * Project: Utilities
 * File:    cmdlineparser.cpp
 * Author:  Jens Wawerla (jwawerla_at_sfu.ca)
 *
 *****************************************************************************
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ******************************************************************************
 * $Log: cmdlineparser.cpp,v $
 * Revision 1.2  2009-03-25 14:49:03  jwawerla
 * Task visualization works now
 *
 * Revision 1.1  2009-03-23 06:25:22  jwawerla
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2005/01/17 22:31:32  jwawerla
 * first time in cvs
 *
 *
 *****************************************************************************/


#include "cmdlineparser.h"


//-----------------------------------------------------------------------------
CCmdLineParser::CCmdLineParser ( const char* progName, bool fgErrorMsg )
{
  m_fgErrorMsg = fgErrorMsg;

  m_progName = new char[strlen ( progName ) ];
  strcpy ( m_progName, progName );

  // init linked list
  m_mainNode = new tNode;
  m_mainNode->next = NULL;

  m_argHelpWidth = 10;
}
//-----------------------------------------------------------------------------
CCmdLineParser::~CCmdLineParser()
{
  tNode* tmpNode = NULL;

  // delete linked list
  while ( m_mainNode != NULL ) {
    tmpNode = m_mainNode->next;
    delete m_mainNode;
    m_mainNode = tmpNode;
  }
}
//-----------------------------------------------------------------------------
void CCmdLineParser::displayHelp()
{

  tNode* tmpNode = m_mainNode->next;

  // print headline
  printf ( "%s ", m_progName );
  while ( tmpNode != NULL ) {
    printf ( "[%s] ", tmpNode->varArg );
    tmpNode = tmpNode->next;
  } // while
  printf ( "\n" );

  // start from the beginning
  tmpNode = m_mainNode->next;

  // now print details for arguments
  while ( tmpNode != NULL ) {

    printf ( " %s", tmpNode->varArg );
    // fill with blanks
    for ( unsigned short i = 0; i < m_argHelpWidth - strlen ( tmpNode->varArg ); i++ )
      printf ( " " );

    printf ( "%s\n", tmpNode->usage );

    // increment linked list
    tmpNode = tmpNode->next;
  } // while
}
//-----------------------------------------------------------------------------
void CCmdLineParser::registerArgument ( void* varPtr,
                                        tVarType varType,
                                        const char* varArg,
                                        unsigned short varNum,
                                        const char* usage )
{
  int iValue;
  tNode* newNode = NULL;
  tNode* tmpNode = m_mainNode;

  // create a new node
  if ( ( varType != INT && varNum == 0 ) ||
       ( varType == STRING && varNum > 1 ) )
    errorMsg ( "Invalied variable length for", varArg );
  else {
    // set boolean to false
    if ( varType == INT && varNum == 0 ) {
      iValue = 0;
      memcpy ( varPtr, &iValue, sizeof ( int ) );
    }
    newNode = new tNode;
    newNode->varPtr = varPtr;
    newNode->varType = varType;
    newNode->varArg = new char[strlen ( varArg ) ];
    strcpy ( newNode->varArg, varArg );
    newNode->usage = new char[strlen ( usage ) ];
    strcpy ( newNode->usage, usage );
    newNode->varNum = varNum;
    newNode->next = NULL;

    // insert into list
    while ( tmpNode->next != NULL )
      tmpNode = tmpNode->next;

    tmpNode->next = newNode;
  }
}
//-----------------------------------------------------------------------------
int CCmdLineParser::parse ( int argc, char **argv )
{
  int i = 1;
  int argN;
  tNode* foundNode;
  double dbValue;
  float flValue;
  int iValue;
  long lValue;
  char* varPtr;

  // go over list of arguments
  while ( i < argc ) {

    if ( findArg ( argv[i], foundNode ) == 1 ) {
      argN = i;
      for ( unsigned short argCnt = 0; argCnt < foundNode->varNum; argCnt++ ) {
        i++; // iterate over arg list
        if ( i < argc ) {

          // convert void pointer in byte count, so we can increment it
          varPtr = ( char* ) foundNode->varPtr;

          switch ( foundNode->varType ) {

            case DOUBLE:
              dbValue = atof ( argv[i] );
              memcpy ( varPtr + ( argCnt*sizeof ( double ) ) , &dbValue, sizeof ( double ) );
              break;

            case FLOAT:
              flValue = ( float ) atof ( argv[i] );
              memcpy ( varPtr + ( argCnt*sizeof ( float ) ), &flValue, sizeof ( float ) );
              break;

            case INT:
              iValue = ( int ) atoi ( argv[i] );
              memcpy ( varPtr + ( argCnt*sizeof ( double ) ), &iValue, sizeof ( int ) );
              break;

            case LONG:
              lValue = atol ( argv[i] );
              memcpy ( varPtr + ( argCnt*sizeof ( long ) ), &lValue, sizeof ( long ) );
              break;

            case STRING:
              // +1 for \0
              memcpy ( varPtr, argv[i], strlen ( argv[i] ) + 1 );
              break;
          } // switch
        }
        else {
          displayHelp();
          errorMsg ( "Not enough values specified for argument: ", argv[argN] );
          return 0; // error
        }
      } // for

      if ( foundNode->varNum == 0 ) {
        iValue = 1;
        memcpy ( foundNode->varPtr, &iValue, sizeof ( int ) );
      }


    }
    else {
      displayHelp();
      errorMsg ( "Argument not defined: ", argv[i] );
      return 0; // error
    }

    i++;
  } // while
  return 1;
}
//-----------------------------------------------------------------------------
int CCmdLineParser::findArg ( char* varArg,
                              tNode* &foundNode )
{
  tNode* tmpNode = m_mainNode->next;

  while ( tmpNode != NULL ) {
    if ( strcmp ( tmpNode->varArg, varArg ) == 0 ) {
      foundNode = tmpNode;
      return 1; // successfully found entry
    }
    // increment list
    tmpNode = tmpNode->next;
  }

  foundNode = NULL;
  return 0; // not found
}
//-----------------------------------------------------------------------------
void CCmdLineParser::errorMsg ( const char* msg1 )
{
  // check if printing error msg is enabled
  if ( m_fgErrorMsg ) {
    printf ( "CCmdLineParser: %s\n", msg1 );
  }
}
//-----------------------------------------------------------------------------
void CCmdLineParser::errorMsg ( const char* msg1, const char* msg2 )
{
  // check if printing error msg is enabled
  if ( m_fgErrorMsg ) {
    printf ( "CCmdLineParser: %s %s\n", msg1, msg2 );
  }
}
//-----------------------------------------------------------------------------

