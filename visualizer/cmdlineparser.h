/*****************************************************************************
 * Project: Utilities
 * File:    cmdlineparser.h
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
 * $Log: cmdlineparser.h,v $
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


#ifndef CMDLINEPARSER_H
#define CMDLINEPARSER_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Provides a command line command parser, register arguments with this class
 * and pass the command line data (argc, argv) to the parser method.
 * The class handles errors and if successfull the registered variables
 * will be filled with data from the command line. There are a few limitations
 * 1. every TYPE has to have a value accept INT
 *    an INT with varNum=0 is treated as boolean
 * 2. every TYPE may have more the one value except those mentioned in 1.
 *    and STRINGS
 * @author Jens Wawerla (jwawerla_at_sfu.ca)
 * @version 0.1 - 09/2004
 */
class CCmdLineParser
{
  public:
    /**
     * Default constuctor
     * @param progName name of the program to be display in help screen
     * @param fgErrorMsg true turns on printing error messages
     */
    CCmdLineParser ( const char* progName, bool fgErrorMsg = true );
    /** Default destructor */
    ~CCmdLineParser();
    /** Type definition for variable types */
    typedef enum {FLOAT, DOUBLE, INT, LONG, STRING} tVarType;
    /**
     * Registers a variable to be parsed for
     * @param varptr pointer to the variable
     * @param varSize size [byte] of the variable
     * @param varArg command line arg symbole
     * @param varNum number of elements this variable has
     * @param usage help line howto use this parameter
     */
    void registerArgument ( void* varPtr,
                            tVarType varType,
                            const char* varArg,
                            unsigned short varNum = 1,
                            const char* usage = NULL );

    /**
     * Parses a command line
     * @param argc number commands
     * @param argv vector of command
     * @return 1 if successfull, 0 otherwise
     */
    int parse ( int argc, char **argv );

  protected:
    /** Type definition of the linked list of agruments */
    typedef struct tNode {
      /** Pointer to the next node */
      tNode* next;
      /** Pointer to the variable */
      void* varPtr;
      /** Type of the variable */
      tVarType varType;
      /** Name of the argument */
      char* varArg;
      /** Number of elements of the variable */
      unsigned short varNum;
      /** Usage of the variable for the help screen */
      char* usage;
    } tNode;
    /**
     * Looks for an argument in the list of registered arguments. Note only
     * looks for the first entry, doesn't handle multiple entries.
     * @param varArg argument to look for
     * @return foundNode pointer to the node that was found or NULL
     * @return 1 if successfully found the entry , 0 otherwise
     */
    int findArg ( char* varArg, tNode* &foundNode );
    /** Displays the help screen */
    void displayHelp();
    /**
     * Sets the width of the arg column in the help screen
     * @param width to be set [characters]
     */
    void setArgHelpColumnWidth ( unsigned short width ) {m_argHelpWidth = width;};
    /**
     * If enabled this method prints out an error message
     * @param msg message to be printed
     */
    void errorMsg ( const char* msg1 );
    /**
     * Overwritten for convenience, takes two parameters
     */
    void errorMsg ( const char* msg1, const char* msg2 );

  private:
    /** Main node of linked argument list */
    tNode* m_mainNode;
    /** Flags whether to print error messages */
    bool m_fgErrorMsg;
    /** Name of the program to be displayed on the help screen */
    char* m_progName;
    /** Number of characters in the argument column of the help screen */
    unsigned short m_argHelpWidth;

};

#endif // CMDLINEPARSER_H
